using System;
using System.Linq;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Fitter.BL.Repositories.Interfaces;
using Xunit;

namespace Fitter.BL.Tests
{
    public class UsersRepositoryTests
    {
        [Fact]
        public void CreateUser()
        {
            var sut = CreateSUT();
            var model = new UserDetailModel
            {
                FirstName = "Jozef",
                LastName = "Kovac",
                Password = "asdf",
                Email = "kovacjozef@gmail.com"
            };
            var createdUser = sut.Create(model);
            Assert.NotNull(createdUser);
        }

        [Fact]
        public void GetUserById()
        {
            var sut = CreateSUT();
            var model = new UserDetailModel()
            {
                FirstName = "Martin",
                LastName = "Slovak",
                Email = "martinko123@azet.sk",
                Password = "assaf"
            };
            var createdUser = sut.Create(model);
            var foundedUser = sut.GetById(createdUser.Id);
            Assert.NotNull(foundedUser);
        }

        [Fact]
        public void CheckUsersPassword()
        {
            var sut = CreateSUT();
            var model = new UserDetailModel()
            {
                FirstName = "Ivana",
                LastName = "Sucha",
                Email = "ivanasucha@seznam.cz",
                Password = "123456"
            };
            var createdUser = sut.Create(model);
            var foundedUser = sut.GetByEmail(model.Email);
            Assert.Equal(foundedUser.Password, model.Password);
        }

        private IUsersRepository CreateSUT()
        {
            return new UsersRepository(new InMemoryDbContext(), new Mapper.Mapper());
        }
    }
}