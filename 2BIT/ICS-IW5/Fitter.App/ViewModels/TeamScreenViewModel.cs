using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using Fitter.App.API;
using Fitter.App.API.Models;
using Fitter.App.Commands;
using Fitter.App.ViewModels.Base;
using Fitter.App.Views;
using Fitter.BL.Extensions;
using Fitter.BL.Messages;
using Fitter.BL.Model;
using Fitter.BL.Repositories.Interfaces;
using Fitter.BL.Services;

namespace Fitter.App.ViewModels
{
    public class TeamScreenViewModel : ViewModelBase
    {
        private readonly APIClient _apiClient;
        private readonly IMediator _mediator;
        private TeamDetailModelInner _teamDetailModel;
        private PostModelInner _postModel;
        private CommentModelInner _commentModel;
        private UserDetailModelInner _userDetailModel;
        private string _searchString;
        public ICommand AddUserToTeamCommand { get; set; }
        public ICommand DeletePostCommand { get; set; }
        public ICommand DeleteCommentCommand { get; set; }
        public ICommand CreatePostCommand { get; set; }
        public ICommand AddCommentCommand { get; set; }
        public ICommand TeamInfoCommand { get; set; }
        public ICommand SelectedPostCommand { get; set; }
        public ICommand SearchCommand { get; set; }
        public ICommand ShowUserCommand { get; set; }

        public TeamDetailModelInner TeamDetailModel
        {
            get => _teamDetailModel;
            set
            {
                if (Equals(value, TeamDetailModel))
                    return;

                _teamDetailModel = value;
                OnPropertyChanged();
            }
        }

        public string SearchString
        {
            get => _searchString;
            set
            {
                if (Equals(value, SearchString))
                {
                    return;
                }
                _searchString = value;
                OnPropertyChanged();
            }
        }
        public PostModelInner PostModel {
            get => _postModel;
            set {
                if (Equals(value, PostModel))
                {
                    return;
                }
                _postModel = value;
                OnPropertyChanged();
            }
        }

        public CommentModelInner CommentModel
        {
            get => _commentModel;
            set {
                if (Equals(value, CommentModel))
                {
                    return;
                }
                _commentModel = value;
                OnPropertyChanged();
            }
        }

        public UserDetailModelInner UserDetailModel
        {
            get => _userDetailModel;
            set
            {
                if (Equals(value,UserDetailModel))
                    return;
                _userDetailModel = value;
                OnPropertyChanged();
            }
        }
        private ObservableCollection<PostModelInner> _posts;

        public ObservableCollection<PostModelInner> Posts
        {
            get => _posts;
            set
            {
                if (Equals(value, Posts))
                    return;

                _posts = value;
                OnPropertyChanged();
            }
        }

        private ObservableCollection<CommentModelInner> _comments;

        public ObservableCollection<CommentModelInner> Comments
        {
            get => _comments;
            set
            {
                if (Equals(value,Comments))
                {
                    return;
                }

                _comments = value;
                OnPropertyChanged();
            }
        }

        private List<PostModelInner> _searchResults;

        public List<PostModelInner> SearchResults {
            get => _searchResults;
            set {
                if (Equals(value, SearchResults))
                {
                    return;
                }

                _searchResults = value;
                OnPropertyChanged();
            }
        }

        public TeamScreenViewModel(IMediator mediator, APIClient apiClient)
        {
            _mediator = mediator;
            _apiClient = apiClient;

            CreatePostCommand = new RelayCommand(CreatePost, CanCreatePost);
            DeletePostCommand = new RelayCommand<Guid>(CanDeletePost);
            DeleteCommentCommand = new RelayCommand<Guid>(CanDeleteComment);
            AddCommentCommand = new RelayCommand<Guid>(AddNewComment, CanAddComment);
            AddUserToTeamCommand = new RelayCommand(AddUserToTeam);
            TeamInfoCommand = new RelayCommand(ShowInfo);
            SelectedPostCommand = new RelayCommand<PostModel>(SelectedPost);
            SearchCommand = new RelayCommand(Search);
            ShowUserCommand = new RelayCommand<Guid>(ShowUser);

            mediator.Register<TeamSelectedMessage>(SelectedTeam);
            mediator.Register<GoToHomeMessage>(GoToHome);
            mediator.Register<UserLoginMessage>(CreateAdmin);
        }

        private void ShowUser(Guid id)
        {
            _mediator.Send(new UserInfoMessage { Id = id });
        }

