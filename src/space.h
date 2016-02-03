#ifndef SPACE_H
#define SPACE_H
#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
//Using modified version of draw to render at twice the scale to improve framerate
#include "../../../libwiiu/src/draw.h"
#include "../../../libwiiu/src/math.h"
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

	//Globals for player1 location and movement dx/dy
	int p1X;
	int p1Y;
	int p1X_size;
	int p1Y_size;

	int p1X_default;
	int p1Y_default;

	//Game engine globals
	int direction;
	uint32_t button;
	Vec2D lstick;
	Vec2D rstick;

	int flag;
	int count;


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
#endif /* SPACE_H */