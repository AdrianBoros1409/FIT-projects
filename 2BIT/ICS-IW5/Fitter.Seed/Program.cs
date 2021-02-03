using System;
using Fitter.DAL;
using Fitter.DAL.Entity;
using Microsoft.EntityFrameworkCore;
using Fitter.BL.Services;

namespace Fitter.Seed
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var dbContext = CreateDbContext())
            {
                ClearDatabase(dbContext);
                SeedData(dbContext);
            }
        }

        private static void SeedData(FitterDbContext dbContext)
        {
            var user = new User
            {
                FirstName = "Adrian",
                LastName = "Boros",
                Email = "ab@fit.cz",
                Password = "aaa",
            };
            var passwordHasher = new PasswordHasher(user.Password);
            user.Password = passwordHasher.GetHashedPassword();
            dbContext.Users.Add(user);
            dbContext.SaveChanges();

            var user2 = new User
            {
                FirstName = "Matej",
                LastName = "Zahorsky",
                Email = "mz@fit.cz",
                Password = "mmm",
            };
            var passwordHasher2 = new PasswordHasher(user2.Password);
            user2.Password = passwordHasher2.GetHashedPassword();
            dbContext.Users.Add(user2);
            dbContext.SaveChanges();
        }

        private static void ClearDatabase(FitterDbContext dbContext)
        {
            dbContext.RemoveRange(dbContext.Comments);
            dbContext.RemoveRange(dbContext.Posts);
            dbContext.RemoveRange(dbContext.Teams);
            dbContext.RemoveRange(dbContext.Users);
            dbContext.RemoveRange(dbContext.UsersInTeams);
            dbContext.SaveChanges();
        }

        private static FitterDbContext CreateDbContext()
        {
            var optionsBuilder = new DbContextOptionsBuilder<FitterDbContext>();
            optionsBuilder.UseSqlServer(
                @"Data Source=fitterdb.database.windows.net;
                Initial Catalog=FitterDB;
                User ID=xabrah04;Password=Fitterdb1;
                Connect Timeout=30;Encrypt=True;TrustServerCertificate=False;ApplicationIntent=ReadWrite;MultiSubnetFailover=False");
            return new FitterDbContext(optionsBuilder.Options);
        }
    }
}
