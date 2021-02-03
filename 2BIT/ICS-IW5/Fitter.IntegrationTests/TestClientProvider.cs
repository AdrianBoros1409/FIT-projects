using System;
using System.Net.Http;
using Fitter.Swagger.API;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.TestHost;

namespace Fitter.IntegrationTests
{
    public class TestClientProvider : IDisposable
    {
        private TestServer TestServer;
        public HttpClient Client { get; private set; }
        public TestClientProvider()
        {
            TestServer = new TestServer(new WebHostBuilder().UseUrls("https://fitterswaggerapi.azurewebsites.net/index.html").UseStartup<Startup>());

            Client = TestServer.CreateClient();
        }

        public void Dispose()
        {
            TestServer?.Dispose();
            Client?.Dispose();
        }
    }
}