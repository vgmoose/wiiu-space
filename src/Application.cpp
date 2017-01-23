#include <coreinit/dynload.h>
#include <coreinit/foreground.h>
#include <coreinit/screen.h>
#include <coreinit/time.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>

#include "Application.h"
#include "images.h"
#include "program.h"
#include "resources/Resources.h"
#include "sounds/SoundHandler.hpp"
#include "space.h"
#include "system/memory.h"
#include "utils/logger.h"

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;

Application::Application()
	: CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	, bgMusic(NULL)
	, exitCode(EXIT_SUCCESS)
{
	//! load resources
	// external01 is the SD Card, and is used under all versions of the program
	// content is provided by Cafe OS, and is only used under the dedicated Wii U Menu version
	// Files on the SD card take precedence over those in content,
	// if a file is present on the SD card, it won't be loaded from content,
	// and if it isn't on the SD card, it will be checked for in content.
	log_print("[Main Thread Init] Loading resources from SD card...\n");
	Resources::LoadFiles("fs:/vol/external01/wiiu/apps/spacegame/media");
	if(!isRunningInHBL()) {
		log_print("[Main Thread Init] Not running in HBL. Loading resources from NAND...\n");
		Resources::LoadFiles("fs:/vol/content");
	}

	log_print("[Main Thread Init] Creating background music thread...\n");
	//! create bgMusic
	bgMusic = new GameSound(Resources::GetFile("spacegame.mp3"), Resources::GetFileSize("spacegame.mp3"));
	bgMusic->fetchMetadata();
	bgMusic->SetLoop(true);
	bgMusic->SetVolume(50);
	bgMusic->Play();
	
	//! Create sound effects
	log_print("[Main Thread Init] Creating \"Pew\" sound effect thread...\n");
	pewSound = new GameSound(Resources::GetFile("pew.wav"), Resources::GetFileSize("pew.wav"));
	pewSound->SetLoop(false);
	pewSound->SetVolume(15);
	
	log_print("[Main Thread Init] Creating \"Boom\" sound effect thread...\n");
	boomSound = new GameSound(Resources::GetFile("boom.wav"), Resources::GetFileSize("boom.wav"));
	boomSound->SetLoop(false);
	boomSound->SetVolume(15);

	if(isRunningInHBL())
		useProcUI = false;
	else
		useProcUI = true;
	
	if(useProcUI) {
		log_print("[Main Thread Init] Not running in HBL. Initializing ProcUI...\n");
		exitApplication = false;
		ProcUIInit(OSSavesDone_ReadyToRelease);
	}
}

Application::~Application()
{
	log_print("[Main Thread Deinit] Unloading sound threads...\n");
	delete bgMusic;
	delete pewSound;
	delete boomSound;

	log_print("[Main Thread Deinit] Stopping AsyncDeleter...\n");
	AsyncDeleter::destroyInstance();

	log_print("[Main Thread Deinit] Unloading resources...\n");
	Resources::Clear();

	log_print("[Main Thread Deinit] Stopping sound handler thread...\n");
	SoundHandler::DestroyInstance();
	
	if(useProcUI) {
		log_print("[Main Thread Deinit] Stopping ProcUI...\n");
		ProcUIShutdown();
	}
}

void Application::screenInit(void)
{
	log_print("[OSScreen] Setting up MEM1 heap...\n");
	memoryInitialize();
	
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
	
	log_print("[OSScreen] Freeing MEM1 heap...\n");
	memoryRelease();
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
	log_print("[Main Thread] Initializing GamePad...\n");
	VPADInit();
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
	
	// Mount the game's save data directories. (Only if launched from the Wii U Menu)
	//if(!isRunningInHBL()) MountSaveData();

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
			if(mySpaceGlobals.shootSoundEffect) {
				if(!pewSound->IsPlaying()) pewSound->Play();
				mySpaceGlobals.shootSoundEffect = false;
			}

			// handle any collisions
			handleCollisions(&mySpaceGlobals);

			// do explosions
			handleExplosions(&mySpaceGlobals);
			if(mySpaceGlobals.explosionSoundEffect) {
				boomSound->Stop();
				boomSound->Play();
				mySpaceGlobals.explosionSoundEffect = false;
			}

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
