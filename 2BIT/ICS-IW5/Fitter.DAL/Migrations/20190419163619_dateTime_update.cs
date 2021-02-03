using System;
using Microsoft.EntityFrameworkCore.Migrations;

namespace Fitter.DAL.Migrations
{
    public partial class dateTime_update : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<string>(
                name: "Created",
                table: "Posts",
                nullable: true,
                oldClrType: typeof(DateTime));

            migrationBuilder.AlterColumn<string>(
                name: "Created",
                table: "Comments",
                nullable: true,
                oldClrType: typeof(DateTime));
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<DateTime>(
                name: "Created",
                table: "Posts",
                nullable: false,
                oldClrType: typeof(string),
                oldNullable: true);

            migrationBuilder.AlterColumn<DateTime>(
                name: "Created",
                table: "Comments",
                nullable: false,
                oldClrType: typeof(string),
                oldNullable: true);
        }
    }
}
