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
			mySpaceGlobals->bullets[xx].m_x = 9*sin(theta); // 9 is the desired bullet speed 
			mySpaceGlobals->bullets[xx].m_y = 9*cos(theta); // we have to solve for the hypotenuese 
			mySpaceGlobals->bullets[xx].active = 1;
			break;
		}
	}
	
//	for (xx=0; xx<100; xx++)
//	{
//		if (mySpaceGlobals->enemies[xx].position.active != 1)
//		{
//			mySpaceGlobals->enemies[xx].position.x = x + 18;
//			mySpaceGlobals->enemies[xx].position.y = y + 18;
//			mySpaceGlobals->enemies[xx].position.m_x = 6*sin(theta); // 9 is the desired bullet speed 
//			mySpaceGlobals->enemies[xx].position.m_y = 6*cos(theta); // we have to solve for the hypotenuese 
//			mySpaceGlobals->enemies[xx].position.active = 1;
//			break;
//		}
//	}
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
		activateBullet(mySpaceGlobals, mySpaceGlobals->angle - 3.14159265, mySpaceGlobals->p1X, mySpaceGlobals->p1Y);
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
	xdif = (xdif >  1 || mySpaceGlobals->hold_button & BUTTON_RIGHT)?  1 : xdif;
	xdif = (xdif < -1 || mySpaceGlobals->hold_button &  BUTTON_LEFT)? -1 : xdif;
	ydif = (ydif >  1 || mySpaceGlobals->hold_button &    BUTTON_UP)?  1 : ydif;
	ydif = (ydif < -1 || mySpaceGlobals->hold_button &  BUTTON_DOWN)? -1 : ydif;
	
	// don't update angle if both are within -.1 < x < .1
	// (this is an expenesive check... 128 bytes compared to just ==0)
	if (xdif < 0.1 && xdif > -0.1 && ydif < 0.1 && ydif > -0.1) return;
	
	// invalid view
	mySpaceGlobals->invalid = 1;
	
	// accept x and y movement from either stick
	mySpaceGlobals->p1X += xdif*5;
	mySpaceGlobals->p1Y -= ydif*5;
	
	handleCollisions(mySpaceGlobals);
	
	// calculate angle to face
	mySpaceGlobals->angle = atan2(ydif, xdif) - 3.14159265/2;

};

void checkPause(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->button & BUTTON_PLUS)
	{
		// switch to the pause state and mark view as invalid
		mySpaceGlobals->state = 3;
		mySpaceGlobals->invalid = 1;
	}
}

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

void makeRotationMatrix(float angle, int width, unsigned char original[][width], unsigned char target[][width], int transIndex)
{
//	if (angle < 0 || angle > 2*3.14159265) return;
	int x;
	for (x=0; x<width; x++)
	{
		int y;
		for (y=0; y<width; y++)
		{
			target[x][y] = transIndex;
		}
	}
	
	float woffset = width/2.0;
	
	// go though every pixel in the original bitmap
	for (x=0; x<width; x++)
	{
		int y;
		for (y=0; y<width; y++)
		{
			// rotate the pixel by the angle into a new spot in the rotation matrix
			int newx = (int)((x-woffset)*cos(angle) - (y-woffset)*sin(angle) + woffset);
			int newy = (int)((x-woffset)*sin(angle) + (y-woffset)*cos(angle) + woffset);
			
			if (newx < 0) newx += width;
			if (newy < 0) newy += width;
			
			if (original[x][y] == transIndex) continue;
			
			if (newx < 0 || newx >= width) continue;
			if (newy < 0 || newy >= width) continue;
			
			target[newx][newy] = original[x][y];
		}
	}
}

