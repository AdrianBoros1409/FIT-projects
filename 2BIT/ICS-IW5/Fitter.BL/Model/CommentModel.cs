using System;

namespace Fitter.BL.Model
{
    public class CommentModel : BaseModel
    {
        public UserDetailModel Author { get; set; }
        public string Created { get; set; }
        public string Text { get; set; }
        public PostModel Post { get; set; }
    }
}