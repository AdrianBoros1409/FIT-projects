using System;
using System.Collections.Generic;
using System.Linq;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Xunit;

namespace Fitter.BL.Tests
{
    public class PostsRepositoryTests
    {
        [Fact]
        public void CreatePost()
        {
            var sut = CreateSUT();
            var users = CreateUsers();
            var teams = CreateTeams();

            var adminOfTeam = new UserDetailModel()
            {
                FirstName = "John",
                LastName = "Smith",
                Email = "johnsmith@gmail.com",
                Password = "asdf12345",
            };

            var authorOfPost = new UserDetailModel()
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
                Admin = adminOfTeam,
                Description = "Team of John",
                Name = "Johny"
            };

            var model = new PostModel()
            {
                Id = Guid.NewGuid(),
                Author = authorOfPost,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = team,
                Text = "Randon text in post",
                Title = "Random"
            };
            users.Create(adminOfTeam);
            users.Create(authorOfPost);
            var CreatedTeam = teams.Create(team);
            sut.Create(model);

            Assert.Equal(1, sut.GetPostsForTeam(CreatedTeam.Id).Count);
        }

        [Fact]
        public void DeletePost()
        {
            var sut = CreateSUT();
            var users = CreateUsers();
            var teams = CreateTeams();

            var adminOfTeam = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Lisa",
                LastName = "Black",
                Email = "LisaBlack@gmail.com",
                Password = "lisalisalisa"
            };

            var authorOfPost = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Amanda",
                LastName = "White",
                Email = "amandaWhite@gmail.com",
                Password = "lisalisalisa"
            };

            var team = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = adminOfTeam,
                Description = "Post in Lisa´s team",
                Name = "Lisa Post"
            };
            var model = new PostModel()
            {
                Id = Guid.NewGuid(),
                Author = authorOfPost,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = team
            };

            users.Create(authorOfPost);
            var createdTeam = teams.Create(team);
            sut.Create(model);
            sut.Delete(model.Id);
            var posts = sut.GetPostsForTeam(createdTeam.Id).ToList();
            Assert.Empty(posts);
        }

        [Fact]
        public void GetPostFromTeam()
        {
            var sut = CreateSUT();
            var users = CreateUsers();
            var teams = CreateTeams();

            var adminOfTeam = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Jozef",
                LastName = "Velky",
                Email = "jozefko@gmail.com",
                Password = "jozef67@gmail.com"
            };

            var authorOfPost = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Martin",
                LastName = "Brnensky",
                Password = "14758963",
                Email = "martin12313@gmail.com"
            };

            var team = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = adminOfTeam
            };

            var model = new PostModel
            {
                Id = Guid.NewGuid(),
                Author = authorOfPost,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = team,
                Title = "Prvy post",
                Text = "Toto je prvy post v skupine",
            };

            users.Create(authorOfPost);
            var createdTeam = teams.Create(team);
            sut.Create(model);
            var retrievedPost = sut.GetPostsForTeam(createdTeam.Id);
            Assert.NotNull(retrievedPost);
        }

        private PostsRepository CreateSUT()
        {
            return new PostsRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }

        private UsersRepository CreateUsers()
        {
            return new UsersRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }
        private TeamsRepository CreateTeams()
        {
            return new TeamsRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }
    }
}