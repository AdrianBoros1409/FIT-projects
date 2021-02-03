using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Newtonsoft.Json;

namespace Fitter.App.API.Models
{
    public partial class PostModelInner : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private IList<CommentModelInner> _comments;

        [JsonProperty(PropertyName = "comments")]
        public IList<CommentModelInner> Comments {
            get => _comments;
            set {
                if (Equals(value, Comments))
                    return;

                _comments = value;
                OnPropertyChanged();
            }
        }
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}