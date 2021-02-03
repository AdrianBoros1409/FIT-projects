using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using Fitter.BL.Model;

namespace Fitter.BL.Messages
{
    public class UserInfoMessage : IMessage
    {
        public Guid? Id { get; set; }
    }
}
