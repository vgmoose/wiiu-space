#ifndef DRAW_H
#define DRAW_H
#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/types.h"
#include "program.h"
//Function declarations for my graphics library
void flipBuffers();
void fillScreen(char r, char g, char b, char a);
void drawString(int x, int y, char * string);
void drawRect(struct Services *services, int x1, int y1, int x2, int y2, char r, char g, char b, char a);

#endif /* DRAW_H */