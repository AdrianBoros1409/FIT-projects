using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
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
    public class RemoveUserFromTeamViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly APIClient _apiClient;
        private TeamDetailModelInner _teamModel;

        private ObservableCollection<UserListModelInner> _users;

        public ObservableCollection<UserListModelInner> Users
        {
            get => _users;
            set
            {
                if (Equals(value, Users))
                    return;

                _users = value;
                OnPropertyChanged();
            }
        }
        public ICommand GoBackCommand { get; set; }
        public ICommand RemoveUserCommand { get; set; }

        public TeamDetailModelInner TeamModel
        {
            get => _teamModel;
            set
            {
                if (Equals(value, TeamModel))
                {
                    return;
                }

                _teamModel = value;
                OnPropertyChanged();
            }
        }

        public RemoveUserFromTeamViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;

            GoBackCommand = new RelayCommand(GoBack);
            RemoveUserCommand = new RelayCommand<UserListModelInner>(RemoveUser);
            mediator.Register<RemoveUserFromTeamMessage>(RemovingUsers);
            mediator.Register<GoToHomeMessage>(GoToHome);
        }

        private async void RemoveUser(UserListModelInner obj)
        {
            if (obj.Id != TeamModel.Admin.Id)
            {
                var user = await _apiClient.UserGetByIdAsync(obj.Id);
                await _apiClient.RemoveUserFromTeamAsync(user, TeamModel.Id);
                _mediator.Send(new TeamInfoMessage { TeamId = TeamModel.Id });
                TeamModel = null;
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.KickAdmin_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
            
        }

        private void GoToHome(GoToHomeMessage obj)
        {
            TeamModel = null;
        }

        private void GoBack()
        {
            TeamModel = null;
        }

        private async void RemovingUsers(RemoveUserFromTeamMessage obj)
        {
            TeamModel = await _apiClient.GetTeamByIdAsync(obj.Id);
            Users = new ObservableCollection<UserListModelInner>(await _apiClient.UsersInTeamAsync(TeamModel.Id));
        }
    }
}
