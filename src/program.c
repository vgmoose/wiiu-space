#ifdef __WIIU__
#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <coreinit/internal.h>
#include <coreinit/screen.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>

#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>
#endif

#include "program.h"
#include "platform.h"
#include "draw.h"
#include "images.h"
#include "space.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "music.h"

void* screenBuffer;

char log_buf[0x400];

struct SpaceGlobals mySpaceGlobals;

void cleanSlate()
{
	// clear both buffers
	int ii;
	for(ii=0;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}

int main(int argc, char **argv)
{
	platformInit();

	/****************************>             Globals             <****************************/
	//struct SpaceGlobals mySpaceGlobals;
	//Flag for restarting the entire game.
	mySpaceGlobals.restart = 1;

	// initial state is title screen
	mySpaceGlobals.state = 1;
	mySpaceGlobals.titleScreenRefresh = 1;

	//Flags for render states
	mySpaceGlobals.renderResetFlag = 0;
	mySpaceGlobals.menuChoice = 0; // 0 is play, 1 is password

	// setup the password list
	int x;
	for (x=0; x<100; x++)
		mySpaceGlobals.passwordList[x] = (int)(rand()*100000);

	mySpaceGlobals.seed = OSGetTime();

	/****************************>            VPAD Loop            <****************************/
	int error;
	VPADStatus vpad_data;

	// decompress compressed things into their arrays, final argument is the transparent color in their palette
	decompress_sprite(3061, 200, 100, compressed_title, mySpaceGlobals.title, 39);
	decompress_sprite(511, 36, 36, compressed_ship, mySpaceGlobals.orig_ship, 14);
	decompress_sprite(206, 23, 23, compressed_enemy, mySpaceGlobals.enemy, 9);

	// setup palette and transparent index
	mySpaceGlobals.curPalette = ship_palette;
	mySpaceGlobals.transIndex = 14;

	// initialize starfield for this game
	initStars(&mySpaceGlobals);

	mySpaceGlobals.invalid = 1;

	initMusicPlayer("fs:/vol/content/sounds/cruise.mp3");
	playMusic();
	while (AppRunning(&mySpaceGlobals))
	{
		updateMusic(); // TODO: use return value
		
		VPADRead(0, &vpad_data, 1, &error);

		mySpaceGlobals.rstick = vpad_data.rightStick;
		mySpaceGlobals.lstick = vpad_data.leftStick;

		//Get the status of the gamepad
#if defined(__WIIU__)
		// TODO: move out to a wiiu_paddata.c file, and use PAD instead of VPAD
		mySpaceGlobals.button = vpad_data.hold;

		mySpaceGlobals.touched = vpad_data.tpNormal.touched;
		if (mySpaceGlobals.touched == 1)
		{
			mySpaceGlobals.touchX = ((vpad_data.tpNormal.x / 9) - 11);
			mySpaceGlobals.touchY = ((3930 - vpad_data.tpNormal.y) / 16);
		}
#else
		mySpaceGlobals.button = vpad_data.btns_h;

		mySpaceGlobals.touched = vpad_data.isTouched;
		if (mySpaceGlobals.touched == 1)
		{
			mySpaceGlobals.touchX = vpad_data.touchData.x;
			mySpaceGlobals.touchY = vpad_data.touchData.y;
		}
#endif

		if (mySpaceGlobals.restart == 1)
		{
			reset(&mySpaceGlobals);
			mySpaceGlobals.restart = 0;
		}

		if (mySpaceGlobals.state == 1) // title screen
		{
			displayTitle(&mySpaceGlobals);
			doMenuAction(&mySpaceGlobals);
		}
		else if (mySpaceGlobals.state == 2) // password screen
		{
			displayPasswordScreen(&mySpaceGlobals);
			doPasswordMenuAction(&mySpaceGlobals);
		}
		else if (mySpaceGlobals.state == 3) // pause screen
		{
			displayPause(&mySpaceGlobals);
			doMenuAction(&mySpaceGlobals);
		}
		else if  (mySpaceGlobals.state == 4) // game over screen
		{
			displayGameOver(&mySpaceGlobals);
			doMenuAction(&mySpaceGlobals);
		}
		else 	// game play
		{
			//Update location of player1 and 2 paddles
			p1Move(&mySpaceGlobals);

			// perform any shooting
			p1Shoot(&mySpaceGlobals);

			// handle any collisions
			handleCollisions(&mySpaceGlobals);

			// do explosions
			handleExplosions(&mySpaceGlobals);

			// if we're out of lives, break
			if (mySpaceGlobals.lives <= 0 && mySpaceGlobals.state == 4)
				continue;

			// add any new enemies
			addNewEnemies(&mySpaceGlobals);

			//Render the scene
			render(&mySpaceGlobals);

			// check for pausing
			checkPause(&mySpaceGlobals);
		}

	}

	stopMusic();
	return 0;
}
