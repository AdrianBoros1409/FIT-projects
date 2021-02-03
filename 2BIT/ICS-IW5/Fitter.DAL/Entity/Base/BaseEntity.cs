using System;

namespace Fitter.DAL.Entity.Base
{
    public abstract class BaseEntity : IEntity
    {
        public Guid Id { get; set; }
    }
}