using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using Fitter.BL.Factories;
using Fitter.BL.Mapper;
using Fitter.BL.Mapper.Interface;
using Fitter.BL.Model;
using Fitter.BL.Repositories;
using Fitter.BL.Repositories.Interfaces;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Swashbuckle.AspNetCore.Annotations;

namespace Fitter.Swagger.API.Controllers
{
    [Route("api/users")]
    [ApiController]
    public class UsersController : ControllerBase
    {
        private readonly IDbContextFactory _dbContextFactory;
        private readonly IMapper _mapper;
        private readonly IUsersRepository _usersRepository;

        public UsersController()
        {
            _dbContextFactory = new DbContextFactory();
            _mapper = new Mapper();
            _usersRepository = new UsersRepository(_dbContextFactory, _mapper);
        }

        [HttpPost]
        [Route("create")]
        [SwaggerOperation(OperationId = "UserCreate")]
        public ActionResult<UserDetailModel> Create(UserDetailModel user)
        {
            return _usersRepository.Create(user);
        }

        [HttpGet]
        [Route("getById")]
        [SwaggerOperation(OperationId = "UserGetById")]
        public ActionResult<UserDetailModel> GetById(Guid id)
        {
            return _usersRepository.GetById(id);
        }

        [HttpGet]
        [Route("getByEmail")]
        [SwaggerOperation(OperationId = "UserGetByEmail")]
        public ActionResult<UserDetailModel> GetByEmail(string email)
        {
            return _usersRepository.GetByEmail(email);
        }

        [HttpGet]
        [Route("getUsersInTeam")]
        [SwaggerOperation(OperationId = "UsersInTeam")]
        public ActionResult<IList<UserListModel>> GetUsersInTeam(Guid id)
        {
            return _usersRepository.GetUsersInTeam(id).ToList();
        }

        [HttpGet]
        [Route("getUsersNotInTeam")]
        [SwaggerOperation(OperationId = "UsersNotInTeam")]
        public ActionResult<IList<UserListModel>> GetUsersNotInTeam(Guid id)
        {
            return _usersRepository.GetUsersNotInTeam(id).ToList();
        }

        [HttpGet]
        [Route("getLastActivity")]
        [SwaggerOperation(OperationId = "UserGetLastActivity")]
        public ActionResult<string> GetLastActivity(Guid id)
        {
            var result = _usersRepository.GetLastActivity(id);
            return $"\"{result}\"";
        }
    }
}