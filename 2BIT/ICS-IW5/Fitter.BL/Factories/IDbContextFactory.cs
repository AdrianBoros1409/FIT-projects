using Fitter.DAL;

namespace Fitter.BL.Factories
{
    public interface IDbContextFactory
    {
        FitterDbContext CreateDbContext();
    }
}