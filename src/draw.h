#ifndef DRAW_H
#define DRAW_H
#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/types.h"
#include "program.h"
//Function declarations for my graphics library
void flipBuffers(struct Services *services);
void fillScreen(struct Services *services, char r, char g, char b, char a);
void drawString(struct Services *services, int x, int y, char * string);
void drawRect(struct Services *services, int x1, int y1, int x2, int y2, char r, char g, char b, char a);
void drawPixel(struct Services *services, int x, int y, char r, char g, char b);

void drawPixels(struct Services *services, struct Pixel pixels[200]);
void drawBitmap(struct Services *services, int ox, int oy, int width, int height, unsigned char input[][width], unsigned const char palette[][3]);
void flipBuffers(struct Services *services);
void fillRect(struct Services *services, int ox, int oy, int width, int height, int r, int g, int b);
#endif /* DRAW_H */