using System;
using System.Collections.ObjectModel;

namespace Fitter.BL.Model
{
    public class PostModel : BaseModel
    {
        public UserDetailModel Author { get; set; }
        public string Created { get; set; }
        public string Title { get; set; }
        public string Text { get; set; }
        public TeamDetailModel Team { get; set; }
        public ObservableCollection<CommentModel> Comments { get; set; }
        public CommentModel NewComment { get; set; } = new CommentModel();
    }
}