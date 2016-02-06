#include "space.h"
#include "trigmath.h"

/**
This class is a bit of a mess, but it basically does "everything else" in the game.
The most interesting function is rotating the bitmap (makeRotationMatrix).

Other things it handles:
	- Joystick input (p1Move)
	- Bullet firing (p1Shoot)
	- Star drawing (renderStars)
	- Status bar drawing (renderTexts)
	- Decompressing sprites (decompress_bitmap)
	- Handling the menu at the title screen (doMenuAction)
	
It relies heavily on a SpaceGlobals struct defined in space.h. This is a carry over from the libwiiu
pong example, but also I believe necesary since global variables don't seem to be able to be set(?)
**/

extern unsigned const char title_palette[41][3];
extern unsigned const char compressed_title[4156];
extern const unsigned char ship_palette[15][3];
extern const unsigned char compressed_ship[840];

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
	
	// Handle D-pad movements as well
	// max out speed at 1 or -1 in both directions
	xdif = (xdif >  1 || mySpaceGlobals->button & BUTTON_RIGHT)?  1 : xdif;
	xdif = (xdif < -1 || mySpaceGlobals->button &  BUTTON_LEFT)? -1 : xdif;
	ydif = (ydif >  1 || mySpaceGlobals->button &    BUTTON_UP)?  1 : ydif;
	ydif = (ydif < -1 || mySpaceGlobals->button &  BUTTON_DOWN)? -1 : ydif;
	
	// don't update angle if both are 0
	if (xdif == 0 && ydif == 0) return;
	
	// accept x and y movement from either stick
	mySpaceGlobals->p1X += xdif/** * mySpaceGlobals->speed **/;
	mySpaceGlobals->p1Y -= ydif/** * mySpaceGlobals->speed **/;
	
	handleCollisions(mySpaceGlobals);
	
	// calculate angle to face
	mySpaceGlobals->angle = atan2(ydif, xdif) - 3.14159265/2;


};

void handleCollisions(struct SpaceGlobals * mySpaceGlobals)
{
	int playerLeft = mySpaceGlobals->p1X;
	int playerRight = playerLeft + 36;
	int playerUp = mySpaceGlobals->p1Y;
	int playerDown = playerUp + 36;
	
	// don't let the player go offscreen
	if (playerLeft < mySpaceGlobals->xMinBoundry)
		mySpaceGlobals->p1X = mySpaceGlobals->xMinBoundry;
	if (playerRight > mySpaceGlobals->xMaxBoundry)
		mySpaceGlobals->p1X = mySpaceGlobals->xMaxBoundry - 36;
	if (playerUp < mySpaceGlobals->yMinBoundry + 20)
		mySpaceGlobals->p1Y = mySpaceGlobals->yMinBoundry + 20;
	if (playerDown > mySpaceGlobals->yMaxBoundry)
		mySpaceGlobals->p1Y = mySpaceGlobals->yMaxBoundry - 36;
	
	
}

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
			
			if (mySpaceGlobals->orig_ship[x][y] == 14) continue;
			
			if (newx < 0 || newx >= width) continue;
			if (newy < 0 || newy >= width) continue;
			
			mySpaceGlobals->rotated_ship[newx][newy] = mySpaceGlobals->orig_ship[x][y];
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
//			renderStars(mySpaceGlobals);

		}
			
		flipBuffers();
	}

	resetRenderFlags(mySpaceGlobals);

}

void decompress_sprite(int arraysize, int width, int height, const unsigned char* input, unsigned char target[][width])
{
	int cx = 0, cy = 0;
	int x;
	int posinrow = 0;
	// go through input array
	for (x=0; x<arraysize; x+=2)
	{
		int count = input[x];
		char value = input[x+1];
		int z;
		for (z=0; z<count; z++)
		{
			target[cy][cx] = value;
			cx++;
		}
		posinrow += z;
		if (posinrow >= width)
		{
			posinrow = 0;
			cx = 0;
			cy++;
		}
	}
}

