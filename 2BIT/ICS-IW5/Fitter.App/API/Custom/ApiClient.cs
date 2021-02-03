using System;

namespace Fitter.App.API
{
    public partial class APIClient
    {
        public APIClient(string uri)
            : this(new Uri(uri))
        {
        }
    }
}