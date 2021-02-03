using Microsoft.EntityFrameworkCore.Migrations;

namespace Fitter.DAL.Migrations
{
    public partial class TextLengthChange : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<string>(
                name: "Text",
                table: "Posts",
                nullable: false,
                oldClrType: typeof(string),
                oldMaxLength: 180);

            migrationBuilder.AlterColumn<string>(
                name: "Text",
                table: "Comments",
                nullable: false,
                oldClrType: typeof(string),
                oldMaxLength: 180);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<string>(
                name: "Text",
                table: "Posts",
                maxLength: 180,
                nullable: false,
                oldClrType: typeof(string));

            migrationBuilder.AlterColumn<string>(
                name: "Text",
                table: "Comments",
                maxLength: 180,
                nullable: false,
                oldClrType: typeof(string));
        }
    }
}
