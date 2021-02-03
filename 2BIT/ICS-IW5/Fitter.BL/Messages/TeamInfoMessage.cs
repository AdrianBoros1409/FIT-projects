using System;
using System.Collections.Generic;
using System.Text;

namespace Fitter.BL.Messages
{
    public class TeamInfoMessage : IMessage
    {
        public Guid? TeamId { get; set; }
        public Guid? UserId { get; set; }
    }
}
