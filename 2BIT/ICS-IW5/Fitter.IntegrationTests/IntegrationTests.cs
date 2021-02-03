using System.Net;
using System.Threading.Tasks;
using Xunit;

namespace Fitter.IntegrationTests
{
    public class IntegrationTests
    {
        [Fact]
        public async Task GetUserByIdAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/users/getById?id=7d9b38d0-4fb9-4b49-b052-08d6ce33d4e9");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task getLastActivityAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/users/getLastActivity?id=7d9b38d0-4fb9-4b49-b052-08d6ce33d4e9");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task GetUserByEmailAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/users/getByEmail?email=a");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task GetUsersInTeamAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/users/getUsersInTeam?id=7d9b38d0-4fb9-4b49-b052-08d6ce33d4e9");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task GetUsersNotInTeamAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/users/getUsersNotInTeam?id=7d9b38d0-4fb9-4b49-b052-08d6ce33d4e9");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }


        [Fact]
        public async Task TeamByIdAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/teams/getById?id=2ae63be4-4d0d-4382-b27d-08d6ce4f56be");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task GetTeamsForUserAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/teams/getTeamsForUser?id=41013b86-a8fa-4daa-85c8-f4ef39ff5f90");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task CheckIfTeamExistsAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/teams/exists?name=IW5Team");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task getPostsForTeamAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/posts/getPostsForTeam?id=ee65198c-a044-4dca-8b14-fc36e5047af8");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task SearchInPostAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/posts/searchInPosts?id=ee65198c-a044-4dca-8b14-fc36e5047af8");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task getCommentsForPostAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/comments/getCommentsForPost?id=f15083ac-082b-4f45-999f-2106663069fd");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }

        [Fact]
        public async Task SearchInCommentsAsync()
        {
            using (var client = new TestClientProvider().Client)
            {
                var response = await client.GetAsync("https://fitterswaggerapi.azurewebsites.net/index.html/api/comments/searchInComments?id=f15083ac-082b-4f45-999f-2106663069fd");

                response.EnsureSuccessStatusCode();

                Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            }
        }
    }
}