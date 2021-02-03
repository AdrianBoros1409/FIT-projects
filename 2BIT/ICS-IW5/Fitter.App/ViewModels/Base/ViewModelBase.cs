using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Fitter.App.ViewModels.Base
{
    public abstract class ViewModelBase : IViewModel, INotifyPropertyChanged
    {
        public virtual void Load()
        {
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

       
    }
}