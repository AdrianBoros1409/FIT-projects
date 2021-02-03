using System.Collections.Generic;
using Fitter.BL.Model;
using Fitter.DAL.Entity;

namespace Fitter.BL.Mapper.Interface
{
    public interface IMapper
    {
        User MapUserToEntity(UserDetailModel model);
        UserDetailModel MapUserDetailModelFromEntity(User user);
        UserListModel MapUserListModelFromEntity(User user);

        Team MapTeamToEntity(TeamDetailModel model);
        TeamDetailModel MapTeamDetailModelFromEntity(Team team);
        TeamListModel MapTeamListModelFromEntity(Team team);

        Post MapPostToEntity(PostModel model);
        PostModel MapPostModelFromEntity(Post post);

        Comment MapCommentToEntity(CommentModel model);
        CommentModel MapCommentModelFromEntity(Comment comment);
    }
}