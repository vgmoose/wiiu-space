#ifndef PROGRAM_H
#define PROGRAM_H

#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
//Using modified version of draw to render at twice the scale to improve framerate

struct Bullet {
	int x;
	int y;
	float m_x;
	float m_y;
	int active;
};

struct Enemy {
	float angle;
	struct Bullet position;
	unsigned char rotated_sprite[23][23];
};

struct Pixel {
	int x;
	int y;
	int r;
	int g;
	int b;
};

struct Services
{
	unsigned int (*OSScreenPutPixelEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
	unsigned int coreinit_handle;
	void(*DCFlushRange)(void *buffer, uint32_t length);
	unsigned int(*OSScreenFlipBuffersEx)(unsigned int bufferNum);
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenClearBufferEx)(unsigned int bufferNum, unsigned int temp);
	unsigned int(*OSScreenPutFontEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, void * buffer);
};
//Struct for global variables for pong
struct SpaceGlobals{
	//Flag for restarting the entire game.
	int restart;
	
	//Gameplay boundry
	unsigned int frame;
	
	unsigned int seed;
	
	unsigned char rotated_ship[36][36];
	unsigned char orig_ship[36][36];
	unsigned char enemy[23][23];
	unsigned char title[100][200];

	//Globals for player1 location and movement dx/dy
	float p1X;
	float p1Y;
	float angle;
	
	int touched;
	int touchX;
	int touchY;
	int titleScreenRefresh;

	//Game engine globals
	uint32_t button;
	uint32_t hold_button;
	uint32_t release_button;
	Vec2D lstick;
	Vec2D rstick;
	
	// only 20 bullets can be onscreen at a time
	struct Bullet bullets[20];
	
	// the locations of the 200 random stars
	struct Pixel stars[200];
	
	// the location of enemies
	struct Enemy enemies[100];

	int renderP1Flag;
	int renderResetFlag;
	int invalid;
	
	struct Services* services;
	
	int state; // 1 is title screen, 2 is gameplay, 3 is password, 4 is about
	int lives;
	int score;
	int level;
	
	int menuChoice;
	int allowInput;

};

void _entryPoint();

#endif /* PROGRAM_H */