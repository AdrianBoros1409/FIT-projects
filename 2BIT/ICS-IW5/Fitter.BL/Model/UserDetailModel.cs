namespace Fitter.BL.Model
{
    public class UserDetailModel : BaseModel
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public string FullName => $"{FirstName} {LastName}";
        public string Email { get; set; }
        public string Password { get; set; }
    }
}