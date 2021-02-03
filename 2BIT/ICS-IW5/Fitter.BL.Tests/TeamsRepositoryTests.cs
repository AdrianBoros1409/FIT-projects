using System;
using System.Diagnostics.SymbolStore;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using Fitter.BL.Factories;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Fitter.BL.Repositories.Interfaces;
using Fitter.DAL;
using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace Fitter.BL.Tests
{
    public class TeamsRepositoryTests
    {
        [Fact]
        public void AddUserToTeam()
        {
            var sut = CreateSUT();
            var users = CreateUser();
            var teamModel = new TeamDetailModel()
            {
                Admin = new UserDetailModel()
                {
                    Id = Guid.NewGuid(),
                    LastName = "Michaela",
                    FirstName = "Sucha",
                    Password = "miskasucha",
                    Email = "michaella159@seznam.cz"
                },
                Id = Guid.NewGuid(),
                Name = "Miskin team",
                Description = "Team pre Michaelu",
            };

            var userModel = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                LastName = "Modra",
                FirstName = "Diana",
                Password = "asassad",
                Email = "dianka@gmail.com"
            };

            var createdTeam = sut.Create(teamModel);
            var createdUser = users.Create(userModel);
            sut.AddUserToTeam(createdUser, createdTeam.Id); //

            var teams = sut.GetTeamsForUser(userModel.Id);
            Assert.Equal(1, teams.Count);
        }

        [Fact]
        public void RemoveUserFromTeam()
        {
            var sut = CreateSUT();
            var users = CreateUser();
            var model = new TeamDetailModel()
            {
                Admin = new UserDetailModel()
                {
                    Id = Guid.NewGuid(),
                    LastName = "Vaclav",
                    FirstName = "Siroky",
                    Password = "dobreheslo",
                    Email = "sirokyvaclav@gmail.com"
                },
                Id = Guid.NewGuid(),
                Name = "Ministri",
                Description = "Team pre ministrov",
            };
            var user = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                LastName = "Michal",
                FirstName = "Kruty",
                Password = "najlepsieheslo",
                Email = "kruty@gmail.com"
            };

            var createdUser = users.Create(user);
            var createdTeam = sut.Create(model);
            sut.AddUserToTeam(createdUser, model.Id);
            sut.RemoveUserFromTeam(createdUser, createdTeam.Id);

            var userCount = users.GetUsersInTeam(createdTeam.Id).ToList().Count;
            Assert.Equal(0, userCount);
        }

        [Fact]
        public void CreateTeam()
        {
            var sut = CreateSUT();
            var model = new TeamDetailModel()
            {
                Admin = new UserDetailModel()
                {
                    LastName = "Eva",
                    FirstName = "Novakova",
                    Password = "asassad",
                    Email = "novakovaEva@gmail.com"
                },
                Name = "Team",
                Description = "Team pre IW5/ICS",
            };
            var createdTeam = sut.Create(model);
            Assert.NotNull(createdTeam);
        }

        [Fact]
        public void GetTeamById()
        {
            var sut = CreateSUT();
            var model = new TeamDetailModel()
            {
                Admin = new UserDetailModel()
                {
                    LastName = "Alfonz",
                    FirstName = "Mokry",
                    Password = "147258369",
                    Email = "dark@gmail.com"
                },
                Name = "The bests",
                Description = "Alfonzov team",
            };

            var createdTeam = sut.Create(model);
            var foundedTeam = sut.GetById(createdTeam.Id);

            Assert.Equal(createdTeam.Id, foundedTeam.Id);
        }

        [Fact]
        public void DeleteTeam()
        {
            var sut = CreateSUT();
            var admin = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                LastName = "Doe",
                FirstName = "John",
                Password = "123",
                Email = "john@doe.com"
            };

            var team = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = admin,
                Name = "Team1",
                Description = "Team1 description"
            };

            var createdTeam = sut.Create(team);
            sut.Delete(createdTeam.Id);

            Assert.Throws<NullReferenceException>(() => sut.GetById(createdTeam.Id));
        }

        [Fact]
        public void CheckTeamExisting()
        {
            var sut = CreateSUT();
            var model = new TeamDetailModel()
            {
                Admin = new UserDetailModel()
                {
                    LastName = "Vaclav",
                    FirstName = "Siroky",
                    Password = "dobreheslo",
                    Email = "sirokyvaclav@gmail.com"
                },
                Name = "Ministri",
                Description = "Team pre ministrov",
            };

            var createdTeam = sut.Create(model);
            var teamExisting = sut.Exists(createdTeam.Name);
            Assert.True(teamExisting);
        }

        [Fact]
        public void CheckTeamExistingAfterDelete()
        {
            var sut = CreateSUT();
            var user = new UserDetailModel()
            {
                LastName = "Michal",
                FirstName = "Kruty",
                Password = "najlepsieheslo",
                Email = "kruty@gmail.com"
            };
            var model = new TeamDetailModel()
            {
                Admin = user,
                Name = "Michalov team",
                Description = "Najlepsi z najlepsich",
            };

            var createdTeam = sut.Create(model);
            sut.Delete(createdTeam.Id);
            var exist = sut.Exists(createdTeam.Name);
            Assert.False(exist);

            
        }

        private TeamsRepository CreateSUT()
        {

            return new TeamsRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }

        private UsersRepository CreateUser()
        {
            return new UsersRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }
    }
}