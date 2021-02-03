using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Security;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Fitter.App.API;
using Fitter.App.API.Models;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;
using Fitter.BL.Messages;
using System.IO;

namespace Fitter.App.ViewModels
{
   public class LoginPanelViewModel : ViewModelBase
   {
        private readonly APIClient _apiClient;
        private readonly IMediator _mediator;
        public UserDetailModelInner Model { get; set; }

        public string Email { get; set; }
        public string Language { get; set; }
        public ICommand SelectedLanguage { get; set; }
        public ICommand NewUserCommand { get; set; }
        public LoginPanelViewModel(IMediator mediator, APIClient apiClient)
        {
            _apiClient = apiClient;
            _mediator = mediator;
            SelectedLanguage = new RelayCommand(ChangeLanguage);
            NewUserCommand = new RelayCommand(LoginUser, CanLogin);
        }

        private void ChangeLanguage()
        {
            Language = Language.Substring(Language.Length - 2);
            using (StreamWriter writer = new StreamWriter("language.txt", false))
            {
                writer.Write(Language);
            }
            MessageBox.Show(Fitter.App.Resources.Texts.TextResources.Restart_Message);
        }

        private async void LoginUser(object obj)
        {
            try
            {
                PasswordBox pwBox = obj as PasswordBox;
                var password = pwBox.Password;
                pwBox.Clear();

                Model = await _apiClient.UserGetByEmailAsync(Email);
                var data = new PasswordComparer();
                if (data.ComparePassword(password, Model.Password))
                {
                    _mediator.Send(new UserLoginMessage { Id = Model.Id });
                }
                else
                {
                    MessageBox.Show(Resources.Texts.TextResources.WrongPassword_Message);
                }
            }
            catch (Exception)
            {
                MessageBox.Show(Resources.Texts.TextResources.NoUser_Message);
            }
            Model = null;
        }
        private bool CanLogin(object obj)
        {
            PasswordBox pwBox = obj as PasswordBox;
            return !string.IsNullOrWhiteSpace(Email)
                   && !string.IsNullOrWhiteSpace(pwBox.Password);
        }

    }
}
