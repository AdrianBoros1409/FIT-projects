using System;
using Microsoft.EntityFrameworkCore.Migrations;

namespace Fitter.DAL.Migrations
{
    public partial class seedUpToDate : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<Guid>(
                name: "Id",
                table: "UsersInTeams",
                nullable: false,
                defaultValue: new Guid("00000000-0000-0000-0000-000000000000"));
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Id",
                table: "UsersInTeams");
        }
    }
}
