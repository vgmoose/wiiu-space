#include "font.h"
#include "../draw.h"
#include <switch.h>

#define FB_WIDTH  1920
#define FB_HEIGHT 1080


Framebuffer fb;
u32 stride;

bool bufferOpen = false;

void screenInit()
{
	NWindow* win = nwindowGetDefault();

	// Create a linear double-buffered framebuffer
	framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	framebufferMakeLinear(&fb);

    //Clear both framebuffers.
	for (int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}

void screenDeinit()
{
    for(int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	
	framebufferClose(&fb);
}

void flipBuffers()
{
	if (bufferOpen) {
		framebufferEnd(&fb);
		bufferOpen = false;
	} else {
		// Retrieve the framebuffer
		u32* framebuf = (u32*) framebufferBegin(&fb, &stride);
		bufferOpen = true;
	}
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

	int ax, ay;
	for (ay=0; ay<2; ay++)
		for (ax=0; ax<2; ax++) {
			u32 pos = (y+ay) * stride / sizeof(u32) + (x+ax);
			// if (pos < FB_WIDTH * FB_HEIGHT)
			((u32*)fb.buf)[pos] = num;
		}
}

void drawString(int xi, int yi, char * string)
{
    // on Switch, we only use the TV screen
    drawStringTv(xi, yi, string);
}

void drawStringTv(int xi, int yi, char * string)
{
	// for every character in the string, if it's within range, render it at the current position
	// and move over 8 characters

	xi *= 6.25;
	yi *= 13;

	char next = -1;
	int i = 0;
	while (next != '\0')
	{
		next = string[i++];

		// actually draw this char pixel by pixel, if it's within range
		if (next >= 0 && next < 128)
		{
			char* bitmap = font[next];
			int x, y;
			for (x=0; x < 8; x++) {
				for (y=0; y < 8; y++) {
					if (bitmap[x] & 1 << y)
						putAPixel(xi+y+i*8, yi+x, 0xff, 0xff, 0xff);
				}
//				printf("\n");
			}
		}
	}
}

void fillScreen(char r,char g,char b,char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	
	// TODO: clear screen
	// OSScreenClearBufferEx(0, num);
	// OSScreenClearBufferEx(1, num);
}