        private void CanDeleteComment(Guid id)
        {
            var commentAuthor = Posts.SelectMany(e => e.Comments).First(e => e.Id == id).Author;
            if (_userDetailModel.Id == commentAuthor.Id)
            {
                DeleteComment(id);
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.CanNotDeleteComment_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void DeleteComment(Guid id)
        {
            var result = MessageBox.Show(Resources.Texts.TextResources.DeleteComment_Message, Resources.Texts.TextResources.DeleteComment_MTitle, MessageBoxButton.YesNo);
            if (result == MessageBoxResult.Yes)
            {
                await _apiClient.DeleteCommentAsync(id);
                OnLoad();
            }
        }

        private void CanDeletePost(Guid id)
        {
            var postAuthor = Posts.First(k => k.Id == id).Author;
            if (_userDetailModel.Id == postAuthor.Id)
            {
                DeletePost(id);
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.CanNotDeletePost_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void DeletePost(Guid id)
        {
            var result = MessageBox.Show(Resources.Texts.TextResources.DeletePost_Message, Resources.Texts.TextResources.DeletePost_MTitle, MessageBoxButton.YesNo);
            if (result == MessageBoxResult.Yes)
            {
                await _apiClient.DeletePostAsync(id);
                OnLoad();
            }
        }

        private async void Search()
        {
            if (string.IsNullOrWhiteSpace(SearchString))
            {
                OnLoad();
                return;
            }

            var searchIds = new List<Guid?>(await _apiClient.SearchInPostsAsync(SearchString, TeamDetailModel.Id));
            searchIds.AddRange( await _apiClient.SearchInCommentsAsync(SearchString, TeamDetailModel.Id));
            searchIds = searchIds.Distinct().ToList();
            GetSearchResults(searchIds);
        }

        private async void GetSearchResults(List<Guid?> searchIds)
        {
            SearchResults = new List<PostModelInner>();
            foreach (var id in searchIds)
            {
                SearchResults.Add(await _apiClient.GetPostByIdAsync(id));
            }

            SearchResults = SearchResults.OrderByDescending(c => c.Created).ToList();
            LoadSearchResults();
        }

        private async void LoadSearchResults()
        {
            Posts = new ObservableCollection<PostModelInner>(SearchResults);

            foreach (var post in Posts)
            {
                post.Comments = new ObservableCollection<CommentModelInner>(await _apiClient.GetCommentsForPostAsync(post.Id));
            }
        }

        private void SelectedPost(PostModel post)
        {
            _mediator.Send(new PostSelectedMessage{Id = post.Id});
        }

        private void ShowInfo()
        {
            _mediator.Send(new TeamInfoMessage
            {
                TeamId = TeamDetailModel.Id,
                UserId = UserDetailModel.Id
            });
        }

        private void AddUserToTeam()
        {
            _mediator.Send(new AddUserToTeamMessage
            {
                Id = TeamDetailModel.Id,
                UserId = UserDetailModel.Id
            });
        }

        private async void CreateAdmin(UserLoginMessage obj)
        {
            UserDetailModel = await _apiClient.UserGetByIdAsync(obj.Id);
        }

        private bool NotEmpty(string text)
        {
            if (text == null)
            {
                return false;
            }
            return !text.Contains("\\ltrch }") && text.Contains("\\ltrch");
        }
        private bool CanCreatePost()
        {
            return PostModel != null && NotEmpty(PostModel.Text) && NotEmpty(PostModel.Title);

        }

        private bool CanAddComment(Guid id)
        {
            var comment = Posts.First(k => k.Id == id).NewComment;
            return comment != null && !string.IsNullOrWhiteSpace(comment.Text);
        }

        private async void CreatePost()
        {
            PostModel.Id = Guid.NewGuid();
            PostModel.Team = TeamDetailModel;
            PostModel.Author = UserDetailModel;
            PostModel.Created = DateTime.Now.ToString("dd/MM/yyyy HH:mm:ss");
            
            await _apiClient.CreatePostAsync(PostModel);

            OnLoad();
        }

        private async void AddNewComment(Guid id)
        {
            string text = Posts.First(k => k.Id == id).NewComment.Text;
            if (NotEmpty(text))
            {
                var comment = new CommentModelInner
                {
                    Id = Guid.NewGuid(),
                    Author = UserDetailModel,
                    Post = await _apiClient.GetPostByIdAsync(id),
                    Text = text,
                    Created = DateTime.Now.ToString("dd/MM/yyyy HH:mm:ss")
                };
                await _apiClient.CreateCommentAsync(comment);
                OnLoad();
            }
            else
            {
                MessageBox.Show(Resources.Texts.TextResources.MissingText_Message, "ERROR",
                    MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void GoToHome(GoToHomeMessage obj)
        {
            TeamDetailModel = null;
        }

        private async void SelectedTeam(TeamSelectedMessage obj)
        {
            TeamDetailModel = await _apiClient.GetTeamByIdAsync(obj.Id);
            OnLoad();
        }

        private async void OnLoad()
        {
            Posts = new ObservableCollection<PostModelInner>(await _apiClient.GetPostsForTeamAsync(TeamDetailModel.Id));
            PostModel = new PostModelInner();
            CommentModel = new CommentModelInner();

            foreach (var post in Posts)
            {
                post.Comments = new ObservableCollection<CommentModelInner>(await _apiClient.GetCommentsForPostAsync(post.Id));
            }
        }
    }
}
