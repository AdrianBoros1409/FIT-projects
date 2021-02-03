using System;
using System.Collections.Generic;
using System.Text;

namespace Fitter.BL.Messages
{
    public class TeamSelectedMessage : IMessage
    {
        public Guid? Id { get; set; }
    }
}
