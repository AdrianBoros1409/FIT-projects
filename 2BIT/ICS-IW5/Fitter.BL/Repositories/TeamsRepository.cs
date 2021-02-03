using System;
using System.Collections.Generic;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using System.Linq;
using Fitter.BL.Factories;
using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;

namespace Fitter.BL.Repositories
{
    public class TeamsRepository : ITeamsRepository
    {
        private readonly IDbContextFactory _fitterDbContext;
        private readonly IMapper _mapper;
        public TeamsRepository(IDbContextFactory fitterDbContext, IMapper mapper)
        {
            this._fitterDbContext = fitterDbContext;
            this._mapper = mapper;
        }

        public TeamDetailModel Create(TeamDetailModel team)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = _mapper.MapTeamToEntity(team);
                dbContext.Teams.Add(entity);
                dbContext.SaveChanges();
                return _mapper.MapTeamDetailModelFromEntity(entity);
            }
        }

        public TeamDetailModel GetById(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Teams.Include(t => t.Admin).FirstOrDefault( t => t.Id == id);
                return _mapper.MapTeamDetailModelFromEntity(entity);
            }
        }

        public void AddUserToTeam(UserDetailModel user, Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var userEntity = _mapper.MapUserToEntity(user);
                var teamEntity = _mapper.MapTeamToEntity(GetById(id));

                var uitEntity = new UsersInTeam()
                {
                    Id = Guid.NewGuid(),
                    User = userEntity,
                    Team = teamEntity
                };

                dbContext.Teams
                    .First(k => k.Id == id)
                    .UsersInTeams
                    .Add(uitEntity);

                dbContext.UsersInTeams.Update(uitEntity);

                dbContext.SaveChanges();
            }
        }

        public void RemoveUserFromTeam(UserDetailModel user, Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = _mapper.MapUserToEntity(user);
                var selected = (dbContext.UsersInTeams
                        .Include(t => t.Team)
                        .Include(t => t.User)
                        .Where(data => (data.TeamId == id && data.User == entity)))
                        .First();

                dbContext.UsersInTeams.Remove(selected);
                dbContext.SaveChanges();
            }
        }

        public IList<TeamListModel> GetTeamsForUser(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.UsersInTeams
                    .Include(u => u.User)
                    .Include(t => t.Team)
                    .Where(data => data.UserId == id)
                    .Select(data => data.Team)
                    .Select(e => _mapper.MapTeamListModelFromEntity(e)).ToList();
            }
        }

        public bool Exists(string name)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                return dbContext.Teams.Any(t => t.Name == name);
            }
        }

        public void Delete(Guid id)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = dbContext.Teams.First(t => t.Id == id);
                dbContext.Remove(entity);
                dbContext.SaveChanges();
            }
        }

        public void Update(TeamDetailModel team)
        {
            using (var dbContext = _fitterDbContext.CreateDbContext())
            {
                var entity = _mapper.MapTeamToEntity(team);
                dbContext.Teams.Update(entity);
                dbContext.SaveChanges();
            }
        }
    }
}
