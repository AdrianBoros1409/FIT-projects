using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace Fitter.DAL
{
    public class DesignTimeDbContextFactory : IDesignTimeDbContextFactory<FitterDbContext>
    {
        public FitterDbContext CreateDbContext(string[] args)
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
