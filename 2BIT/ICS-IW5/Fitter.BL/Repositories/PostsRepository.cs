using System;
using System.Collections.Generic;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using System.Linq;
using Fitter.BL.Factories;
using Fitter.BL.Repositories.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace Fitter.BL.Repositories
{
    public class PostsRepository : IPostsRepository
    {
        private readonly IDbContextFactory _fitterDbContext;
        private readonly IMapper _mapper;

        public PostsRepository(IDbContextFactory fitterDbContext, IMapper mapper)
        {
            this._fitterDbContext = fitterDbContext;
            this._mapper = mapper;
        }

        public void Create(PostModel post)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = _mapper.MapPostToEntity(post);
                dbContext.Posts.Add(entity);
                dbContext.Entry(entity.Team).State = EntityState.Unchanged;
                dbContext.Entry(entity.Team.Admin).State = EntityState.Unchanged;
                dbContext.Entry(entity.Author).State = EntityState.Unchanged;
                dbContext.SaveChanges();
            }
        }

        public void Delete(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Posts.First(t => t.Id == id);
                dbContext.Remove(entity);
                dbContext.SaveChanges();
            }
        }

        public IList<PostModel> GetPostsForTeam(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return (dbContext.Posts.Where(data => data.CurrentTeamId == id)
                        .Include(t => t.Team)
                        .Include(a => a.Author)
                        .Join(dbContext.Comments, data => data.Id, comm => comm.CurrentPostId, (data, comm) => new {data, comm})
                        .OrderByDescending(t => t.comm.Created)
                        .Select(t => t.data))
                        .Union(dbContext.Posts.Where(post => post.CurrentTeamId == id && !post.Comments.Any()).OrderByDescending(p => p.Created))
                        .Select(e => _mapper.MapPostModelFromEntity(e)).ToList();
            }
        }

        public PostModel GetById(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Posts
                    .Include(a => a.Author)
                    .Include(t => t.Team)
                    .First(t => t.Id == id);
                return _mapper.MapPostModelFromEntity(entity);
            }
        }

        public IList<Guid> SearchInPosts(string substring, Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.Posts
                    .Include(a => a.Author)
                    .Include(t => t.Team)
                    .Where(t => t.CurrentTeamId == id)
                    .Where(e => e.Title.Contains(substring) || e.Text.Contains(substring))
                    .Select(e => e.Id).ToList();
            }
        }
    }
}