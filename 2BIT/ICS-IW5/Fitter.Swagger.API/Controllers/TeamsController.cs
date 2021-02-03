using System;
using System.Collections.Generic;
using System.Linq;
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
    [Route("api/teams")]
    [ApiController]
    public class TeamsController : ControllerBase
    {
        private readonly IDbContextFactory _dbContextFactory;
        private readonly IMapper _mapper;
        private readonly ITeamsRepository _teamsRepository;

        public TeamsController()
        {
            _dbContextFactory = new DbContextFactory();
            _mapper = new Mapper();
            _teamsRepository = new TeamsRepository(_dbContextFactory, _mapper);
        }

        [HttpPost]
        [Route("create")]
        [SwaggerOperation(OperationId = "CreateTeam")]
        public ActionResult<TeamDetailModel> Create(TeamDetailModel team)
        {
            return _teamsRepository.Create(team);
        }

        [HttpGet]
        [Route("getById")]
        [SwaggerOperation(OperationId = "GetTeamById")]
        public ActionResult<TeamDetailModel> GetById(Guid id)
        {
            return _teamsRepository.GetById(id);
        }

        [HttpPost]
        [Route("addUserToTeam")]
        [SwaggerOperation(OperationId = "AddUserToTeam")]
        public ActionResult AddUserToTeam(UserDetailModel user, Guid id)
        {
            _teamsRepository.AddUserToTeam(user, id);
            return Ok();
        }

        [HttpDelete]
        [Route("removeUserFromTeam")]
        [SwaggerOperation(OperationId = "RemoveUserFromTeam")]
        public ActionResult RemoveUserFromTeam(UserDetailModel user, Guid id)
        {
            _teamsRepository.RemoveUserFromTeam(user, id);
            return Ok();
        }

        [HttpGet]
        [Route("getTeamsForUser")]
        [SwaggerOperation(OperationId = "GetTeamsForUser")]
        public ActionResult<IList<TeamListModel>> GetTeamsForUser(Guid id)
        {
            return _teamsRepository.GetTeamsForUser(id).ToList();
        }

        [HttpGet]
        [Route("exists")]
        [SwaggerOperation(OperationId = "TeamExists")]
        public ActionResult<bool> Exists(string name)
        {
            return _teamsRepository.Exists(name);
        }

        [HttpDelete]
        [Route("delete")]
        [SwaggerOperation(OperationId = "DeleteTeam")]
        public ActionResult Delete(Guid id)
        {
            _teamsRepository.Delete(id);
            return Ok();
        }

        [HttpDelete]
        [Route("update")]
        [SwaggerOperation(OperationId = "UpdateTeam")]
        public ActionResult Update(TeamDetailModel team)
        {
            _teamsRepository.Update(team);
            return Ok();
        }
    }
}