using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using System.Linq;
using Fitter.BL.Factories;
using Fitter.BL.Repositories.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace Fitter.BL.Repositories
{
    public class CommentsRepository : ICommentsRepository
    {
        private readonly IDbContextFactory _fitterDbContext;
        private readonly IMapper _mapper;
        public CommentsRepository(IDbContextFactory fitterDbContext, IMapper mapper)
        {
            this._fitterDbContext = fitterDbContext;
            this._mapper = mapper;
        }

        public void Create(CommentModel comment)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = _mapper.MapCommentToEntity(comment);
                dbContext.Entry(entity).State = EntityState.Unchanged;
                dbContext.Comments.Add(entity);
                /*dbContext.Entry(entity.Author).State = EntityState.Unchanged;
                dbContext.Entry(entity.Post).State = EntityState.Unchanged;
                dbContext.Entry(entity.Post.Team).State = EntityState.Unchanged;*/
                dbContext.SaveChanges();
            }
        }

        public void Delete(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Comments.First(t => t.Id == id);
                dbContext.Remove(entity);
                dbContext.SaveChanges();
            }
        }

        public List<CommentModel> GetCommentsForPost(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.Posts
                    .Include(t => t.Team)
                        .ThenInclude(a => a.Admin)
                    .Include(a => a.Author)
                    .Include(c => c.Comments)
                        .ThenInclude(k => k.Author)
                    .First(p => p.Id == id)
                    .Comments
                    .OrderBy(e => e.Created)
                    .Select(e => _mapper.MapCommentModelFromEntity(e)).ToList();
            }
        }

        public IList<Guid> SearchInComments(string substring, Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var posts = dbContext.Comments
                    .Include(k => k.Post)
                    .Where(e => e.Text.Contains(substring))
                    .Select(e => e.Post)
                    .Where(e => e.CurrentTeamId == id).Distinct();

                return posts.Include(k => k.Team).Include(k => k.Author).Select(e => e.Id).ToList();
            }
        }
    }
}