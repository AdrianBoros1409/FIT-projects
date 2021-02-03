using System;
using Microsoft.EntityFrameworkCore.Migrations;

namespace Fitter.DAL.Migrations
{
    public partial class tag_attachment_removed : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_Users_Comments_CommentId",
                table: "Users");

            migrationBuilder.DropForeignKey(
                name: "FK_Users_Posts_PostId",
                table: "Users");

            migrationBuilder.DropTable(
                name: "Attachments");

            migrationBuilder.DropIndex(
                name: "IX_Users_CommentId",
                table: "Users");

            migrationBuilder.DropIndex(
                name: "IX_Users_PostId",
                table: "Users");

            migrationBuilder.DropColumn(
                name: "CommentId",
                table: "Users");

            migrationBuilder.DropColumn(
                name: "PostId",
                table: "Users");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<Guid>(
                name: "CommentId",
                table: "Users",
                nullable: true);

            migrationBuilder.AddColumn<Guid>(
                name: "PostId",
                table: "Users",
                nullable: true);

            migrationBuilder.CreateTable(
                name: "Attachments",
                columns: table => new
                {
                    Id = table.Column<Guid>(nullable: false),
                    CurrentPostId = table.Column<Guid>(nullable: false),
                    File = table.Column<byte[]>(nullable: true),
                    FileType = table.Column<int>(nullable: false),
                    Name = table.Column<string>(nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Attachments", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Attachments_Posts_CurrentPostId",
                        column: x => x.CurrentPostId,
                        principalTable: "Posts",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_Users_CommentId",
                table: "Users",
                column: "CommentId");

            migrationBuilder.CreateIndex(
                name: "IX_Users_PostId",
                table: "Users",
                column: "PostId");

            migrationBuilder.CreateIndex(
                name: "IX_Attachments_CurrentPostId",
                table: "Attachments",
                column: "CurrentPostId");

            migrationBuilder.AddForeignKey(
                name: "FK_Users_Comments_CommentId",
                table: "Users",
                column: "CommentId",
                principalTable: "Comments",
                principalColumn: "Id",
                onDelete: ReferentialAction.Restrict);

            migrationBuilder.AddForeignKey(
                name: "FK_Users_Posts_PostId",
                table: "Users",
                column: "PostId",
                principalTable: "Posts",
                principalColumn: "Id",
                onDelete: ReferentialAction.Restrict);
        }
    }
}
