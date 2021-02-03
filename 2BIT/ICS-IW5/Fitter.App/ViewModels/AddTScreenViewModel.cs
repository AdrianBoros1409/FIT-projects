using System;
using System.Windows;
using System.Windows.Input;
using Fitter.App.API;
using Fitter.App.API.Models;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.BL.Messages;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class AddTScreenViewModel : ViewModelBase
    {
        private readonly APIClient _apiClient;
        private readonly IMediator _mediator;
        private UserDetailModelInner _model;
        private TeamDetailModelInner _teamModel;

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
        public ICommand AddTeamCommand { get; set; }
        public ICommand GoBackCommand { get; set; }
        public UserDetailModelInner Model
        {
            get => _model;
            set
            {
                if (Equals(value, Model))
                    return;

                _model = value;
                OnPropertyChanged();
            }
        }
        public AddTScreenViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;
            GoBackCommand = new RelayCommand(GoBack);
            AddTeamCommand = new RelayCommand(AddTeam, CanAddTeam);
            mediator.Register<GoToHomeMessage>(GoToHome);
            mediator.Register<AddTMessage>(NewTeam);
        }

        private void GoToHome(GoToHomeMessage obj)
        {
            Model = null;
        }

        private void GoBack()
        {
            Model = null;
        }

        private async void AddTeam()
        {
            if (await _apiClient.TeamExistsAsync(TeamModel.Name) == true)
            {
                MessageBox.Show(Resources.Texts.TextResources.SameTeam_Message);
            }
            else
            {
                TeamModel = await _apiClient.CreateTeamAsync(TeamModel);
                TeamModel.Admin = Model;
                await _apiClient.AddUserToTeamAsync(TeamModel.Admin, TeamModel.Id);
                await _apiClient.UpdateTeamAsync(TeamModel);
                _mediator.Send(new UpdatedTeamsMessage());
                TeamModel = null;
                Model = null;
            }
        }

        private bool CanAddTeam()
        {
            return Model != null && TeamModel != null
                                 && !string.IsNullOrWhiteSpace(TeamModel.Name)
                                 && !string.IsNullOrWhiteSpace(TeamModel.Description);
        }

        private async void NewTeam(AddTMessage obj)
        {
            Guid id = Guid.Parse(obj.Id.ToString());
            Model = await _apiClient.UserGetByIdAsync(id);
            TeamModel = new TeamDetailModelInner();
        }


    }
}
