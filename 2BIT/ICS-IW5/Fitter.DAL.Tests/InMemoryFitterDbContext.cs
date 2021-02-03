using Microsoft.EntityFrameworkCore;

namespace Fitter.DAL.Tests
{
    public class InMemoryFitterDbContext : IFitterDbContext
    {
        public FitterDbContext CreateDbContext()
        {
            var optionBuilder = new DbContextOptionsBuilder<FitterDbContext>();
            optionBuilder.UseInMemoryDatabase("FitterDb");
            return new FitterDbContext(optionBuilder.Options);
        }
    }
}