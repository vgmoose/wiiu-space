
#include "draw.h"

#include <coreinit/cache.h>
#include <coreinit/screen.h>

void flipBuffers()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	
	//Flush the cache
	DCFlushRange((void *)screenBuffer + buf0_size, buf1_size);
	DCFlushRange((void *)screenBuffer, buf0_size);
	
	//Flip the buffer
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);
}

/**
This is the main function that does the grunt work of drawing to both screens. It takes in the 
Services structure that is constructed in program.c, which contains the pointer to the function
that is responsible for putting a pixel on the screen. By doing it this way, the OSScreenPutPixelEx function pointer is only
looked up once, at the program initialization, which makes successive calls to this pixel caller quicker.
**/
void putAPixel(int x, int y, int r, int g, int b)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | 0;
	x *= 2;
	y *= 2;

	int ax, ay, az;
	for (ax=0; ax<2; ax++)
		for (ay=0; ay<2; ay++)
			for (az=0; az<2; az++)
				if (ax) {  // uncomment for fullscreen on TV, text on the TV will have to be moved though
					OSScreenPutPixelEx(ax, x + ay, y + az, num);
				}    // uncomment for fullscreen on TV, text on the TV will have to be moved though
				else {    // uncomment for fullscreen on TV, text on the TV will have to be moved though
					 int a;  // uncomment for fullscreen on TV, text on the TV will have to be moved though
					 for (a = 0; a < 2; a++) {  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 int x1 = ( ( (x + ay) * 3 ) / 2 ) + a;  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 int y1 = ( ( (y + az) * 3 ) / 2 ) + a;  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 int x2 = ( ( (x + ay) * 3 ) / 2 ) + ( 1 - a );  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 int y2 = ( ( (y + az) * 3 ) / 2 ) + a;  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 OSScreenPutPixelEx( 0, x1, y1, num );  // uncomment for fullscreen on TV, text on the TV will have to be moved though
						 OSScreenPutPixelEx( 0, x2, y2, num );  // uncomment for fullscreen on TV, text on the TV will have to be moved though
					 }  // uncomment for fullscreen on TV, text on the TV will have to be moved though
					 OSScreenPutPixelEx( 1, x, y, num );  // uncomment for fullscreen on TV, text on the TV will have to be moved though
				 }  // uncomment for fullscreen on TV, text on the TV will have to be moved though
}

void drawString(int x, int y, char * string)
{
	//OSScreenPutFontEx(0, x, y, string);
	OSScreenPutFontEx(1, x, y, string);
}

void drawStringTv(int x, int y, char * string)
{
	OSScreenPutFontEx(0, x, y, string);
}

void fillScreen(char r,char g,char b,char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	
	OSScreenClearBufferEx(0, num);
	OSScreenClearBufferEx(1, num);
}