using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
    public class UserInfoViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly APIClient _apiClient;
        private UserDetailModelInner _userModel;
        private string _lastActivity;
        public ICommand GoBackCommand { get; set; }
        public ICommand TeamSelectedCommand { get; set; }

        public string LastActivity
        {
            get => _lastActivity;
            set
            {
                if (Equals(value,_lastActivity)) return;
                {
                    _lastActivity = value;
                    OnPropertyChanged();
                }
            }
        }
        private ObservableCollection<TeamListModelInner> _teams;

        public ObservableCollection<TeamListModelInner> Teams
        {
            get => _teams;
            set
            {
                if (Equals(value, _teams)) return;
                {
                    _teams = value;
                    OnPropertyChanged();
                }
            }
        }
        public UserDetailModelInner UserModel
        {
            get => _userModel;
            set
            {
                if (Equals(value, UserModel))
                    return;

                _userModel = value;
                OnPropertyChanged();
            }
        }

        public UserInfoViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;
            
            TeamSelectedCommand = new RelayCommand<TeamListModelInner>(TeamSelected);
            GoBackCommand = new RelayCommand(GoBack);
            mediator.Register<UserInfoMessage>(ShowInfo);
            mediator.Register<GoToHomeMessage>(GoHome);
            mediator.Register<ResetTeamMessage>(ResetTeam);
        }

        private void ResetTeam(ResetTeamMessage obj)
        {
            UserModel = null;
        }

        private void TeamSelected(TeamListModelInner team)
        {
            _mediator.Send(new TeamSelectedMessage { Id = team.Id });
            UserModel = null;
        }

        private void GoHome(GoToHomeMessage obj)
        {
            UserModel = null;
        }

        private void GoBack()
        {
            UserModel = null;
        }

        private async void ShowInfo(UserInfoMessage obj)
        {
            UserModel = await _apiClient.UserGetByIdAsync(obj.Id);
            OnLoad();
        }

        public async void OnLoad()
        {
            Teams = new ObservableCollection<TeamListModelInner>(await _apiClient.GetTeamsForUserAsync(UserModel.Id));
            
            try
            {
                LastActivity = await _apiClient.UserGetLastActivityAsync(UserModel.Id);

            }
            catch
            {
                LastActivity = "-";
            }
        }
    }
}
