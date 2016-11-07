#include "program.h"
#include "Application.h"
#include "common/common.h"
#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <coreinit/internal.h>
#include <coreinit/screen.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>
// #include "dynamic_libs/os_functions.h"
// #include "gui/FreeTypeGX.h"
// #include "gui/VPadController.h"
// #include "gui/WPadController.h"
#include "resources/Resources.h"
#include "sounds/SoundHandler.hpp"
#include "utils/logger.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"
#include "memory.h"


bool isAppRunning = true;
bool initialized = false;
bool Application::exitApplication = false;

struct SpaceGlobals mySpaceGlobals = {};
void* screenBuffer;

void screenInit()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(OSScreenID::SCREEN_TV);
	int buf1_size = OSScreenGetBufferSizeEx(OSScreenID::SCREEN_DRC);
	/*__os_snprintf(log_buf, 0x400, "Screen sizes %x, %x\n", buf0_size, buf1_size);
	OSReport(log_buf);*/
	
	//Set the buffer area.
	screenBuffer = MEM1_alloc(buf0_size + buf1_size, 0x100);
	/*__os_snprintf(log_buf, 0x400, "Allocated screen buffers at %x\n", screenBuffer);
	OSReport(log_buf);*/
	
    OSScreenSetBufferEx(OSScreenID::SCREEN_TV, screenBuffer);
    OSScreenSetBufferEx(OSScreenID::SCREEN_DRC, (screenBuffer + buf0_size));
    //OSReport("Set screen buffers\n");

    OSScreenEnableEx(OSScreenID::SCREEN_TV, 1);
    OSScreenEnableEx(OSScreenID::SCREEN_DRC, 1);
    
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

bool Application::procUI(void)
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
            
            // redraw the screen upon resume
            mySpaceGlobals.invalid = 1;
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

Application *Application::applicationInstance = NULL;

Application::Application()
	: CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	, bgMusic(NULL)
    , exitCode(EXIT_SUCCESS)
{
    //! load resources
    Resources::LoadFiles("sd:/wiiu/apps/spacegame/media");

    //! create bgMusic
    bgMusic = new GuiSound(Resources::GetFile("spacegame.mp3"), Resources::GetFileSize("spacegame.mp3"));
    bgMusic->SetLoop(true);
    bgMusic->SetVolume(50);
    bgMusic->Play();

	exitApplication = false;
}

Application::~Application()
{
    delete bgMusic;
    
	AsyncDeleter::destroyInstance();

    Resources::Clear();

	SoundHandler::DestroyInstance();
    
    ProcUIInit(OSSavesDone_ReadyToRelease);
}

void Application::playBGM()
{
    bgMusic->SetVolume(50);
}

void Application::stopBGM()
{
	bgMusic->SetVolume(0);
}

int Application::exec()
{
    //! start main GX2 thread
    resumeThread();
    //! now wait for thread to finish
	shutdownThread();

	return exitCode;
}

void Application::executeThread(void)
{
	/****************************>             Globals             <****************************/
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
	
	// set the starting time
	mySpaceGlobals.seed = OSGetTime();
	
	/****************************>            VPAD Loop            <****************************/
	VPADReadError error;
	VPADStatus vpad_data;
	
	// decompress compressed things into their arrays, final argument is the transparent color in their palette
	decompress_sprite(3061, 200, 100, compressed_title, mySpaceGlobals.title, 39);
	decompress_sprite(511, 36, 36, compressed_ship, mySpaceGlobals.orig_ship, 14);
	decompress_sprite(206, 23, 23, compressed_enemy, mySpaceGlobals.enemy, 9);
	
	// setup palette and transparent index
	mySpaceGlobals.curPalette = ship_palette;
	mySpaceGlobals.transIndex = 14;
    
    mySpaceGlobals.passwordEntered = 0;
	
	// initialize starfield for this game
	initStars(&mySpaceGlobals);
	
	mySpaceGlobals.invalid = 1;
		
	while(procUI())
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
		//To exit the game
		if (mySpaceGlobals.button&VPAD_BUTTON_HOME)
		{
			break;
		}

        AsyncDeleter::triggerDeleteProcess();
	}
}
