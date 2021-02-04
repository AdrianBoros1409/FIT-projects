/******************************************************************************
* Laborator 01 - Zaklady pocitacove grafiky - IZG
* ihulik@fit.vutbr.cz
*
* $Id: $
*
* Popis: Hlavicky funkci pro funkce studentu
*
* Opravy a modifikace:
* - ibobak@fit.vutbr.cz, orderedDithering
*/

#include "student.h"
#include "globals.h"

#include <time.h>

const int M[] = {
	0, 204, 51, 255,
	68, 136, 187, 119,
	34, 238, 17, 221,
	170, 102, 153, 85
};

const int M_SIDE = 4;

/******************************************************************************
******************************************************************************
Funkce vraci pixel z pozice x, y. Je nutne hlidat frame_bufferu, pokud
je dana souradnice mimo hranice, funkce vraci barvu (0, 0, 0).
Ukol za 0.25 bodu */
S_RGBA getPixel(int x, int y)
{
	// todo
	if (x >= 0 && y >= 0 && x < width && y < height)
		return frame_buffer[width * y + x];

	return COLOR_BLACK; //vraci barvu (0, 0, 0)
}
/******************************************************************************
******************************************************************************
Funkce vlozi pixel na pozici x, y. Je nutne hlidat frame_bufferu, pokud
je dana souradnice mimo hranice, funkce neprovadi zadnou zmenu.
Ukol za 0.25 bodu */
void putPixel(int x, int y, S_RGBA color)
{
	// todo
	if (x >= 0 && y >= 0 && x < width && y < height)
		frame_buffer[width * y + x] = color;
}
/******************************************************************************
******************************************************************************
Funkce prevadi obrazek na odstiny sedi. Vyuziva funkce GetPixel a PutPixel.
Ukol za 0.5 bodu */
void grayScale()
{
	// todo
	S_RGBA color;
	int x, y;
	unsigned char intensity;

	for (x = 0; x <= width; x++)
		for (y = 0; y <= height; y++)
		{
			color = getPixel(x, y);
			intensity = ROUND(0.299 * color.red + 0.587 * color.green + 0.114 * color.blue);
			color.red = intensity;
			color.green = intensity;
			color.blue = intensity;
			putPixel(x, y, color);
		}

}

/******************************************************************************
******************************************************************************
Funkce prevadi obrazek na cernobily pomoci algoritmu maticoveho rozptyleni.
Ukol za 1 bod */

void orderedDithering()
{
	// todo
	int i, j;
	grayScale();
	S_RGBA color;

	for (int x = 0; x <= width; x++)
		for (int y = 0; y <= height; y++)
		{
			i = x % M_SIDE;
			j = y % M_SIDE;

			color = getPixel(x, y);

			if (color.red > M[M_SIDE * j + i])
				putPixel(x, y, COLOR_WHITE);
			else
				putPixel(x, y, COLOR_BLACK);
		}
}

/******************************************************************************
******************************************************************************
Funkce prevadi obrazek na cernobily pomoci algoritmu distribuce chyby.
Ukol za 1 bod */

void distributeErr(int x, int y, double error)
{
	S_RGBA color;
	int intensity;

	color = getPixel(x, y);
	intensity = ROUND(color.blue + error);
	if (intensity < 0)
		intensity = 0;
	if (intensity > 255)
		intensity = 255;
	color.red = intensity;
	color.green = intensity;
	color.blue = intensity;
	putPixel(x, y, color);
}


void errorDistribution()
{
	// todo
	S_RGBA color;
	int x, y, error;
	int intensity;

	grayScale();
	for (x = 0; x < width; x++)
		for (y = 0; y < height; y++)
		{
			color = getPixel(x, y);
			intensity = color.red;
			if (intensity <= 127)
				color = COLOR_BLACK;
			else
				color = COLOR_WHITE;
			error = intensity - color.red;
			putPixel(x, y, color);
			distributeErr(x + 1, y, error * 3 / 8.0);
			distributeErr(x, y + 1, error * 3 / 8.0);
			distributeErr(x + 1, y + 1, error / 4.0);
		}
}

/******************************************************************************
******************************************************************************
Funkce prevadi obrazek na cernobily pomoci metody prahovani.
Demonstracni funkce */
void thresholding(int Threshold)
{
	/* Prevedeme obrazek na grayscale */
	grayScale();

	/* Projdeme vsechny pixely obrazku */
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			/* Nacteme soucasnou barvu */
			S_RGBA color = getPixel(x, y);

			/* Porovname hodnotu cervene barevne slozky s prahem.
			Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
			obrazek je sedotonovy, takze R=G=B */
			if (color.red > Threshold)
				putPixel(x, y, COLOR_WHITE);
			else
				putPixel(x, y, COLOR_BLACK);
		}
}

/******************************************************************************
******************************************************************************
Funkce prevadi obrazek na cernobily pomoci nahodneho rozptyleni.
Vyuziva funkce GetPixel, PutPixel a GrayScale.
Demonstracni funkce. */
void randomDithering()
{
	/* Prevedeme obrazek na grayscale */
	grayScale();

	/* Inicializace generatoru pseudonahodnych cisel */
	srand((unsigned int)time(NULL));

	/* Projdeme vsechny pixely obrazku */
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			/* Nacteme soucasnou barvu */
			S_RGBA color = getPixel(x, y);

			/* Porovname hodnotu cervene barevne slozky s nahodnym prahem.
			Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
			obrazek je sedotonovy, takze R=G=B */
			if (color.red > rand() % 255)
			{
				putPixel(x, y, COLOR_WHITE);
			}
			else
				putPixel(x, y, COLOR_BLACK);
		}
}
/*****************************************************************************/
/*****************************************************************************/
