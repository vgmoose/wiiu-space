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

#define xMinBoundry 0
#define xMaxBoundry 427
#define yMinBoundry 0
#define yMaxBoundry 240

void activateBullet(struct SpaceGlobals * mySpaceGlobals, float theta, int x, int y)
{
	// find an inactive bullet
	int xx;
	for (xx=0; xx<20; xx++)
	{
		if (mySpaceGlobals->bullets[xx].active != 1)
		{
			mySpaceGlobals->bullets[xx].x = x + 18;
			mySpaceGlobals->bullets[xx].y = y + 18;
			mySpaceGlobals->bullets[xx].m_x = 2*cos(theta);
			mySpaceGlobals->bullets[xx].m_y = 2*sin(theta);
			mySpaceGlobals->bullets[xx].active = 1;
			break;
		}
	}
}

void blackout(struct Services * services)
{
	fillScreen(services, 0,0,0,0);
}

void p1Shoot(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->touched)
	{
		float xdif = mySpaceGlobals->p1X - mySpaceGlobals->touchX + 18;
		float ydif = mySpaceGlobals->p1Y - mySpaceGlobals->touchY + 18;
		mySpaceGlobals->angle = atan2(xdif, ydif) /*- 3.14159265/2*/;
		
		// shoot a bullet
		activateBullet(mySpaceGlobals, mySpaceGlobals->angle, mySpaceGlobals->p1X, mySpaceGlobals->p1Y);
	}
	
	moveBullets(mySpaceGlobals);
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
	
	// don't update angle if both are within -.1 < x < .1
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
	if (playerLeft < xMinBoundry)
		mySpaceGlobals->p1X = xMinBoundry;
	if (playerRight > xMaxBoundry)
		mySpaceGlobals->p1X = xMaxBoundry - 36;
	if (playerUp < yMinBoundry + 20)
		mySpaceGlobals->p1Y = yMinBoundry + 20;
	if (playerDown > yMaxBoundry)
		mySpaceGlobals->p1Y = yMaxBoundry - 36;
	
	
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
	if (mySpaceGlobals->invalid == 1)
	{
		blackout(mySpaceGlobals->services);

		mySpaceGlobals->frame++;

		if (mySpaceGlobals->renderResetFlag)
		{
			renderReset(mySpaceGlobals);
		}

		renderShip(mySpaceGlobals);
		renderTexts(mySpaceGlobals);
		renderBullets(mySpaceGlobals);

		flipBuffers(mySpaceGlobals->services);
		mySpaceGlobals->invalid = 0;
	}
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

void moveBullets(struct SpaceGlobals *mySpaceGlobals)
{
	// for all active bullets, advance them
	int x=0;
	for (x=0; x<20; x++)
	{
		if (mySpaceGlobals->bullets[x].active == 1)
		{
			mySpaceGlobals->bullets[x].x += mySpaceGlobals->bullets[x].m_x;
			mySpaceGlobals->bullets[x].y += mySpaceGlobals->bullets[x].m_y;
						
			if (mySpaceGlobals->bullets[x].x > xMaxBoundry ||
				mySpaceGlobals->bullets[x].x < xMinBoundry ||
				mySpaceGlobals->bullets[x].y > yMaxBoundry ||
				mySpaceGlobals->bullets[x].y < yMinBoundry + 20)
				mySpaceGlobals->bullets[x].active = 0;
			
			mySpaceGlobals->invalid = 1;
		}
		
	}
}
					  
void renderBullets(struct SpaceGlobals *mySpaceGlobals)
{
	// for all active bullets, advance them
	int x=0;
	for (x=0; x<20; x++)
	{
		if (mySpaceGlobals->bullets[x].active == 1)
		{
			
			drawPixel(mySpaceGlobals->services, mySpaceGlobals->bullets[x].x, mySpaceGlobals->bullets[x].y, 255, 0, 0);
			// undraw the previous space
			drawPixel(mySpaceGlobals->services, mySpaceGlobals->bullets[x].x - mySpaceGlobals->bullets[x].m_x, mySpaceGlobals->bullets[x].y - mySpaceGlobals->bullets[x].m_y, 255, 0, 0);
		}
		
	}
}

