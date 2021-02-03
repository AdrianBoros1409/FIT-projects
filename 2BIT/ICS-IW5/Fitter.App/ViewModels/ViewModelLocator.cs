using Fitter.App.API;
using Fitter.BL.Factories;
using Fitter.BL.Mapper;
using Fitter.BL.Repositories;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class ViewModelLocator
    {
        private readonly IMediator _mediator;

        private readonly APIClient _apiClient;

        public HomeScreenViewModel HomeScreenViewModel => new HomeScreenViewModel(_mediator, _apiClient);
        public AddUScreenViewModel AddUScreenViewModel => new AddUScreenViewModel(_mediator, _apiClient);
        public AddTScreenViewModel AddTScreenViewModel => new AddTScreenViewModel(_mediator, _apiClient);
        public LoginPanelViewModel LoginPanelViewModel => new LoginPanelViewModel(_mediator, _apiClient);
        public AppPanelViewModel AppPanelViewModel => new AppPanelViewModel(_mediator, _apiClient);
        public TeamScreenViewModel TeamScreenViewModel => new TeamScreenViewModel(_mediator, _apiClient);
        public AddUserToTeamViewModel AddUserToTeamViewModel => new AddUserToTeamViewModel(_mediator, _apiClient);
        public UserInfoViewModel UserInfoViewModel => new UserInfoViewModel(_mediator, _apiClient);
        public TeamInfoViewModel TeamInfoViewModel => new TeamInfoViewModel(_mediator, _apiClient);
        public RemoveUserFromTeamViewModel RemoveUserFromTeamViewModel => new RemoveUserFromTeamViewModel(_mediator, _apiClient);
        public MainWindowViewModel MainWindowViewModel => new MainWindowViewModel();
        public ViewModelLocator()
       {
           _apiClient = new APIClient("https://fitterswaggerapi.azurewebsites.net");
           _mediator = new Mediator();
       }
    }
}
