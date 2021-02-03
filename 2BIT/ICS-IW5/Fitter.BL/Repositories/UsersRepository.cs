using System;
using System.Collections.Generic;
using System.Linq;
using Fitter.BL.Factories;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;
using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query.Expressions;

namespace Fitter.BL.Repositories
{
    public class UsersRepository : IUsersRepository
    {
        private readonly IDbContextFactory _fitterDbContext;
        private readonly IMapper _mapper;
        public UsersRepository(IDbContextFactory fitterDbContext, IMapper mapper)
        {
            this._fitterDbContext = fitterDbContext;
            this._mapper = mapper;
        }

        public UserDetailModel Create(UserDetailModel user)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var passwordHasher = new PasswordHasher(user.Password);
                user.Password = passwordHasher.GetHashedPassword();

                var entity = _mapper.MapUserToEntity(user);
                dbContext.Users.Add(entity);
                dbContext.SaveChanges();
                return _mapper.MapUserDetailModelFromEntity(entity);
            }
        }

        public UserDetailModel GetById(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Users.First(t => t.Id == id);
                return _mapper.MapUserDetailModelFromEntity(entity);
            }
        }

        public UserDetailModel GetByEmail(string email)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Users.First(t => t.Email == email);
                return _mapper.MapUserDetailModelFromEntity(entity);
            }
        }

        public IList<UserListModel> GetUsersInTeam(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.UsersInTeams.Include(p => p.User)
                    .Include(t => t.Team)
                    .Where(data => data.TeamId == id)
                    .Select(data => _mapper.MapUserListModelFromEntity(data.User)).ToList();
            }
        }

        public IList<UserListModel> GetUsersNotInTeam(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.Users
                    .Include(u => u.UsersInTeams)?
                    .Where(p => p.UsersInTeams
                        .All(t => t.TeamId != id))
                    .Select(e => _mapper.MapUserListModelFromEntity(e)).ToList();
            }
        }

        public string GetLastActivity(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var comments = dbContext.Users
                    .Include(c => c.Comments)
                    .First(e => e.Id == id).Comments.Select(e => e).ToList();

                var posts = dbContext.Users
                    .Include(c => c.Posts)
                    .First(e => e.Id == id).Posts.Select(e => e).ToList();

                var lastComment = "01.01.1970 00:00:01";
                var lastPost = "01.01.1970 00:00:01";
                if (comments.Count > 0)
                {
                    lastComment = comments.OrderByDescending(e => e.Created).First().Created;
                }

                if (posts.Count > 0)
                {
                    lastPost = posts.OrderByDescending(e => e.Created).First().Created;
                }
               
                var commentDt = DateTime.Parse(lastComment);
                var postDt = DateTime.Parse(lastPost);
                var result = DateTime.Compare(commentDt, postDt);

                if (result == 0)
                {
                    return "-";
                }

                return result > 0 ? lastComment : lastPost;
            }
        }
    }
}
