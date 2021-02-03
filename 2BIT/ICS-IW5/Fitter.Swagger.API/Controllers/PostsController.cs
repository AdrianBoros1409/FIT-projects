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
    [Route("api/posts")]
    [ApiController]
    public class PostsController : ControllerBase
    {
        private readonly IPostsRepository _postsRepository;

        public PostsController()
        {
            IDbContextFactory dbContextFactory = new DbContextFactory();
            IMapper mapper = new Mapper();
            _postsRepository = new PostsRepository(dbContextFactory, mapper);
        }

        [HttpPost]
        [Route("create")]
        [SwaggerOperation(OperationId = "CreatePost")]
        public ActionResult Create(PostModel post)
        {
            _postsRepository.Create(post);
            return Ok();
        }

        [HttpDelete]
        [Route("delete")]
        [SwaggerOperation(OperationId = "DeletePost")]
        public ActionResult Delete(Guid id)
        {
            _postsRepository.Delete(id);
            return Ok();
        }

        [HttpGet]
        [Route("getPostsForTeam")]
        [SwaggerOperation(OperationId = "GetPostsForTeam")]
        public ActionResult<IList<PostModel>> GetPostsForTeam(Guid id)
        {
            return _postsRepository.GetPostsForTeam(id).ToList();
        }

        [HttpGet]
        [Route("getById")]
        [SwaggerOperation(OperationId = "GetPostById")]
        public ActionResult<PostModel> GetPostById(Guid id)
        {
            return _postsRepository.GetById(id);
        }

        [HttpGet]
        [Route("searchInPosts")]
        [SwaggerOperation(OperationId = "SearchInPosts")]
        public ActionResult<IList<Guid>> SearchInPosts(string substring, Guid id)
        {
            return _postsRepository.SearchInPosts(substring, id).ToList();
        }
    }
}