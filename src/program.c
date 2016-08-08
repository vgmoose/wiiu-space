#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <coreinit/internal.h>
#include <coreinit/screen.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>
#include "memory.h"

#include "program.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"

char log_buf[0x400];

bool isAppRunning = true;
bool initialized = false;

void screenInit()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	/*__os_snprintf(log_buf, 0x400, "Screen sizes %x, %x\n", buf0_size, buf1_size);
	OSReport(log_buf);*/
	
	//Set the buffer area.
	screenBuffer = MEM1_alloc(buf0_size + buf1_size, 0x40);
	/*__os_snprintf(log_buf, 0x400, "Allocated screen buffers at %x\n", screenBuffer);
	OSReport(log_buf);*/
	
    OSScreenSetBufferEx(0, screenBuffer + 0x40);
    OSScreenSetBufferEx(1, (screenBuffer + buf0_size + 0x40));
    //OSReport("Set screen buffers\n");

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);
    
    //Clear both framebuffers.
	for (int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}

void screenDeinit()
{
    for(int ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
    
    MEM1_free(screenBuffer);
}

void SaveCallback()
{
   OSSavesDone_ReadyToRelease(); // Required
}

bool AppRunning()
{
   if(!OSIsMainCore())
   {
      ProcUISubProcessMessages(true);
   }
   else
   {
      ProcUIStatus status = ProcUIProcessMessages(true);
    
      if(status == PROCUI_STATUS_EXITING)
      {
          // Being closed, deinit things and prepare to exit
          isAppRunning = false;
          
          if(initialized)
          {
              initialized = false;
              screenDeinit();
              memoryRelease();
          }
          ProcUIShutdown();
      }
      else if(status == PROCUI_STATUS_RELEASE_FOREGROUND)
      {
          // Free up MEM1 to next foreground app, etc.
          initialized = false;
          
          screenDeinit();
          memoryRelease();
          ProcUIDrawDoneRelease();
      }
      else if(status == PROCUI_STATUS_IN_FOREGROUND)
      {
         // Reallocate MEM1, reinit screen, etc.
         if(!initialized)
         {
            initialized = true;
            
            memoryInitialize();
            screenInit();
         }
      }
   }

   return isAppRunning;
}

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
	OSDynLoadModule avm_handle = 0;
	OSDynLoad_Acquire("avm.rpl", &avm_handle);
    bool(*AVMSetTVScale)(int width, int height);
    OSDynLoad_FindExport(avm_handle, 0, "AVMSetTVScale", &AVMSetTVScale);  // compiler warning, not quite understanding how they setup OSDynLoad_FindExport in WUT
    AVMSetTVScale(854, 480);  // Not working, hope to find a solution

	OSScreenInit();
	ProcUIInit(&SaveCallback);
	struct SpaceGlobals mySpaceGlobals;
	
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
	unsigned int pwSeed = 27;
	int x;
	for (x=0; x<100; x++)
		mySpaceGlobals.passwordList[x] = (int)(prand(&pwSeed)*100000);
	
	mySpaceGlobals.seed = 654321;
	
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
		
	while (AppRunning())
	{
		VPADRead(0, &vpad_data, 1, &error);
		
		//Get the status of the gamepad
		mySpaceGlobals.button = vpad_data.hold;
		
		mySpaceGlobals.rstick = vpad_data.rightStick;
		mySpaceGlobals.lstick = vpad_data.leftStick;
		
		mySpaceGlobals.touched = vpad_data.tpNormal.touched;
		if (mySpaceGlobals.touched == 1)
		{
			mySpaceGlobals.touchX = ((vpad_data.tpNormal.x / 9) - 11);
			mySpaceGlobals.touchY = ((3930 - vpad_data.tpNormal.y) / 16);
		}
		
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

	return 0;
}