#include "space.h"

static const unsigned char output[36][36] = { { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 13, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 12, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 12, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 3, 11, 3, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 11, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 11, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 4, 3, 8, 11, 8, 3, 4, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 4, 3, 6, 12, 6, 3, 4, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 12, 4, 12, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 3, 6, 12, 3, 12, 6, 3, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 12, 12, 14, 14, 2, 5, 3, 12, 5, 7, 5, 12, 3, 5, 2, 14, 14, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 11, 10, 12, 14, 2, 5, 3, 12, 5, 7, 5, 12, 3, 5, 2, 14, 12, 10, 11, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 10, 9, 12, 11, 13, 13, 2, 13, 12, 4, 6, 4, 12, 13, 2, 13, 13, 11, 12, 9, 10, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 10, 12, 11, 13, 14, 14, 2, 13, 2, 3, 2, 13, 2, 14, 14, 13, 11, 12, 10, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 12, 13, 12, 14, 14, 12, 11, 2, 12, 2, 12, 2, 11, 12, 14, 14, 12, 13, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 9, 7, 9, 11, 14, 12, 13, 10, 5, 6, 7, 6, 5, 10, 13, 12, 14, 11, 9, 7, 9, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 9, 10, 11, 12, 13, 13, 10, 5, 8, 7, 8, 5, 10, 13, 13, 12, 11, 10, 9, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 10, 11, 2, 11, 10, 9, 9, 5, 3, 7, 3, 5, 9, 9, 10, 11, 2, 11, 10, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 11, 2, 3, 12, 11, 10, 10, 5, 6, 7, 6, 5, 10, 10, 11, 12, 5, 2, 11, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 2, 5, 3, 12, 11, 10, 9, 10, 5, 6, 5, 10, 9, 10, 11, 12, 3, 5, 2, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 2, 2, 5, 3, 6, 12, 11, 9, 7, 9, 5, 3, 5, 9, 7, 9, 11, 12, 6, 3, 5, 2, 2, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 2, 5, 3, 4, 4, 2, 12, 11, 10, 7, 9, 11, 5, 11, 9, 7, 10, 11, 12, 2, 4, 4, 3, 5, 2, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 2, 3, 4, 8, 8, 2, 2, 12, 11, 10, 11, 11, 14, 11, 11, 10, 11, 12, 2, 2, 8, 8, 4, 3, 2, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 2, 2, 2, 2, 2, 14, 14, 12, 12, 11, 11, 12, 14, 12, 11, 11, 12, 12, 14, 14, 2, 2, 2, 2, 2, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 4, 5, 2, 14, 14, 14, 2, 5, 4, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 2, 4, 2, 14, 14, 14, 2, 4, 2, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 } };


static const unsigned char ship_palette[15][4] = { {0xFF, 0xFF, 0xFF, 0x00 }, { 0x89, 0x01, 0x1D, 0x00 }, { 0xFF, 0x94, 0x6A, 0x00 }, { 0xCC, 0x3E, 0x00, 0x00 }, { 0xD9, 0x67, 0x00, 0x00 }, { 0xFD, 0xCA, 0x31, 0x00 }, { 0xFF, 0xFA, 0xEA, 0x00 }, { 0x1C, 0x5B, 0xF0, 0x00 }, { 0xBE, 0xBE, 0xBE, 0x00 }, { 0x94, 0x94, 0x94, 0x00 }, { 0x6B, 0x6B, 0x6B, 0x00 }, { 0x40, 0x40, 0x40, 0x00 }, { 0x1F, 0x1F, 0x1F, 0x00 }, { 0xFF, 0xFF, 0xFF, 0xFF } };

//Updates player1 location
void p1Move(struct SpaceGlobals *mySpaceGlobals) {
	
	mySpaceGlobals->renderP1Flag = 1;
	
	// Handle analog stick movements
	Vec2D left = mySpaceGlobals->lstick;
	Vec2D right = mySpaceGlobals->rstick;

	// get the differences
	float xdif = left.x + right.x;
	float ydif = left.y + right.y;
	
//	// Handle D-pad movements as well
	xdif += (mySpaceGlobals->button & BUTTON_RIGHT)? 1 : 0;
	xdif -= (mySpaceGlobals->button &  BUTTON_LEFT)? 1 : 0;
	ydif += (mySpaceGlobals->button &    BUTTON_UP)? 1 : 0;
	ydif -= (mySpaceGlobals->button &  BUTTON_DOWN)? 1 : 0;
	
	// max out speed at 1 or -1 in both directions
	xdif = (xdif >  1)?  1 : xdif;
	xdif = (xdif < -1)? -1 : xdif;
	ydif = (ydif >  1)?  1 : ydif;
	ydif = (ydif < -1)? -1 : ydif;
	
	// accept x and y movement from either stick
	mySpaceGlobals->p1X += xdif/** * mySpaceGlobals->speed **/;
	mySpaceGlobals->p1Y -= ydif/** * mySpaceGlobals->speed **/;
	
	// calculate angle to face
	mySpaceGlobals->angle = atan2(ydif, xdif);
//	mySpaceGlobals->angle += 0.01;
//	if (mySpaceGlobals->angle > 3.14159265*2)
//		mySpaceGlobals->angle = 0;
	mySpaceGlobals->angle -= 3.14159265/2;
	
//	if (mySpaceGlobals->angle > 3.14159265*2) 
//		mySpaceGlobals->angle -= 3.14159265*2;
//	
//	if (mySpaceGlobals->angle < 0) 
//		mySpaceGlobals->angle += 3.14159265*2;
	
//	mySpaceGlobals->angle = round2decimals(mySpaceGlobals->angle);

};

