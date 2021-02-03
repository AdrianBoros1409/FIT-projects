using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Fitter.DAL.Entity.Base
{
    public abstract class BasePost:BaseEntity
    {
        public Guid? CurrentAuthorId { get; set; }
        public User Author { get; set; }

        [Required]
        public string Text { get; set; }
        public string Created { get; set; }
    }
}
