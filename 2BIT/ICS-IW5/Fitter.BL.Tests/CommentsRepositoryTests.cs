using System;
using System.Collections.Generic;
using System.Linq;
using Fitter.BL.Factories;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Xunit;

namespace Fitter.BL.Tests
{
    public class CommentsRepositoryTests
    {
        [Fact]
        public void CreateComment()
        {
            var sut = CreateSUT();
            var posts = CreatePost();
            var users = CreateUser();
            var postAuthor = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Adrian",
                LastName = "New",
                Email = "adrian@new.com",
                Password = "abcdefgh"
            };
            var adminOfTeam = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Harry",
                LastName = "Callum",
                Email = "harry@callum.com",
                Password = "asdf131"
            };
            var teamForPost = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = adminOfTeam,
                Description = "Harry Callum Team",
                Name = "United"
            };
            var postModel = new PostModel()
            {
                Id = Guid.NewGuid(),
                Author = postAuthor,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = teamForPost,
                Text = "Post in harry callum team",
                Title = "Main Title"
            };
            var commentAuthor = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Oliver",
                LastName = "Jake",
                Email = "olijake@outlook.com",
                Password = "oli12345"
            };
            var model = new CommentModel()
            {
                Id = Guid.NewGuid(),
                Author = commentAuthor,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Post = postModel,
                Text = "Comment on post in team"
            };
            posts.Create(postModel);
            users.Create(commentAuthor);
            sut.Create(model);
            Assert.NotNull(model);        
        }

        [Fact]
        public void DeleteComment()
        {
            var sut = CreateSUT();
            var posts = CreatePost();
            var users = CreateUser();

            var authorOfPost = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "James",
                LastName = "Charlie",
                Email = "james@charlie.com",
                Password = "charlie258"
            };

            var authorOfComment = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Oliver",
                LastName = "Jake",
                Email = "olijake@outlook.com",
                Password = "oli12345"
            };

            var teamOfPost = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = new UserDetailModel()
                {
                    Id = Guid.NewGuid(),
                    FirstName = "William",
                    LastName = "Damian",
                    Email = "william@damian.com",
                    Password = "password"
                },
                Description = "William Damian Team",
                Name = "Manchester"
            };

            var postModel = new PostModel()
            {
                Id = Guid.NewGuid(),
                Author = authorOfPost,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = teamOfPost,
                Text = "Best footbal club",
                Title = "Football"
            };

            var commentModel = new CommentModel()
            {
                Id = Guid.NewGuid(),
                Author = authorOfComment,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Post = postModel,
                Text = "Comment on post in team"
            };
            posts.Create(postModel);
            users.Create(authorOfComment);
            sut.Create(commentModel);
            sut.Delete(commentModel.Id);
            Assert.Throws<InvalidOperationException>(() => sut.GetCommentsForPost(postModel.Id));
        }

        [Fact]
        public void GetCommentsInPost()
        {
            var sut = CreateSUT();
            var users = CreateUser();
            var posts = CreatePost();

            var user = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Oscar",
                LastName = "Rhys",
                Email = "oscar@rhys.com",
                Password = "password"
            };

            var authorOfPost = new UserDetailModel()
            {
                Id = Guid.NewGuid(),
                FirstName = "Thomas",
                LastName = "Joe",
                Email = "thomas@joe.com",
                Password = "password"
            };

            var team = new TeamDetailModel()
            {
                Id = Guid.NewGuid(),
                Admin = user,
                Description = "Jacob Jacob Team",
                Name = "Sea Dogs"
            };

            var post = new PostModel()
            {
                Id = Guid.NewGuid(),
                Author = authorOfPost,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Team = team,
                Text = "Post in Jacob Jacob team",
                Title = "Main Title of post"
            };

            var model = new CommentModel()
            {
                Id = Guid.NewGuid(),
                Author = user,
                Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                Post = post,
                Text = "Unbelievable comment"
            };
            users.Create(user);
            posts.Create(post);
            sut.Create(model);
            Assert.Equal("Unbelievable comment", model.Text);
        }

        private CommentsRepository CreateSUT()
        {
            return new CommentsRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }

        private PostsRepository CreatePost()
        {
            return new PostsRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }

        private UsersRepository CreateUser()
        {
            return new UsersRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }
    }
}