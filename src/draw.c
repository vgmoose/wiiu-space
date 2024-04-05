#include "draw.h"
#include "space.h"
#include "program.h"

// draw black rect all at once
void fillRect(int ox, int oy, int width, int height, int r, int g, int b)
{	
			
	int rx;
	for (rx=0; rx<width; rx++)
	{
		int ry;
		for (ry=0; ry<height; ry++)
		{
			int x = ox + rx;
			int y = oy + ry;
			
			// do actual pixel drawing logic
			putAPixel(x, y, r, g, b);
		}
	}
}

/**
This function draws a "bitmap" in a very particular fashion: it takes as input the matrix of chars to draw.
In this matrix, each char represents the index to look it up in the palette variable which is also passed. 
Alpha isn't used here, and instead allows the "magic color" of 0x272727 to be "skipped" when drawing.
By looking up the color in the palette, the bitmap can be smaller. Before compression was implemented, this was
more important. A potential speedup may be to integrate the three pixel colors into a matrix prior to this function.
**/
void drawBitmap(int ox, int oy, int width, int height, unsigned char input[][width], unsigned const char palette[][3])
{	
	int rx;
	for (rx=0; rx<width; rx++)
	{
		int ry;
		for (ry=0; ry<height; ry++)
		{
			const unsigned char* color = palette[input[ry][rx]];
			char r = color[2];
			char g = color[1];
			char b = color[0];
			
//			// transparent pixels
			if (r == 0x27 && g == 0x27 && b == 0x27)
			{
				continue;
			}
			
			int x = ox + rx;
			int y = oy + ry;
			
			// do actual pixel drawing logic
			putAPixel(x, y, r, g, b);
		}
	}
}

/**
This is primarly used for drawing the stars, and takes in a pixel map. It is similar to bitmap, but now
it takes the whole pixel map as well as which portion of it to actually draw. At the beginning, all of the stars
are drawn, but whenever the ship moves, only the stars underneath the ship need to be redrawn.
**/
void drawPixels(struct Pixel pixels[200])
{	
	int rx;
	for (rx=0; rx<200; rx++)
	{
		int x = pixels[rx].x;
		int y = pixels[rx].y;
		
		putAPixel(x, y, pixels[rx].r, pixels[rx].g, pixels[rx].b);
	}
}

void drawPixel(int x, int y, char r, char g, char b)
{		
	putAPixel(x, y, r, g, b);

}