void renderEnemies(struct SpaceGlobals *mySpaceGlobals)
{
	// for all active enemies, advance them
	int x=0;
	for (x=0; x<100; x++) // up to 100 enemies at once
	{
		if (mySpaceGlobals->enemies[x].position.active == 1)
		{
			drawBitmap(mySpaceGlobals->services, mySpaceGlobals->enemies[x].position.x, mySpaceGlobals->enemies[x].position.y, 23, 23, mySpaceGlobals->enemies[x].rotated_sprite, enemy_palette);
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

		renderStars(mySpaceGlobals);
		renderBullets(mySpaceGlobals);
		renderEnemies(mySpaceGlobals);
		renderShip(mySpaceGlobals);
		renderTexts(mySpaceGlobals);

		flipBuffers(mySpaceGlobals->services);
		mySpaceGlobals->invalid = 0;
	}
}

// see the notes in images.c for more info on how this works
void decompress_sprite(int arraysize, int width, int height, const signed char* input, unsigned char target[][width], char transIndex)
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
		
		if (count == -120) // full value rows of last index in palette
		{
			for (z=0; z<value; z++)
			{
				int za;
				for (za=0; za<width; za++)
				{
					target[cy+z][cx+za] = transIndex;
				}
			}
			
			cy += value;
			continue;
		}
////		
		if (count <= 0) // if it's negative, -count is value, and value is meaningless and advance by one
		{
			value = -1*count;
			count = 1;
			x--; // subtract one, so next time it goes up by 2, putting us at x+1
		}

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
	
	for (x=0; x<100; x++)
	{
		if (mySpaceGlobals->enemies[x].position.active == 1)
		{
			mySpaceGlobals->enemies[x].position.x += mySpaceGlobals->enemies[x].position.m_x;
			mySpaceGlobals->enemies[x].position.y += mySpaceGlobals->enemies[x].position.m_y;
						
			if (mySpaceGlobals->enemies[x].position.x > xMaxBoundry ||
				mySpaceGlobals->enemies[x].position.x < xMinBoundry ||
				mySpaceGlobals->enemies[x].position.y > yMaxBoundry ||
				mySpaceGlobals->enemies[x].position.y < yMinBoundry + 20)
				mySpaceGlobals->enemies[x].position.active = 0;
			
			// rotate the enemy slowly
			mySpaceGlobals->enemies[x].angle += 0.02;
			if (mySpaceGlobals->enemies[x].angle > 6.28318530)
				mySpaceGlobals->enemies[x].angle = 0;
			
			makeRotationMatrix(mySpaceGlobals->enemies[x].angle, 23, mySpaceGlobals->enemy, mySpaceGlobals->enemies[x].rotated_sprite, 9);
			
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
			
			int z, za;
			for (z=0; z<4; z++)
				for (za=0; za<2; za++)
					drawPixel(mySpaceGlobals->services, mySpaceGlobals->bullets[x].x + z, mySpaceGlobals->bullets[x].y + za, 255, 0, 0);
		}
	}
}

					
void renderTexts(struct SpaceGlobals *mySpaceGlobals)
{
	fillRect(mySpaceGlobals->services, 0, 0, xMaxBoundry, 20, 0, 40, 40);

	char score[255];
	__os_snprintf(score, 255, "Score: %d", mySpaceGlobals->score);
	drawString(mySpaceGlobals->services, 0, -1, score);

	char lives[255];
	__os_snprintf(lives, 255, "Lives: %d", mySpaceGlobals->lives);
	drawString(mySpaceGlobals->services, 55, -1, lives);
			
}

void renderShip(struct SpaceGlobals *mySpaceGlobals) 
{
	const int posx = (int)mySpaceGlobals->p1X;
	const int posy = (int)mySpaceGlobals->p1Y;
	
	makeRotationMatrix(mySpaceGlobals->angle, 36, mySpaceGlobals->orig_ship, mySpaceGlobals->rotated_ship, 14);

	drawBitmap(mySpaceGlobals->services, posx, posy, 36, 36, mySpaceGlobals->rotated_ship, ship_palette);

}

void renderStars(struct SpaceGlobals *mySpaceGlobals)
{
	drawPixels(mySpaceGlobals->services, mySpaceGlobals->stars);
}

//Reset the game
void reset(struct SpaceGlobals *mySpaceGlobals) {
	mySpaceGlobals->button = 0;

	//Set flag to render reset screen;
	mySpaceGlobals->renderResetFlag = 1;

};

void initGameState(struct SpaceGlobals *mySpaceGlobals)
{
	// init bullets
	int x;
	for (x=0; x<20; x++)
	{
		mySpaceGlobals->bullets[x].active = 0;
	}
	
	// init x and y pos of player
	mySpaceGlobals->p1X =  40;
	mySpaceGlobals->p1Y = 150;
	
	// init enemies
	for (x=0; x<100; x++)
	{
		mySpaceGlobals->enemies[x].position.active = 0;
	}

}

