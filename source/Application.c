#include "program.h"
#include "Application.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"

void executeThread(u8* screen)
{
	/****************************>             Globals             <****************************/
    struct SpaceGlobals mySpaceGlobals = { };
	//Flag for restarting the entire game.
	mySpaceGlobals.restart = 1;

	// initial state is title screen
	mySpaceGlobals.state = 1;
	mySpaceGlobals.titleScreenRefresh = 1;

	//Flags for render states
	mySpaceGlobals.renderResetFlag = 0;
	mySpaceGlobals.menuChoice = 0; // 0 is play, 1 is password
	
	// setup the password list
	unsigned int pwSeed = 27;
	int x;
	for (x=0; x<100; x++)
		mySpaceGlobals.passwordList[x] = (int)(prand(&pwSeed)*100000);
	
    mySpaceGlobals.screen = screen;
	// set the starting time
//	unsigned int coreinit_handle;
//	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
//	int64_t (*OSGetTime)();
//	OSDynLoad_FindExport(coreinit_handle, 0, "OSGetTime", &OSGetTime);
	mySpaceGlobals.seed = (unsigned int)(svcGetSystemTick() / 446872);;
	
	/****************************>            VPAD Loop            <****************************/
        
	// decompress compressed things into their arrays, final argument is the transparent color in their palette
	decompress_sprite(3061, 200, 100, compressed_title, mySpaceGlobals.title, 39);
//	decompress_sprite(511, 36, 36, compressed_ship, mySpaceGlobals.orig_ship, 14);
//	decompress_sprite(206, 23, 23, compressed_enemy, mySpaceGlobals.enemy, 9);
    return;
    
	// setup palette and transparent index
	mySpaceGlobals.curPalette = ship_palette;
	mySpaceGlobals.transIndex = 14;
    
    mySpaceGlobals.passwordEntered = 0;
	
	// initialize starfield for this game
	initStars(&mySpaceGlobals);
	
	mySpaceGlobals.invalid = 1;
    
    int exitApplication = 0;
		
	while(!exitApplication)
	{
        gspWaitForVBlank();

        hidScanInput();
        mySpaceGlobals.button = hidKeysDown();
		
//		mySpaceGlobals.touched = vpad_data.tpdata.touched;
//		if (mySpaceGlobals.touched == 1)
//		{
//			mySpaceGlobals.touchX = ((vpad_data.tpdata.x / 9) - 11);
//			mySpaceGlobals.touchY = ((3930 - vpad_data.tpdata.y) / 16);
//		}
		
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
		//To exit the game
		if (mySpaceGlobals.button&KEY_SELECT)
		{
			break;
		}

	}
}
