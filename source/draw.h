#pragma once

#define TOP_ASPECT_X 0x5
#define TOP_ASPECT_Y 0x3
#define TOP_HEIGHT 240
#define TOP_WIDTH 400
#define BOTTOM_HEIGHT 240
#define BOTTOM_WIDTH 320

#include <3ds/types.h>
#include <3ds/gfx.h>
#include <stdio.h>
#include "draw.h"
#include "space.h"
#include "program.h"
#include "font.h"

void flipBuffers();
void fillScreen(char r, char g, char b, char a, u8* screen);
void drawPixels(struct Pixel pixels[200], u8* screen);
void drawBitmap(int ox, int oy, int width, int height, void *inp, void *pal, u8* screen);
void fillRect(int ox, int oy, int width, int height, int r, int g, int b, u8* screen);

int drawCharacter(u8* fb, font_s* f, char c, s16 x, s16 y, u16 w, u16 h);
void clearScreen(u8* screen,gfxScreen_t screenPos);
void drawPixel(int x, int y, char r, char g, char b, u8* screen);
void drawChar(char letter,int x,int y, char r, char g, char b, u8* screen);
void drawString(int x,int y, char* word, u8* screen);
void drawLine( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen);
void drawRect( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen);
void drawFillRect( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen);
void drawCircle(int x, int y, int radius, char r, char g, char b, u8* screen);
void drawFillCircle(int x, int y, int radius, char r , char g, char b, u8* screen);
void drawCircleCircum(int xc, int yc, int x, int y, char r, char g, char b, u8* screen);

void gfxFadeScreen(gfxScreen_t screen, gfx3dSide_t side, u32 f);
void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y);
void gfxDrawDualSprite(u8* spriteData, u16 width, u16 height, s16 x, s16 y);
void gfxDrawSpriteAlpha(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y);
void gfxDrawSpriteAlphaBlend(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y);
void gfxDrawSpriteAlphaBlendFade(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y, u8 fadeValue);
