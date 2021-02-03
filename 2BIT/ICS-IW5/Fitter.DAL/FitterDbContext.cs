using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;

namespace Fitter.DAL
{
    public class FitterDbContext : DbContext
    {
        public FitterDbContext()
        {
        }

        public FitterDbContext(DbContextOptions options) : base(options)
        {
        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<Team>()
                .HasMany<Post>(p => p.Posts)
                .WithOne(t => t.Team)
                .HasForeignKey(k => k.CurrentTeamId)
                .OnDelete(DeleteBehavior.Cascade);
            modelBuilder.Entity<Post>()
                .HasMany<Comment>(c => c.Comments)
                .WithOne(p => p.Post)
                .HasForeignKey(k => k.CurrentPostId)
                .OnDelete(DeleteBehavior.Cascade);
            modelBuilder.Entity<User>()
                .HasMany<Post>(p => p.Posts)
                .WithOne(a => a.Author)
                .HasForeignKey(k => k.CurrentAuthorId)
                .OnDelete(DeleteBehavior.SetNull);
            modelBuilder.Entity<User>()
                .HasMany<Comment>(c => c.Comments)
                .WithOne(a => a.Author)
                .HasForeignKey(k => k.CurrentAuthorId)
                .OnDelete(DeleteBehavior.SetNull);
            modelBuilder.Entity<UsersInTeam>().HasKey(ut => new {ut.UserId, ut.TeamId});
        }

        public DbSet<Comment> Comments { get; set; }
        public DbSet<Post> Posts { get; set; }
        public DbSet<Team> Teams { get; set; }
        public DbSet<User> Users { get; set; }
        public DbSet<UsersInTeam> UsersInTeams { get; set; } 
    }
}
