using System;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using Fitter.DAL.Entity;

namespace Fitter.BL.Mapper
{
    public class Mapper : IMapper
    {
        #region User Mapping

        public User MapUserToEntity(UserDetailModel model)
        {
            if (model == null)
            {
                return null;
            }

            return new User
            {
                Id = model.Id,
                FirstName = model.FirstName,
                LastName = model.LastName,
                Email = model.Email,
                Password = model.Password
            };
        }

        public UserDetailModel MapUserDetailModelFromEntity(User user)
        {
            if (user == null)
            {
                return null;
            }

            return new UserDetailModel
            {
                Id = user.Id,
                FirstName = user.FirstName,
                LastName = user.LastName,
                Email = user.Email,
                Password = user.Password
            };
        }

        public UserListModel MapUserListModelFromEntity(User user)
        {
            return new UserListModel
            {
                Id = user.Id,
                Fullname = user.FullName
            };

        }

        #endregion

        #region Team Mapping

        public Team MapTeamToEntity(TeamDetailModel model)
        {
            if (model == null)
            {
                return null;
            }

            return new Team
            {
                Id = model.Id,
                Name = model.Name,
                Description = model.Description,
                Admin = MapUserToEntity(model.Admin)
            };
        }

        public TeamDetailModel MapTeamDetailModelFromEntity(Team team)
        {
            return new TeamDetailModel
            {
                Id = team.Id,
                Name = team.Name,
                Description = team.Description,
                Admin = MapUserDetailModelFromEntity(team.Admin)
            };
        }

        public TeamListModel MapTeamListModelFromEntity(Team team)
        {
            return new TeamListModel
            {
                Id = team.Id,
                Name = team.Name
            };
        }

        #endregion

        #region Post Mapping

        public Post MapPostToEntity(PostModel model)
        {
            return new Post
            {
                Id = model.Id,
                Title = model.Title,
                Author = MapUserToEntity(model.Author),
                Text = model.Text,
                Created = model.Created,
                Team = MapTeamToEntity(model.Team)
            };
        }

        public PostModel MapPostModelFromEntity(Post post)
        {
            return new PostModel
            {
                Id = post.Id,
                Author = MapUserDetailModelFromEntity(post.Author),
                Created = post.Created,
                Team = MapTeamDetailModelFromEntity(post.Team),
                Text = post.Text,
                Title = post.Title
            };
        }

        #endregion

        #region Comment Mapping

        public Comment MapCommentToEntity(CommentModel model)
        {
            return new Comment
            {
                Id = model.Id,
                Author = MapUserToEntity(model.Author),
                Created = model.Created,
                Text = model.Text,
                Post = MapPostToEntity(model.Post)
            };
        }

        public CommentModel MapCommentModelFromEntity(Comment comment)
        {
            return new CommentModel
            {
                Id = comment.Id,
                Author = MapUserDetailModelFromEntity(comment.Author),
                Created = comment.Created,
                Text = comment.Text,
                Post = MapPostModelFromEntity(comment.Post)
            };
        }

        #endregion
    }
}
