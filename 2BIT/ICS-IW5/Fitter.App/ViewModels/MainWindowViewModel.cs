using System.Windows;
using System.Windows.Input;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.App.Views;
using Fitter.BL.Messages;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        public ICommand ExitApplication { get; set; }
        public ICommand MinimizeApplication { get; set; }
        public ICommand MaximizeApplication { get; set; }

        public MainWindowViewModel()
        {
            
            ExitApplication = new RelayCommand(Exit);
            MinimizeApplication = new RelayCommand(Minimize);
            MaximizeApplication = new RelayCommand(Maximize);
        }

        private void Exit()
        {
            System.Windows.Application.Current.Shutdown();
        }

        private void Minimize()
        {
            System.Windows.Application.Current.MainWindow.WindowState = WindowState.Minimized;
        }
        private void Maximize()
        {
            System.Windows.Application.Current.MainWindow.WindowState = System.Windows.Application.Current.MainWindow.WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
        }
    }
}