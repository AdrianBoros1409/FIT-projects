using System;
using System.Linq;
using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Xunit;

namespace Fitter.DAL.Tests
{
    public class InMemoryFitterCreationTests
    {
        public class FitterDbContextTests
        {
            private IFitterDbContext dbContextFitter;

            public FitterDbContextTests()
            {
                dbContextFitter = new InMemoryFitterDbContext();
            }

            [Fact]
            public void CreateUser()
            {
                var user = new User
                {
                    LastName = "Danko",
                    FirstName = "Daniel",
                    Email = "danko123@azet.sk",
                    Password = "danulko12313"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Users.Add(user);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedUser = dbContext.Users
                        .First(x => x.Id == user.Id);
                    Assert.NotNull(retrievedUser);
                }
            }

            [Fact]
            public void RemoveUser()
            {
                var user = new User
                {
                    LastName = "Vajs",
                    FirstName = "Michal",
                    Email = "vajsko42@gmail.com",
                    Password = "ronaldo7"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Users.Add(user);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Users.Remove(user);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedUser = dbContext.Users
                        .FirstOrDefault(x => x.Id == user.Id);
                    Assert.Null(retrievedUser);
                }
            }

            [Fact]
            public void CreateTeam()
            {
                var team = new Team()
                {
                    Name = "IW5",
                    Description = "Skupina pre IW5 projekt"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Teams.Add(team);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedTeam = dbContext.Teams
                        .First(x => x.Id == team.Id);
                    Assert.NotNull(retrievedTeam);
                }
            }

            [Fact]
            public void RemoveTeam()
            {
                var team = new Team()
                {
                    Name = "ICS",
                    Description = "Skupina pre ICS projekt"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Teams.Add(team);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Teams.Remove(team);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedTeam = dbContext.Teams
                        .FirstOrDefault(x => x.Id == team.Id);
                    Assert.Null(retrievedTeam);
                }
            }

            [Fact]
            public void CreatePost()
            {
                var post = new Post()
                {
                    Title = "Prvy post v skupine",
                    Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                    Text = "Toto je prvy post v tejto skupine"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Posts.Add(post);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedPost = dbContext.Posts
                        .First(x => x.Id == post.Id);
                    Assert.NotNull(retrievedPost);
                }
            }

            [Fact]
            public void RemovePost()
            {
                var post = new Post()
                {
                    Title = "Prvy post v skupine",
                    Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                    Text = "Toto je prvy post v tejto skupine"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Posts.Add(post);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Posts.Remove(post);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedPost = dbContext.Posts
                        .FirstOrDefault(x => x.Id == post.Id);
                    Assert.Null(retrievedPost);
                }
            }

            [Fact]
            public void CreateComment()
            {
                var comment = new Comment()
                {
                    Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                    Text = "Koment na post"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Comments.Add(comment);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedComment = dbContext.Comments
                        .First(x => x.Id == comment.Id);
                    Assert.NotNull(retrievedComment);
                }
            }

            [Fact]
            public void RemoveComment()
            {
                var comment = new Comment()
                {
                    Created = DateTime.Now.ToString("MM/dd/yyyy HH:mm"),
                    Text = "Koment na post"
                };

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Comments.Add(comment);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    dbContext.Comments.Remove(comment);
                    dbContext.SaveChanges();
                }

                using (var dbContext = dbContextFitter.CreateDbContext())
                {
                    var retrievedComment = dbContext.Comments
                        .FirstOrDefault(x => x.Id == comment.Id);
                    Assert.Null(retrievedComment);
                }
            }
        }
    }
}