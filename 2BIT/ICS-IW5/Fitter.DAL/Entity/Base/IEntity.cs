using System;

namespace Fitter.DAL.Entity.Base
{
    public interface IEntity
    {
        Guid Id { get; set; }
    }
}