void renderTexts(struct SpaceGlobals *mySpaceGlobals)
{
	// every 60th frame draw the status bar
	if (mySpaceGlobals->frame == 60) {
		fillRect(mySpaceGlobals->services, 0, 0, xMaxBoundry, 20, 0, 40, 40);

		char score[255];
		__os_snprintf(score, 255, "Score: %d", mySpaceGlobals->score);
		drawString(mySpaceGlobals->services, 0, -1, score);

		char lives[255];
		__os_snprintf(lives, 255, "Lives: %d", mySpaceGlobals->lives);
		drawString(mySpaceGlobals->services, 55, -1, lives);
	}
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
	mySpaceGlobals->p1X =  40;
	mySpaceGlobals->p1Y = 150;
	
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
		mySpaceGlobals->stars[x].x = (int)(prand(&mySpaceGlobals->seed)*xMaxBoundry);
		mySpaceGlobals->stars[x].y = (int)(prand(&mySpaceGlobals->seed)*yMaxBoundry);
		int randomNum = (int)(prand(&mySpaceGlobals->seed)*4);
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
			mySpaceGlobals->stars[x].b =   0;
		}
		else
		{
			mySpaceGlobals->stars[x].r =   0;
			mySpaceGlobals->stars[x].g =   0;
			mySpaceGlobals->stars[x].b = 255;
		}
	}
}

void displayTitle(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->invalid == 1)
	{
		blackout(mySpaceGlobals->services);
		
		// draw some stars
		renderStars(mySpaceGlobals);

		// display the bitmap in upper center screen
		drawBitmap(mySpaceGlobals->services, 107, 30, 200, 100, mySpaceGlobals->title, title_palette);

		char credits[255];
		__os_snprintf(credits, 255, "by vgmoose");

		char play[255];
		__os_snprintf(play, 255, "Start Game");
		char password[255];
		__os_snprintf(password, 255, "Password");

		//display the menu under it
		drawString(mySpaceGlobals->services, 37, 9, credits);
		drawString(mySpaceGlobals->services, 25, 12, play);
		drawString(mySpaceGlobals->services, 25, 13, password);
		
		drawMenuCursor(mySpaceGlobals);
		
		flipBuffers(mySpaceGlobals->services);
		mySpaceGlobals->invalid = 0;
	}

}

void drawMenuCursor(struct SpaceGlobals *mySpaceGlobals)
{
	// cover up any old cursors
	fillRect(mySpaceGlobals->services, 155, 155, 16, 30, 0, 0, 0);
	fillRect(mySpaceGlobals->services, 240, 155, 16, 30, 0, 0, 0);

	// display the cursor on the correct item
	char cursor[255];
	__os_snprintf(cursor, 255, ">>            <<");
	drawString(mySpaceGlobals->services, 22, 12 + mySpaceGlobals->menuChoice, cursor);
	
}

void doMenuAction(struct SpaceGlobals *mySpaceGlobals)
{
	if (mySpaceGlobals->button & BUTTON_A || mySpaceGlobals->button & BUTTON_PLUS)
	{
		mySpaceGlobals->state = 2; // start game
		mySpaceGlobals->renderResetFlag = 1; // redraw screen
	}
	
	if (mySpaceGlobals->button & BUTTON_DOWN || mySpaceGlobals->lstick.y < -0.3)
	{
		mySpaceGlobals->menuChoice = 1;
		mySpaceGlobals->invalid = 1;
	}
	
	if (mySpaceGlobals->button & BUTTON_UP || mySpaceGlobals->lstick.y > 0.3)
	{
		mySpaceGlobals->menuChoice = 0;
		mySpaceGlobals->invalid = 1;
	}
}

void renderReset(struct SpaceGlobals *mySpaceGlobals)
{
	blackout(mySpaceGlobals->services);
	renderStars(mySpaceGlobals);
}
