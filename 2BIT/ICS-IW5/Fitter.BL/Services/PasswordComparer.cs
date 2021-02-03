using System;
using System.Collections.Generic;
using System.Text;

namespace Fitter.BL.Services
{
    public  class PasswordComparer
    {
        public bool ComparePassword(string psswd, string hashedP)
        {
            var password = new PasswordHasher(psswd);
            psswd = password.GetHashedPassword();
            return psswd == hashedP;
        }
    }
}
