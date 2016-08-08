#ifndef DRAW_H
#define DRAW_H
#include "program.h"
//Function declarations for my graphics library
void flipBuffers();
void fillScreen(char r, char g, char b, char a);
void drawString(int x, int y, char * string);
void drawRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a);
void drawPixel(int x, int y, char r, char g, char b);

void drawPixels(struct Pixel pixels[200]);
void drawBitmap(int ox, int oy, int width, int height, unsigned char input[][width], unsigned const char palette[][3]);
void flipBuffers();
void fillRect(int ox, int oy, int width, int height, int r, int g, int b);
#endif /* DRAW_H */