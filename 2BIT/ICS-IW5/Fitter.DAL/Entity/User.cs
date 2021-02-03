using System.Collections.Generic;
using Fitter.DAL.Entity.Base;

namespace Fitter.DAL.Entity
{   
    public class User : BaseEntity
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public string FullName => $"{FirstName} {LastName}";
        public string Email { get; set; }
        public string Password { get; set; }
        public ICollection<UsersInTeam> UsersInTeams { get; set; } = new List<UsersInTeam>();
        public ICollection<Comment> Comments { get; set; } = new List<Comment>();
        public ICollection<Post> Posts { get; set; } = new List<Post>();
    }
}