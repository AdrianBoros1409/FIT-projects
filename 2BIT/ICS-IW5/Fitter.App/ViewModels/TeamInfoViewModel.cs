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
    public class TeamInfoViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly APIClient _apiClient;
        private TeamDetailModelInner _teamDetailModel;
        private UserDetailModelInner _userDetailModel;
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
        public ICommand GoToUserCommand { get; set; }
        public ICommand AddUserToTeamCommand { get; set; }
        public ICommand RemoveUserFromTeamCommand { get; set; }
        public ICommand LeaveTeamCommand { get; set; }
        public ICommand DeleteTeamCommand { get; set; }
        public TeamDetailModelInner TeamDetailModel
        {
            get => _teamDetailModel;
            set
            {
                if (Equals(value, TeamDetailModel))
                    return;

                _teamDetailModel = value;
                OnPropertyChanged();
            }
        }

        public UserDetailModelInner UserDetailModel {
            get => _userDetailModel;
            set {
                if (Equals(value, UserDetailModel))
                    return;

                _userDetailModel = value;
                OnPropertyChanged();
            }
        }

        public TeamInfoViewModel(IMediator mediator, APIClient apiClient)
        {
            this._mediator = mediator;
            _apiClient = apiClient;

            GoBackCommand = new RelayCommand(GoBack);
            GoToUserCommand = new RelayCommand<UserListModelInner>(GoToUser);
            AddUserToTeamCommand = new RelayCommand(AddUser);
            RemoveUserFromTeamCommand = new RelayCommand(RemoveUser);
            DeleteTeamCommand = new RelayCommand(DeleteTeam);
            LeaveTeamCommand = new RelayCommand(LeaveTeam);

            mediator.Register<TeamInfoMessage>(ShowTeamInfo);
            mediator.Register<GoToHomeMessage>(GoHome);
            mediator.Register<ResetTeamMessage>(ResetTeam);
        }

        private void ResetTeam(ResetTeamMessage obj)
        {
            TeamDetailModel = null;
        }

        private async void LeaveTeam()
        {
            if (UserDetailModel.Id != TeamDetailModel.Admin.Id)
            {
                var result = MessageBox.Show(Resources.Texts.TextResources.LeaveTeam_Message, Resources.Texts.TextResources.LeaveTeam_MTitle, MessageBoxButton.YesNo);
                if (result == MessageBoxResult.Yes)
                {
                    await _apiClient.RemoveUserFromTeamAsync(UserDetailModel, TeamDetailModel.Id);
                    _mediator.Send(new UpdatedTeamsMessage());
                    _mediator.Send(new GoToHomeMessage());
                }
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.LeaveAdmin_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void DeleteTeam()
        {
            if (UserDetailModel.Id == TeamDetailModel.Admin.Id)
            {
                var result = MessageBox.Show(Resources.Texts.TextResources.DeleteTeam_Message, Resources.Texts.TextResources.DeleteTeam_MTitle, MessageBoxButton.YesNo);
                if (result == MessageBoxResult.Yes)
                {
                    await _apiClient.DeleteTeamAsync(TeamDetailModel.Id);
                    _mediator.Send(new UpdatedTeamsMessage());
                    _mediator.Send(new GoToHomeMessage());
                }
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.CanNotDeleteTeam_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void RemoveUser()
        {
            _mediator.Send(new RemoveUserFromTeamMessage{Id = TeamDetailModel.Id});

        }

        private void AddUser()
        {
            _mediator.Send(new AddUserToTeamMessage { Id = TeamDetailModel.Id });
        }

        private void GoToUser(UserListModelInner user)
        {
            _mediator.Send(new UserInfoMessage{Id = user.Id});
        }

        private void GoBack()
        {
            TeamDetailModel = null;
        }

        private void GoHome(GoToHomeMessage obj)
        {
            TeamDetailModel = null;
        }

        private async void ShowTeamInfo(TeamInfoMessage obj)
        {
            if (obj.UserId == null && UserDetailModel != null)
            {
                obj.UserId = UserDetailModel.Id;
            }

            TeamDetailModel = await _apiClient.GetTeamByIdAsync(obj.TeamId);
            UserDetailModel = await _apiClient.UserGetByIdAsync(obj.UserId);
            Users = new ObservableCollection<UserListModelInner>(await _apiClient.UsersInTeamAsync(TeamDetailModel.Id));
        }
    }
}