void initStars(struct SpaceGlobals *mySpaceGlobals) 
{
	// create the stars randomly
	int x;
	for (x=0; x<200; x++)
	{
		mySpaceGlobals->stars[x].x = (int)(prand(&mySpaceGlobals->seed)*xMaxBoundry);
		mySpaceGlobals->stars[x].y = (int)(prand(&mySpaceGlobals->seed)*yMaxBoundry);
		int randomNum = (int)(prand(&mySpaceGlobals->seed)*4);
		
		// half of the time make them white, 1/4 yellow, 1/4 blue
		mySpaceGlobals->stars[x].r = (randomNum <= 2)? 255 : 0;
		mySpaceGlobals->stars[x].g = (randomNum <= 2)? 255 : 0;
		mySpaceGlobals->stars[x].b = (randomNum != 2)? 255 : 0;
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
	// cover up any old cursors (used to be needed before changing to draw everything mode)
//	fillRect(mySpaceGlobals->services, 155, 155, 16, 30, 0, 0, 0);
//	fillRect(mySpaceGlobals->services, 240, 155, 16, 30, 0, 0, 0);

	// display the cursor on the correct item
	char cursor[255];
	__os_snprintf(cursor, 255, ">>            <<");
	drawString(mySpaceGlobals->services, 22, 12 + mySpaceGlobals->menuChoice, cursor);	
}

void doMenuAction(struct SpaceGlobals *mySpaceGlobals)
{
	// if we've seen the A button not being pressed
	if (!(mySpaceGlobals->button & BUTTON_A))
	{
		mySpaceGlobals->allowInput = 1;
	}
	
	if (mySpaceGlobals->button & BUTTON_A && mySpaceGlobals->allowInput)
	{
		// if we're on the title menu
		if (mySpaceGlobals->state == 1)
		{
			if (mySpaceGlobals->menuChoice == 0)
			{
				// start game chosen
				mySpaceGlobals->state = 7; // switch to game state
				mySpaceGlobals->renderResetFlag = 1; // redraw screen
			}
			else if (mySpaceGlobals->menuChoice == 1)
			{
				// password screen chosen
				mySpaceGlobals->state = 2;
			}
		}
		// password screen
		else if (mySpaceGlobals->state == 2)
		{
			
		}
		// pause screen 
		else if (mySpaceGlobals->state == 3)
		{
			if (mySpaceGlobals->menuChoice == 0)
			{
				// resume chosen
				mySpaceGlobals->state = 7; // switch to game state
				
			}
			else if (mySpaceGlobals->menuChoice == 1)
			{
				// quit chosen
				mySpaceGlobals->state = 1;
			}
		}
		
		// reset the choice
		mySpaceGlobals->menuChoice = 0;
		
		// disable menu input after selecting to prevent double selects
		mySpaceGlobals->allowInput = 0;

		// mark view invalid to redraw
		mySpaceGlobals->invalid = 1;
	}
	
	float stickY = mySpaceGlobals->lstick.y + mySpaceGlobals->rstick.y;
	
	if (mySpaceGlobals->button & BUTTON_DOWN || stickY < -0.3)
	{
		mySpaceGlobals->menuChoice = 1;
		mySpaceGlobals->invalid = 1;
	}
	
	if (mySpaceGlobals->button & BUTTON_UP || stickY > 0.3)
	{
		mySpaceGlobals->menuChoice = 0;
		mySpaceGlobals->invalid = 1;
	}
}

void displayPause(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->invalid == 1)
	{
		blackout(mySpaceGlobals->services);

		// display the password here
		char resume[255];
		__os_snprintf(resume, 255, "Resume");
		char quit[255];
		__os_snprintf(quit, 255, "Quit");
		
		drawString(mySpaceGlobals->services, 25, 12, resume);
		drawString(mySpaceGlobals->services, 25, 13, quit);
		
		drawMenuCursor(mySpaceGlobals);
		
		flipBuffers(mySpaceGlobals->services);
		mySpaceGlobals->invalid = 0;
	}
}

