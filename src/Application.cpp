#include <coreinit/core.h>
#include <coreinit/foreground.h>
#include <coreinit/screen.h>
#include <coreinit/time.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include <vpad/input.h>
#include "program.h"
#include "Application.h"
#include "common/common.h"
#include "resources/Resources.h"
#include "sounds/SoundHandler.hpp"
#include "system/memory.h"
#include "utils/logger.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"

#include <coreinit/filesystem.h>
#include <coreinit/title.h>
#include "fs/sd_fat_devoptab.h"
#include "fs/fs_utils.h"

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;

Application::Application()
	: CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	, bgMusic(NULL)
	, exitCode(EXIT_SUCCESS)
{
	log_print("[Main Thread Init] Loading resources from SD Card...\n");
	//! load resources
	Resources::LoadFiles("sd:/wiiu/apps/spacegame/media");

	log_print("[Main Thread Init] Creating sound thread...\n");
	//! create bgMusic
	bgMusic = new GameSound(Resources::GetFile("spacegame.mp3"), Resources::GetFileSize("spacegame.mp3"));
	bgMusic->SetLoop(true);
	bgMusic->SetVolume(50);
	bgMusic->Play();

	log_print("[Main thread init] Determining if we are running under HBL...\n");
	if(OSGetTitleID() != 0x000500101004A000 && OSGetTitleID() != 0x000500101004A100 && OSGetTitleID() != 0x000500101004A200 && OSGetTitleID() != 0x0005000013374842)
		useProcUI = true;
	else
		useProcUI = false;
	
	if(useProcUI) {
		log_print("[Main Thread Init] Not running in HBL. Initializing ProcUI...\n");
		exitApplication = false;
		ProcUIInit(OSSavesDone_ReadyToRelease);
	}
}

Application::~Application()
{
	// This hangs the system for some unknown reason.
	// Commenting it out does not do anything feature-wise, because
	// deleting bgMusic removes the one and only decoder registered in
	// the sound thread, but SoundHandler::DestroyInstance() below does
	// exactly the same thing, clears all the registered decoders.
	// I'm no expert on C++, but this may or may not cause issues down
	// the line, but its the only way I could find to allow the program
	// to exit correctly. -CreeperMario.
	//log_print("[Main Thread Deinit] Unloading background music...\n");
	//delete bgMusic;

	log_print("[Main Thread Deinit] Stopping AsyncDeleter...\n");
	AsyncDeleter::destroyInstance();

	log_print("[Main Thread Deinit] Unloading resources...\n");
	Resources::Clear();

	log_print("[Main Thread Deinit] Stopping sound thread...\n");
	SoundHandler::DestroyInstance();
	
	if(useProcUI) {
		log_print("[Main Thread Deinit] Stopping ProcUI...\n");
		ProcUIShutdown();
	}
}

void Application::screenInit(void)
{
	log_print("[OSScreen] Initializing...\n");
	//Call the Screen initilzation function.
	OSScreenInit();
	
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
	int buf1_size = OSScreenGetBufferSizeEx(SCREEN_DRC);
	//Set the buffer area.
	screenBuffer = MEM1_alloc(buf0_size + buf1_size, 0x100);
	
	// Normally, we would not typecast this to an int*, but if we don't,
	// the compiler will complain "use of void* in arithmetic."
	// Functionality-wise, this does nothing. -CreeperMario
	OSScreenSetBufferEx(SCREEN_TV, (int*)screenBuffer);
	OSScreenSetBufferEx(SCREEN_DRC, (int*)screenBuffer + buf0_size);
	
	OSScreenEnableEx(SCREEN_TV, 1);
	OSScreenEnableEx(SCREEN_DRC, 1);
	
	cleanSlate();
}

void Application::screenDeinit(void)
{
	log_print("[OSScreen] Deinitializing...\n");
	cleanSlate();
	MEM1_free(screenBuffer);
}

bool Application::AppRunning(void)
{
	if(useProcUI) {
		switch(ProcUIProcessMessages(true))
		{
			case PROCUI_STATUS_EXITING:
				log_print("[ProcUI] Program is closing...\n");
				exitApplication = true;
				break;
			case PROCUI_STATUS_RELEASE_FOREGROUND:
				log_print("[ProcUI] Foreground lost. Halting...\n");
				if(isForegroundApp) {
					isForegroundApp = false;
					screenDeinit();
					memoryRelease();
				}
				ProcUIDrawDoneRelease();
				break;
			case PROCUI_STATUS_IN_FOREGROUND:
				if(!isForegroundApp)
				{
					log_print("[ProcUI] Foreground gained. Returning to game...\n");
					isForegroundApp = true;
					memoryInitialize();
					screenInit();
					break;
				}
			case PROCUI_STATUS_IN_BACKGROUND:
			default:
				break;
		}
		return isForegroundApp;
	}
	else {
		return true;
	}
}

void Application::playBGM()
{
	log_print("[Sound] Playing background music...\n");
	bgMusic->SetVolume(50);
}

void Application::stopBGM()
{
	log_print("[Sound] Stopping background music...\n");
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
	log_print("[Main Thread] Main function running...\n");
	screenInit();
	isForegroundApp = true;
	/****************************>			 Globals			 <****************************/
	struct SpaceGlobals mySpaceGlobals = {};
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

	/****************************>			VPAD Loop			<****************************/
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
	
	// Before we begin, get the soundtrack's name and artist.
	mySpaceGlobals.trackName = bgMusic->getTrackName();
	mySpaceGlobals.artistName = bgMusic->getArtistName();

	log_print("[Main Thread] Starting game loop...");
	while(!exitApplication)
	{
		if(AppRunning() == false)
		{
			log_print("[Main Thread] Not in foreground. Skipping iteration...\n");
			mySpaceGlobals.invalid = true;
			continue;
		}
		
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
		if(!useProcUI) {
			if (mySpaceGlobals.button&VPAD_BUTTON_HOME)
			{
				exitApplication = true;
			}
		}

		AsyncDeleter::triggerDeleteProcess();
	}
	log_print("[Main Thread] Game loop finished. Beginning deinitialization...\n");
	if(!useProcUI) screenDeinit();
}
