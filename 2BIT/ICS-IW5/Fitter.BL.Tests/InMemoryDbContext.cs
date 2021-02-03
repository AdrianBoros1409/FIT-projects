using System;
using System.Collections.Generic;
using System.Text;
using Fitter.BL.Factories;
using Fitter.DAL;
using Microsoft.EntityFrameworkCore;

namespace Fitter.BL.Tests
{
    public class InMemoryDbContext : IDbContextFactory
    {
        public FitterDbContext CreateDbContext()
        {
            var optionBuilder = new DbContextOptionsBuilder<FitterDbContext>();
            optionBuilder.UseInMemoryDatabase("FitterDb");
            return new FitterDbContext(optionBuilder.Options);
        }
    }
}
