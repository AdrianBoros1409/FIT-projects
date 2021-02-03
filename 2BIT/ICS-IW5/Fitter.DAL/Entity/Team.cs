using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using Fitter.DAL.Entity.Base;

namespace Fitter.DAL.Entity
{
    public class Team : BaseEntity 
    {
        [Required]
        [StringLength(20, ErrorMessage = "{0} length must be between {2} and {1}.", MinimumLength = 3)]
        public string Name { get; set; }
        [Required(ErrorMessage = "Description is missing.")]
        public string Description { get; set; }
        public User Admin { get; set; }
        public ICollection<UsersInTeam> UsersInTeams { get; set; } = new List<UsersInTeam>();
        public ICollection<Post> Posts { get; set; } = new List<Post>();
    }
}
