#ifndef SPACE_H
#define SPACE_H
#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
// This math has added trig function approximations
#include "trigmath.h"

struct Bullet {
	int x;
	int y;
	float slope;
	int active;
};

//Struct for global variables for pong
struct SpaceGlobals{
	//Flag for restarting the entire game.
	int restart;
	
	unsigned char* arraything;
	unsigned char (*shipBitmap)[36];
	
	//Gameplay boundry
	int xMinBoundry;
	int xMaxBoundry;
	int yMinBoundry;
	int yMaxBoundry;
	int scale;
	int winX;
	int winY;
	int speed;
	unsigned int frame;
	
	unsigned char rotated_ship[36][36];

	//Globals for player1 location and movement dx/dy
	float p1X;
	float p1Y;
	int p1X_size;
	int p1Y_size;
	float angle;

	int p1X_default;
	int p1Y_default;

	//Game engine globals
	int direction;
	uint32_t button;
	Vec2D lstick;
	Vec2D rstick;

	int flag;
	int count;

	// only 20 bullets can be onscreen at a time
	struct Bullet bullets[20];

	int renderP1Flag;
	int renderResetFlag;

};

//Function declarations for pong functions.
void renderShip(struct SpaceGlobals *mySpaceGlobals);
void renderInitialPlayers(struct SpaceGlobals *mySpaceGlobals);
void renderReset(struct SpaceGlobals *mySpaceGlobals);
void reset(struct SpaceGlobals *mySpaceGlobals);
void resetRenderFlags(struct SpaceGlobals *mySpaceGlobals);
void render(struct SpaceGlobals *mySpaceGlobals);
void p1Move(struct SpaceGlobals *mySpaceGlobals);
void renderTexts(struct SpaceGlobals *mySpaceGlobals);
float atan2(float x, float y);
float sin(float x);
float cos(float x);

#endif /* SPACE_H */