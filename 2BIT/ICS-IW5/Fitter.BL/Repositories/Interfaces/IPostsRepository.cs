using System;
using System.Collections.Generic;
using Fitter.BL.Model;

namespace Fitter.BL.Repositories.Interfaces
{
    public interface IPostsRepository
    {
        void Create(PostModel post);
        void Delete(Guid id);

        IList<PostModel> GetPostsForTeam(Guid id);
        IList<Guid> SearchInPosts(string substring, Guid id);

        PostModel GetById(Guid id);
    }
}