void renderTexts(struct SpaceGlobals *mySpaceGlobals)
{
	// every 60th frame draw the status bar
	if (mySpaceGlobals->frame % 60 == 0) {
		fillRect(mySpaceGlobals->services, 0, 0, mySpaceGlobals->xMaxBoundry, 20);

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

	drawBitmap(mySpaceGlobals->services, posx, posy, 36, 36, mySpaceGlobals->rotated_ship, ship_palette);

}

void renderStars(struct SpaceGlobals *mySpaceGlobals)
{
	drawPixels(mySpaceGlobals->services, mySpaceGlobals->stars);
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
//	for (x=0; x<200; x++)
//	{
//		mySpaceGlobals->stars[x].x = prand(mySpaceGlobals)*mySpaceGlobals->xMaxBoundry;
//		mySpaceGlobals->stars[x].y = prand(mySpaceGlobals)*mySpaceGlobals->yMaxBoundry;
//		int randomNum = prand(mySpaceGlobals)*4;
//		int r,g,b;
//		// half of the time make them white, 1/4 yellow, 1/4 blue
//		if (randomNum < 2)
//		{
//			mySpaceGlobals->stars[x].r = 255;
//			mySpaceGlobals->stars[x].g = 255;
//			mySpaceGlobals->stars[x].b = 255;
//		}
//		else if (randomNum < 3)
//		{
//			mySpaceGlobals->stars[x].r = 255;
//			mySpaceGlobals->stars[x].g = 255;
//			mySpaceGlobals->stars[x].b = 0;
//		}
//		else
//		{
//			mySpaceGlobals->stars[x].r = 0;
//			mySpaceGlobals->stars[x].g = 0;
//			mySpaceGlobals->stars[x].b = 255;
//		}
//	}
}

void displayTitle(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->titleScreenRefresh == 1)
	{
		// decompress our title image from the compressed array
		decompress_sprite(4156, 200, 100, compressed_title, mySpaceGlobals->title);

		int ii;
		for (ii=0; ii<2; ii++)
		{
			// display the bitmap in upper center screen
			drawBitmap(mySpaceGlobals->services, 107, 30, 200, 100, mySpaceGlobals->title, title_palette);
			
			char credits[255];
			__os_snprintf(credits, 255, "by vgmoose");
			
			char play[255];
			__os_snprintf(play, 255, "Start Game");
			char password[255];
			__os_snprintf(password, 255, "Password");
			
			//display the menu under it
			drawString(37, 9, credits);
			drawString(25, 12, play);
			drawString(25, 13, password);
			
			flipBuffers();
		}
		
		drawMenuCursor(mySpaceGlobals);
		
		mySpaceGlobals->titleScreenRefresh = 0;
	}
}

void drawMenuCursor(struct SpaceGlobals *mySpaceGlobals)
{
	int ii;
	for (ii=0; ii<2; ii++)
	{
		// cover up any old cursors
//		fillRect(100, 100, 50, 50);
//		fillRect(200, 100, 50, 50);

		// display the cursor on the correct item
		char cursor[255];
		__os_snprintf(cursor, 255, ">>            <<");
		drawString(22, 12 + mySpaceGlobals->menuChoice, cursor);
		
		flipBuffers();
	}

}

void doMenuAction(struct SpaceGlobals *mySpaceGlobals)
{
	if (mySpaceGlobals->button & BUTTON_A || mySpaceGlobals->button & BUTTON_PLUS)
	{
		mySpaceGlobals->state = 2; // start game
		mySpaceGlobals->renderResetFlag = 1; // redraw screen
	}
	
	if (mySpaceGlobals->button & BUTTON_DOWN || mySpaceGlobals->lstick.y > 0.03)
	{
		mySpaceGlobals->menuChoice = 1;
		drawMenuCursor(mySpaceGlobals);
	}
	
	if (mySpaceGlobals->button & BUTTON_UP || mySpaceGlobals->lstick.y < 0.03)
	{
		mySpaceGlobals->menuChoice = 0;
		drawMenuCursor(mySpaceGlobals);
	}
}

void renderReset(struct SpaceGlobals *mySpaceGlobals)
{
	fillScreen(0,0,0,0);
	decompress_sprite(840, 36, 36, compressed_ship, mySpaceGlobals->orig_ship);

	initStars(mySpaceGlobals);
}
