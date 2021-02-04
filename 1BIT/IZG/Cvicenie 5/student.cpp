///////////////////////////////////////////////////////////////////////////////
// File containing student functions
///////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "base.h"
#include "transform.h"
#include "model.h"

#include <cstdio>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// name spaces

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// function for drawing projected line, etc.
///////////////////////////////////////////////////////////////////////////////

// draw line defined in 3D
// applies transform matrix "model" and consequently "projection"
// a - first vertex in 3D
// b - second vertex
// color - drawing color
void ProjectLine(const S_Coords& a,
	const S_Coords& b,
	const S_RGBA &color)
{
	// line is projected by projection of its end points
	// first pally trsnforms of object and scene
	S_Coords aa, bb;
	trTransformVertex(aa, a);
	trTransformVertex(bb, b);

	// subsequently project to screen
	int u1, v1, u2, v2;
	trProjectVertex(u1, v1, aa);
	trProjectVertex(u2, v2, bb);

	DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
}

// draw triangle defined in 3D
// a,b,c - vertices
// n - normal
// material - material diffuse parameters of lightning model
void ProjectTriangle(const S_Coords& a,
	const S_Coords& b,
	const S_Coords& c,
	const S_Coords& n,
	const S_Material &material)
{
	// transform vertices with model matrix (object/scene manipulation)
	S_Coords aa, bb, cc;
	trTransformVertex(aa, a);
	trTransformVertex(bb, b);
	trTransformVertex(cc, c);

	// project triangle vertices to display
	int u1, v1, u2, v2, u3, v3;
	trProjectVertex(u1, v1, aa);
	trProjectVertex(u2, v2, bb);
	trProjectVertex(u3, v3, cc);

	// transform normal
	S_Coords nn;
	trTransformVector(nn, n);
	nn.normalize();

	/** Je potreba vzit transformovanou normalu (nn) a vektor mezi kamerou a trojuhelnikem (vv) a spocitat cosinus ze skalarniho soucinu techto vektoru (geometricky vyznam ss).
	*
	* Vektor vv:
	*   spocitame jako pozice kamery (koncovy bod) [0,0,-CAMERA_DIST] minus transformovany vrchol vrojuhelnika (pocatecni bod) - teoreticky lze vzit libovolny bod trojuhelnika
	*
	* Rovnice skalarniho soucinu pro 2D prostor:
	*          u  *  v           u1 * v1    +    u2 * v2
	*   cos = --------- = ------------------------------------- = alfa = velikost uhlu mezi dvema vektory
	*         |u| * |v|   sqrt(u1^2 + u2^2) * sqrt(v1^2 + v2^2)
	*
	* -> staci nam jen vrchni cast (zkoumame, jestli je kladny nebo zaporny (porovnani < 0)) a spodni cast je vzdy kladna -> zbytecne slozite :)
	* pozn. reseni si tedy upravte, zjednoduste, ale hlavne sami POCHOPTE!
	* Johny
	*/

	S_Coords vv(-cc.x, -cc.y, -CAMERA_DIST - cc.z);

	double vrchnicast = (vv.x * nn.x) + (vv.y * nn.y) + (vv.z * nn.z);
	// double spodnicast = sqrt(vv.x*vv.x + nn.x*nn.x) * sqrt(vv.y*vv.y + nn.y*nn.y) * sqrt(vv.z*vv.z + nn.z*nn.z);
	// double cos = vrchnicast / spodnicast;

	double cos = vrchnicast;

	if (cos < 0) // nevykresluje se! (uhel je tupy)
		return;

	// vykresluje se - ostry uhel, trojuhelnik je otocen k pozorovateli

	// recompute color
	S_RGBA color;
	color.red = ROUND2BYTE(255 * material.red);
	color.green = ROUND2BYTE(255 * material.green);
	color.blue = ROUND2BYTE(255 * material.blue);

	// draw as wire model
	DrawLine(u1, v1, u2, v2, aa.z, bb.z, color);
	DrawLine(u2, v2, u3, v3, bb.z, cc.z, color);
	DrawLine(u3, v3, u1, v1, cc.z, aa.z, color);

}

///////////////////////////////////////////////////////////////////////////////
// ProjectObject function
// - draw object transformed with active transform matrix

void ProjectObject(const S_Material &material)
{
	T_Triangles::iterator end = triangles.end();
	for (T_Triangles::iterator it = triangles.begin(); it != end; ++it)
	{
		ProjectTriangle(vertices[it->v[0]],
			vertices[it->v[1]],
			vertices[it->v[2]],
			normals[it->n],
			material);
	}
}

///////////////////////////////////////////////////////////////////////////////
// function DrawScene() invoked from main.cpp
// - draw whole scene including placed objects

void DrawScene()
{
	// clear frame buffer
	ClearBuffers();

	// set projection matrix
	trProjectionPerspective(CAMERA_DIST, frame_w, frame_h);

	// initialize model matrix
	trLoadIdentity();

	// translate whole scene
	trTranslate(0.0, 0.0, scene_move_z);

	// rotate whole scene - in two axis only - mouse is 2D device only... :(
	trRotateX(scene_rot_x * 0.01);
	trRotateY(scene_rot_y * 0.01);

	// draw "mirror" with blue color
	ProjectLine(S_Coords(0, -2, -2), S_Coords(0, 2, -2), COLOR_BLUE);
	ProjectLine(S_Coords(0, 2, -2), S_Coords(0, 2, 2), COLOR_BLUE);
	ProjectLine(S_Coords(0, 2, 2), S_Coords(0, -2, 2), COLOR_BLUE);
	ProjectLine(S_Coords(0, -2, 2), S_Coords(0, -2, -2), COLOR_BLUE);

	/** Translaci a rotaci prvniho objektu bychom znehodnotili matici frameworku a pote by se vykreslovalo mimo stred,
	* matici frameworku si ulozime pred vykreslenim cerveneho objektu a pote ji nastavime zpet.
	* Nastavime Scale tak, abychom otocili souradnici x mohli vykreslit druhy objekt uz s jinymi souradnicemi (iluze zdrcadla)
	* */

	// ulozeni matice z frameworku -> neznehodnoti se pri vykreslovani cerveneho objektu
	Matrix framework = trGetMatrix();

	// draw object with red color
	// add translation and rotation of object
	trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
	trRotateX(obj_rot_x * 0.01);
	trRotateY(obj_rot_y * 0.01);

	// now project and draw the object
	ProjectObject(MAT_RED);

	// vraceni neznicene matice frameworku, jaka byla na zacatku vykresleni sceny
	trSetMatrix(framework);

	// otoceni osy x druheho objektu (iluze zrdcadla)
	trScale(-1, 1, 1);

	// translace a rotace druheho objektu
	trTranslate(obj_move_x * 0.015, obj_move_y * 0.015, 0.0);
	trRotateX(obj_rot_x * 0.01);
	trRotateY(obj_rot_y * 0.01);

	// vykresleni druheho objektu (ruzovy)
	ProjectObject(MAT_RED2);
}
