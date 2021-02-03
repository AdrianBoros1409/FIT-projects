using System;
using Fitter.BL.Messages;

namespace Fitter.BL.Services
{
    public interface IMediator
    {
        void Register<TMessage>(Action<TMessage> action)
            where TMessage : IMessage;
        void Send<TMessage>(TMessage message)
            where TMessage : IMessage;
        void UnRegister<TMessage>(Action<TMessage> action)
            where TMessage : IMessage;
    }
}