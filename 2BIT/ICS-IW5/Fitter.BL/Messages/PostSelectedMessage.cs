﻿using System;
using System.Collections.Generic;
using System.Text;

namespace Fitter.BL.Messages
{
    public class PostSelectedMessage : IMessage
    {
        public Guid Id { get; set; }
    }
}
