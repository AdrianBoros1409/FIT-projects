using System;
using Fitter.DAL.Entity.Base;

namespace Fitter.DAL.Entity
{
    public class UsersInTeam : BaseEntity
    {
        public Guid UserId { get; set; }
        public User User { get; set; }
        public Guid TeamId { get; set; }
        public Team Team { get; set; }
    }
}