void makeRotationMatrix(struct SpaceGlobals *mySpaceGlobals, float angle, int width)
{
//	if (angle < 0 || angle > 2*3.14159265) return;
	int x;
	for (x=0; x<width; x++)
	{
		int y;
		for (y=0; y<width; y++)
		{
			mySpaceGlobals->rotated_ship[x][y] = 14;
		}
	}
		
	// use only the first 2 decimal places
//	angle = round2decimals(angle);
	
	// go though every pixel in the original bitmap
	for (x=0; x<width; x++)
	{
		int y;
		for (y=0; y<width; y++)
		{
			// rotate the pixel by the angle into a new spot in the rotation matrix
			int newx = (int)((x-18)*cos(angle) - (y-18)*sin(angle) + 18);
			int newy = (int)((x-18)*sin(angle) + (y-18)*cos(angle) + 18);
			
			if (newx < 0) newx += 36;
			if (newy < 0) newy += 36;
			
			if (output[x][y] == 14) continue;
			
			if (newx < 0 || newx >= width) continue;
			if (newy < 0 || newy >= width) continue;
			mySpaceGlobals->rotated_ship[newx][newy] = output[x][y];
		}
	}
}

void render(struct SpaceGlobals *mySpaceGlobals)
{
	mySpaceGlobals->frame++;
	
	int ii = 0;
	for (ii; ii < 2; ii++)
	{
		if (mySpaceGlobals->renderResetFlag)
		{
			renderReset(mySpaceGlobals);
		}

		if (mySpaceGlobals->renderP1Flag)
		{
			renderShip(mySpaceGlobals);
			renderTexts(mySpaceGlobals);
		}
			
		flipBuffers();
	}

	resetRenderFlags(mySpaceGlobals);

}

void renderTexts(struct SpaceGlobals *mySpaceGlobals)
{
	// every 60th frame draw the status bar
	if (mySpaceGlobals->frame % 60 == 0) {
		fillRect(0, 0, 400, 20);

		char credits[255];
		__os_snprintf(credits, 255, "%f (%f, %f)", atan2(mySpaceGlobals->lstick.x, mySpaceGlobals->lstick.y), mySpaceGlobals->lstick.x, mySpaceGlobals->lstick.y);
//				__os_snprintf(credits, 255, "%lf (%f, %f)", atan2(0.883202, -0.468992), 0.883202, -0.468992);
		drawString(0, -1, credits);
	}
}

void resetRenderFlags(struct SpaceGlobals *mySpaceGlobals)
{
	mySpaceGlobals->renderResetFlag = 0;
	mySpaceGlobals->renderP1Flag = 0;

}

void renderShip(struct SpaceGlobals *mySpaceGlobals) 
{
	const int posx = (int)mySpaceGlobals->p1X;
	const int posy = (int)mySpaceGlobals->p1Y;
	
	makeRotationMatrix(mySpaceGlobals, mySpaceGlobals->angle, 36);

	drawBitmap(posx, posy, 36, 36, mySpaceGlobals->rotated_ship, ship_palette);
}

//Reset the game
void reset(struct SpaceGlobals *mySpaceGlobals) {
	//Set global variables to default state
	mySpaceGlobals->p1X = mySpaceGlobals->p1X_default;
	mySpaceGlobals->p1Y = mySpaceGlobals->p1Y_default;
	
	mySpaceGlobals->direction = (mySpaceGlobals->count & 3);
	mySpaceGlobals->button = 0;

	//Set flag to render reset screen;
	mySpaceGlobals->renderResetFlag = 1;

};

void renderReset(struct SpaceGlobals *mySpaceGlobals)
{
	fillScreen(0,0,0,0);
	renderInitialPlayers(mySpaceGlobals);
}

//Draws the inital player paddles and ball.
void renderInitialPlayers(struct SpaceGlobals *mySpaceGlobals) {
				renderShip(mySpaceGlobals);

};
