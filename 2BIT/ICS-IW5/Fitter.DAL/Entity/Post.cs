using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using Fitter.DAL.Entity.Base;

namespace Fitter.DAL.Entity
{
    public class Post : BasePost
    {
        [Required(ErrorMessage = "Title is missing.")]
        public string Title { get; set; }
        public ICollection<Comment> Comments { get; set; } = new List<Comment>();
        public Guid CurrentTeamId { get; set; }
        public Team Team { get; set; }
    }
}
