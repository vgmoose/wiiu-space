#include "space.h"

static const unsigned char output[36][36] = { { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 13, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 12, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 12, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 3, 11, 3, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 11, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 11, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 4, 3, 8, 11, 8, 3, 4, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 4, 3, 6, 12, 6, 3, 4, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 5, 6, 12, 4, 12, 6, 5, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 3, 6, 12, 3, 12, 6, 3, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 12, 12, 14, 14, 2, 5, 3, 12, 5, 7, 5, 12, 3, 5, 2, 14, 14, 12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 11, 10, 12, 14, 2, 5, 3, 12, 5, 7, 5, 12, 3, 5, 2, 14, 12, 10, 11, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 10, 9, 12, 11, 13, 13, 2, 13, 12, 4, 6, 4, 12, 13, 2, 13, 13, 11, 12, 9, 10, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 10, 12, 11, 13, 14, 14, 2, 13, 2, 3, 2, 13, 2, 14, 14, 13, 11, 12, 10, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 12, 13, 12, 14, 14, 12, 11, 2, 12, 2, 12, 2, 11, 12, 14, 14, 12, 13, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 9, 7, 9, 11, 14, 12, 13, 10, 5, 6, 7, 6, 5, 10, 13, 12, 14, 11, 9, 7, 9, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 9, 10, 11, 12, 13, 13, 10, 5, 8, 7, 8, 5, 10, 13, 13, 12, 11, 10, 9, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 12, 11, 10, 11, 2, 11, 10, 9, 9, 5, 3, 7, 3, 5, 9, 9, 10, 11, 2, 11, 10, 11, 12, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 11, 2, 3, 12, 11, 10, 10, 5, 6, 7, 6, 5, 10, 10, 11, 12, 5, 2, 11, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 12, 2, 5, 3, 12, 11, 10, 9, 10, 5, 6, 5, 10, 9, 10, 11, 12, 3, 5, 2, 12, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 2, 2, 5, 3, 6, 12, 11, 9, 7, 9, 5, 3, 5, 9, 7, 9, 11, 12, 6, 3, 5, 2, 2, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 2, 5, 3, 4, 4, 2, 12, 11, 10, 7, 9, 11, 5, 11, 9, 7, 10, 11, 12, 2, 4, 4, 3, 5, 2, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 2, 3, 4, 8, 8, 2, 2, 12, 11, 10, 11, 11, 14, 11, 11, 10, 11, 12, 2, 2, 8, 8, 4, 3, 2, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 2, 2, 2, 2, 14, 14, 12, 12, 11, 11, 12, 14, 12, 11, 11, 12, 12, 14, 14, 2, 2, 2, 2, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 4, 5, 2, 14, 14, 14, 2, 5, 4, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 2, 4, 2, 14, 14, 14, 2, 4, 2, 2, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 } };


static const unsigned char ship_palette[15][4] = { {0xFF, 0xFF, 0xFF, 0x00 }, { 0x89, 0x01, 0x1D, 0x00 }, { 0xFF, 0x94, 0x6A, 0x00 }, { 0xCC, 0x3E, 0x00, 0x00 }, { 0xD9, 0x67, 0x00, 0x00 }, { 0xFD, 0xCA, 0x31, 0x00 }, { 0xFF, 0xFA, 0xEA, 0x00 }, { 0x1C, 0x5B, 0xF0, 0x00 }, { 0xBE, 0xBE, 0xBE, 0x00 }, { 0x94, 0x94, 0x94, 0x00 }, { 0x6B, 0x6B, 0x6B, 0x00 }, { 0x40, 0x40, 0x40, 0x00 }, { 0x1F, 0x1F, 0x1F, 0x00 }, { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF } };

void activateBullet(struct SpaceGlobals * mySpaceGlobals, float slope, int x, int y)
{
	// find an inactive bullet
	int xx;
	for (xx=0; xx<20; xx++)
	{
		if (mySpaceGlobals->bullets[x].active == 0)
		{
			mySpaceGlobals->bullets[x].x = x;
			mySpaceGlobals->bullets[x].y = y;
			mySpaceGlobals->bullets[x].slope = slope;
			mySpaceGlobals->bullets[x].active = 1;
			break;
		}
	}
}

void p1Shoot(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->touched)
	{
//		mySpaceGlobals->angle = atan2(mySpaceGlobals->touchY - mySpaceGlobals->p1Y, mySpaceGlobals->touchX - mySpaceGlobals->p1X);
		// shoot a bullet
		float slope = (mySpaceGlobals->touchY - mySpaceGlobals->p1Y) / (mySpaceGlobals->touchX - mySpaceGlobals->p1X);
		activateBullet(mySpaceGlobals, slope, mySpaceGlobals->touchX, mySpaceGlobals->touchY);
	}
}

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
	
	// don't update angle if both are 0
	if (xdif == 0 && ydif == 0) return;
	
	// accept x and y movement from either stick
	mySpaceGlobals->p1X += xdif/** * mySpaceGlobals->speed **/;
	mySpaceGlobals->p1Y -= ydif/** * mySpaceGlobals->speed **/;
	
	// calculate angle to face
	mySpaceGlobals->angle = atan2(ydif, xdif);
	mySpaceGlobals->angle -= 3.14159265/2;


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
			renderStars(mySpaceGlobals);

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

void renderStars(struct SpaceGlobals *mySpaceGlobals)
{
	drawPixels(mySpaceGlobals->stars);
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

void initStars(struct SpaceGlobals *mySpaceGlobals) 
{
	int x;
	for (x=0; x<20; x++)
	{
		mySpaceGlobals->bullets[x].active = 0;
	}
	
	// create the stars randomly
	for (x=0; x<200; x++)
	{
		mySpaceGlobals->stars[x].x = prand(mySpaceGlobals)*mySpaceGlobals->xMaxBoundry;
		mySpaceGlobals->stars[x].y = prand(mySpaceGlobals)*mySpaceGlobals->yMaxBoundry;
		int randomNum = prand(mySpaceGlobals)*4;
		int r,g,b;
		// half of the time make them white, 1/4 yellow, 1/4 blue
		if (randomNum < 2)
		{
			mySpaceGlobals->stars[x].r = 255;
			mySpaceGlobals->stars[x].g = 255;
			mySpaceGlobals->stars[x].b = 255;
		}
		else if (randomNum < 3)
		{
			mySpaceGlobals->stars[x].r = 255;
			mySpaceGlobals->stars[x].g = 255;
			mySpaceGlobals->stars[x].b = 0;
		}
		else
		{
			mySpaceGlobals->stars[x].r = 0;
			mySpaceGlobals->stars[x].g = 0;
			mySpaceGlobals->stars[x].b = 255;
		}
	}
}

void renderReset(struct SpaceGlobals *mySpaceGlobals)
{
	fillScreen(0,0,0,0);
	initStars(mySpaceGlobals);
}
