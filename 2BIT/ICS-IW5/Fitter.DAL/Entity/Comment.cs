using System;
using Fitter.DAL.Entity.Base;

namespace Fitter.DAL.Entity
{
    public class Comment : BasePost
    {
        public Guid CurrentPostId { get; set; }
        public Post Post { get; set; }
    }
}