void doPasswordMenuAction(struct SpaceGlobals * mySpaceGlobals)
{
	// if we've seen up, down, left, right, and a buttons not being pressed
	if (!(mySpaceGlobals->button & BUTTON_A & BUTTON_LEFT & BUTTON_RIGHT & BUTTON_UP & BUTTON_DOWN))
	{
		mySpaceGlobals->allowInput = 1;
	}
	
	if (mySpaceGlobals->allowInput)
	{
		if (mySpaceGlobals->button & BUTTON_B)
		{
			// go back to title screen
			mySpaceGlobals->state = 1;
			
			// update the menu choice
			mySpaceGlobals->menuChoice = 0;
			
			// disable menu input after selecting to prevent double selects
			mySpaceGlobals->allowInput = 0;

			// mark view invalid to redraw
			mySpaceGlobals->invalid = 1;
		}
		if (mySpaceGlobals->button & BUTTON_A)
		{
			// try the password
	//		tryPassword();

			// disable menu input after selecting to prevent double selects
			mySpaceGlobals->allowInput = 0;
			
			// update the menu choice
			mySpaceGlobals->menuChoice = 0;

			// mark view invalid to redraw
			mySpaceGlobals->invalid = 1;
		}
		
		float stickY = mySpaceGlobals->lstick.y + mySpaceGlobals->rstick.y;
		float stickX = mySpaceGlobals->lstick.x + mySpaceGlobals->rstick.x;
		int down  = (mySpaceGlobals->button & BUTTON_DOWN  || stickY < -0.3);
		int up    = (mySpaceGlobals->button & BUTTON_UP    || stickY >  0.3);
		int left  = (mySpaceGlobals->button & BUTTON_LEFT  || stickX < -0.3);
		int right = (mySpaceGlobals->button & BUTTON_RIGHT || stickX >  0.3);
		
		if (up || down)
		{
			int offset = 1, x;
			// keep going up in the 10s place to match current choice
			for (x=0; x<(4 - mySpaceGlobals->menuChoice); x++)
				offset *= 10;
				
			if (up)
				mySpaceGlobals->passwordEntered += offset;
			if (down)
				mySpaceGlobals->passwordEntered -= offset;
				
			mySpaceGlobals->invalid = 1;
			mySpaceGlobals->allowInput = 0;
		}

		if (left || right)
		{
			if (right)
				mySpaceGlobals->menuChoice ++;
			if (left)
				mySpaceGlobals->menuChoice --;;
			
			// bound the menu choices
			if (mySpaceGlobals->menuChoice < 0)
				mySpaceGlobals->menuChoice = 0;
			if (mySpaceGlobals->menuChoice > 4)
				mySpaceGlobals->menuChoice = 4;
			
			mySpaceGlobals->invalid = 1;
			mySpaceGlobals->allowInput = 0;
		}
		
		// bound the password
		if (mySpaceGlobals->passwordEntered < 0)
			mySpaceGlobals->passwordEntered = 0;
		if (mySpaceGlobals->passwordEntered > 99999)
			mySpaceGlobals->passwordEntered = 99999;
	}
}

void displayPasswordScreen(struct SpaceGlobals * mySpaceGlobals)
{
	if (mySpaceGlobals->invalid == 1)
	{
		blackout(mySpaceGlobals->services);

		
//		drawPasswordMenuCursor(mySpaceGlobals);
		char password[255];
		__os_snprintf(password, 255, "Password:");
		char up_cur[255];
		__os_snprintf(up_cur, 255, "v");
		char cur_pw[255];
		__os_snprintf(cur_pw, 255, "%05d", mySpaceGlobals->passwordEntered);
		char down_cur[255];
		__os_snprintf(down_cur, 255, "^");
		
		drawString(mySpaceGlobals->services, 22, 8, password);
		
		drawString(mySpaceGlobals->services, 32 + mySpaceGlobals->menuChoice, 7, up_cur);
		drawString(mySpaceGlobals->services, 32, 8, cur_pw);
		drawString(mySpaceGlobals->services, 32 + mySpaceGlobals->menuChoice, 9, down_cur);
		
		flipBuffers(mySpaceGlobals->services);
		mySpaceGlobals->invalid = 0;
	}
}

void renderReset(struct SpaceGlobals *mySpaceGlobals)
{
	initGameState(mySpaceGlobals);
	mySpaceGlobals->renderResetFlag = 0;
	mySpaceGlobals->invalid = 1;
}
