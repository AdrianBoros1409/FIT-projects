using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
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
    public class AddUScreenViewModel : ViewModelBase
    {
        private readonly APIClient _apiClient;
        private readonly IMediator _mediator;
        private UserDetailModelInner _model;
        public ICommand AddUserCommand { get; set; }
        public ICommand GoBackCommand { get; set; }
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
        public AddUScreenViewModel(IMediator mediator, APIClient apiClient)
        {
            _apiClient = apiClient;
            _mediator = mediator;
            AddUserCommand = new RelayCommand(AddUser, CanAddUser);
            GoBackCommand = new RelayCommand(GoBack);
            mediator.Register<AddUMessage>(NewUser);
            mediator.Register<GoToHomeMessage>(GoHome);
        }

        private void GoHome(GoToHomeMessage obj)
        {
            Model = null;
        }

        private void GoBack()
        {
            Model = null;
        }

        private bool CanAddUser(object obj)
        {
            PasswordBox pwBox = obj as PasswordBox;
            return Model != null && !string.IsNullOrWhiteSpace(Model.Email)
                   && !string.IsNullOrWhiteSpace(pwBox.Password) 
                   && !string.IsNullOrWhiteSpace(Model.FirstName) 
                   && !string.IsNullOrWhiteSpace(Model.LastName);
        }

        private async void AddUser(object obj)
        {
            PasswordBox pwBox = obj as PasswordBox;
            Model.Password = pwBox.Password;
            
            try
            {
                await _apiClient.UserGetByEmailAsync(Model.Email);
                MessageBox.Show(Resources.Texts.TextResources.SameUser_Message);
            }
            catch 
            {
                await _apiClient.UserCreateAsync(Model);
                Model = null;
                pwBox.Password = "";
            }
        }

        private void NewUser(AddUMessage obj)
        {
            Model = new UserDetailModelInner();
        }
    }
}
