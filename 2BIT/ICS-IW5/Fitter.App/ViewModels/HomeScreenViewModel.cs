using System;
using System.ComponentModel;
using System.Windows.Input;
using Fitter.App.API;
using Fitter.App.API.Models;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.BL.Messages;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class HomeScreenViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly APIClient _apiClient;
        private UserDetailModelInner _model;

        public ICommand GoToCreateT { get; set; }
        public ICommand GoToCreateU { get; set; }
        public ICommand LogOut { get; set; }

        public UserDetailModelInner Model
        {
            get { return _model; }
            set
            {
                if (Equals(value, Model))
                    return;

                _model = value;
                OnPropertyChanged();
            }
        }

        public HomeScreenViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;
            GoToCreateT = new RelayCommand(NewTeam);
            GoToCreateU = new RelayCommand(NewUser);
            LogOut = new RelayCommand(LogOutUser);
            mediator.Register<UserLoginMessage>(UserLogin);
        }

        private void LogOutUser()
        {
            Model = null;
            _mediator.Send(new LogOutMessage());
        }

        private void NewUser()
        {
            _mediator.Send(new AddUMessage());
        }

        private void NewTeam()
        {
            _mediator.Send(new AddTMessage{Id = Model.Id});
        }

        private async void UserLogin(UserLoginMessage obj)
        {
            Guid id = Guid.Parse(obj.Id.ToString());
            Model = await _apiClient.UserGetByIdAsync(id);
        }
    }
}