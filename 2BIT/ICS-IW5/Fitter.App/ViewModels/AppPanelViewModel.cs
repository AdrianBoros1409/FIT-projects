using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Fitter.App.API;
using Fitter.App.API.Models;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.BL.Extensions;
using Fitter.BL.Messages;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class AppPanelViewModel : ViewModelBase
    {
        private readonly APIClient _apiClient;
        private readonly IMediator _mediator;

        private UserDetailModelInner _model;
        private ObservableCollection<TeamListModelInner> _teams;

        public ObservableCollection<TeamListModelInner> Teams
        {
            get { return _teams; }
            set
            {
                if (Equals(value,_teams))return;
                {
                    _teams = value;
                    OnPropertyChanged();
                }
            }
        }
        public ICommand TeamSelectedCommand { get; set; }
        public ICommand UserInfoCommand { get; set; }
        public ICommand GoToHomeCommand { get; set; }
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
        public AppPanelViewModel(IMediator mediator, APIClient apiClient)
        {
            _apiClient = apiClient;
            _mediator = mediator;
            TeamSelectedCommand = new RelayCommand<TeamListModelInner>(TeamSelected);
            UserInfoCommand = new RelayCommand(UserInfo);
            GoToHomeCommand = new RelayCommand(GoToHome);
            mediator.Register<UserLoginMessage>(UserLog);
            mediator.Register<LogOutMessage>(UserLogOut);
            mediator.Register<UpdatedTeamsMessage>(TeamListUpdated);
        }

        private void UserInfo()
        {
            _mediator.Send(new UserInfoMessage{Id = Model.Id});
        }

        private void GoToHome()
        {
            _mediator.Send(new GoToHomeMessage());
        }

        private void UserLogOut(LogOutMessage obj)
        {
            Model = null;
        }

        private void TeamSelected(TeamListModelInner team)
        {
            _mediator.Send(new ResetTeamMessage());
            _mediator.Send(new TeamSelectedMessage{Id = team.Id});
        }

        private async void UserLog(UserLoginMessage obj)
        {
            Guid id = Guid.Parse(obj.Id.ToString());
            Model = await _apiClient.UserGetByIdAsync(id);
            OnLoad();
        }

        public void TeamListUpdated(UpdatedTeamsMessage msg)
        {
            OnLoad();
        }

        public async void OnLoad()
        {
            Teams = new ObservableCollection<TeamListModelInner>(await _apiClient.GetTeamsForUserAsync(Model.Id));
        }
    }
}
