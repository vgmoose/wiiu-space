#ifndef SPACE_H
#define SPACE_H
#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
// This math has added trig function approximations
#include "trigmath.h"
#include "program.h"
#include "images.h"

struct Bullet {
	int x;
	int y;
	float m_x;
	float m_y;
	int active;
};

struct Pixel {
	int x;
	int y;
	int r;
	int g;
	int b;
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
	Vec2D lstick;
	Vec2D rstick;
	
	// only 20 bullets can be onscreen at a time
	struct Bullet bullets[20];
	struct Pixel stars[200];

	int renderP1Flag;
	int renderResetFlag;
	int invalid;
	
	struct Services* services;
	
	int state; // 1 is title screen, 2 is gameplay, 3 is password, 4 is about
	int lives;
	int score;
	int level;
	int menuChoice;

};

//Function declarations for space functions.
void renderShip(struct SpaceGlobals *mySpaceGlobals);
void renderInitialPlayers(struct SpaceGlobals *mySpaceGlobals);
void renderReset(struct SpaceGlobals *mySpaceGlobals);
void reset(struct SpaceGlobals *mySpaceGlobals);
void resetRenderFlags(struct SpaceGlobals *mySpaceGlobals);
void render(struct SpaceGlobals *mySpaceGlobals);
void p1Move(struct SpaceGlobals *mySpaceGlobals);
void renderTexts(struct SpaceGlobals *mySpaceGlobals);
void initStars(struct SpaceGlobals *mySpaceGlobals);
void handleCollisions(struct SpaceGlobals * mySpaceGlobals);
void displayTitle(struct SpaceGlobals * mySpaceGlobals);
void doMenuAction(struct SpaceGlobals *mySpaceGlobals);
void drawMenuCursor(struct SpaceGlobals *mySpaceGlobals);
void renderBullets(struct SpaceGlobals *mySpaceGlobals);
void moveBullets(struct SpaceGlobals *mySpaceGlobals);
void renderStars(struct SpaceGlobals *mySpaceGlobals);
void decompress_sprite(int arraysize, int width, int height, const unsigned char* input, unsigned char target[][width]);

#endif /* SPACE_H */