using System.Windows;
using System.Windows.Controls;
using Fitter.App.ViewModels;

namespace Fitter.App.Views
{
    public abstract class UserControlBase : UserControl
    {
        protected UserControlBase()
        {
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (DataContext is IViewModel viewModel)
            {
                viewModel.Load();
            }
        }
    }
}
