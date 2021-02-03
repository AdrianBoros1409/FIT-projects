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
    public class AddUserToTeamViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly APIClient _apiClient;
        private TeamDetailModelInner _teamModel;
        private Guid? _userId;

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
        public ICommand AddUserCommand { get; set; }

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

        public AddUserToTeamViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;

            GoBackCommand = new RelayCommand(GoBack);
            AddUserCommand = new RelayCommand<UserListModelInner>(AddUserToTeam);
            mediator.Register<AddUserToTeamMessage>(AddUser);
            mediator.Register<GoToHomeMessage>(GoToHome);
        }

        private async void AddUserToTeam(UserListModelInner obj)
        {
            try
            {
                var user = await _apiClient.UserGetByIdAsync(obj.Id);
                await _apiClient.AddUserToTeamAsync(user, TeamModel.Id);
                _mediator.Send(new TeamInfoMessage
                {
                    TeamId = TeamModel.Id, 
                    UserId = _userId
                });
                TeamModel = null;
            }
            catch (Exception)
            {
                MessageBox.Show(Resources.Texts.TextResources.NoUser_Message);
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

        private async void AddUser(AddUserToTeamMessage obj)
        {
            if (obj.UserId != null)
            {
                _userId = obj.UserId;
            }
            TeamModel = await _apiClient.GetTeamByIdAsync(obj.Id);
            Users = new ObservableCollection<UserListModelInner>(await _apiClient.UsersNotInTeamAsync(TeamModel.Id));
        }
    }
}
