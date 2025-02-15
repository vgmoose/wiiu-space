#include "program.h"
#include "Application.h"
#include "common/common.h"
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

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;

Application::Application()
	: CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	, bgMusic(NULL)
    , exitCode(EXIT_SUCCESS)
{
    //! load resources
    Resources::LoadFiles("sd:/wiiu/apps/spacegame/media");

    //! create bgMusic
    bgMusic = new GameSound(Resources::GetFile("spacegame.mp3"), Resources::GetFileSize("spacegame.mp3"));
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
	// Not necessary - updated dynamic_libs fix this
	/*unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	int64_t (*OSGetTime)();
	OSDynLoad_FindExport(coreinit_handle, 0, "OSGetTime", &OSGetTime);*/
	mySpaceGlobals.seed = OSGetTime();
	
	/****************************>            VPAD Loop            <****************************/
	int error;
	VPADData vpad_data;
	
	KPADData pad_one_data;
	KPADData pad_two_data;
	KPADData pad_three_data;
	KPADData pad_four_data;
	
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
	
	// Before the game starts, we will ask the user which controller they will use.
	drawControllerSelectScreen(&mySpaceGlobals);
	
	while(1) {
		VPADRead(0, &vpad_data, 1, &error);
		KPADRead(0, &pad_one_data, 1);
		KPADRead(1, &pad_two_data, 1);
		KPADRead(2, &pad_three_data, 1);
		KPADRead(3, &pad_four_data, 1);
		if(error == 0 && (vpad_data.btns_r & VPAD_BUTTON_A)) {
			mySpaceGlobals.selectedController = 0;
			break;
		}
		if(WPADProbe(0, NULL) == 0 && pad_one_data.device_type == 31) if(pad_one_data.pro.btns_r & WPAD_PRO_BUTTON_A) {
			mySpaceGlobals.selectedController = 1;
			break;
		}
		if(WPADProbe(1, NULL) == 0 && pad_two_data.device_type == 31) if(pad_two_data.pro.btns_r & WPAD_PRO_BUTTON_A) {
			mySpaceGlobals.selectedController = 2;
			break;
		}
		if(WPADProbe(2, NULL) == 0 && pad_three_data.device_type == 31) if(pad_three_data.pro.btns_r & WPAD_PRO_BUTTON_A) {
			mySpaceGlobals.selectedController = 3;
			break;
		}
		if(WPADProbe(3, NULL) == 0 && pad_four_data.device_type == 31) if(pad_four_data.pro.btns_r & WPAD_PRO_BUTTON_A) {
			mySpaceGlobals.selectedController = 4;
			break;
		}
	}
		
	while(!exitApplication)
	{
		switch(mySpaceGlobals.selectedController) {
			case 0:
			VPADRead(0, &vpad_data, 1, &error);
			if(error == 0) {
				mySpaceGlobals.button = vpad_data.btns_h;
				mySpaceGlobals.rstick = vpad_data.rstick;
				mySpaceGlobals.lstick = vpad_data.lstick;
				mySpaceGlobals.touched = vpad_data.tpdata.touched;
				if (mySpaceGlobals.touched == 1) {
					mySpaceGlobals.touchX = ((vpad_data.tpdata.x / 9) - 11);
					mySpaceGlobals.touchY = ((3930 - vpad_data.tpdata.y) / 16);
				}
			} else {
				mySpaceGlobals.button = 0;
				mySpaceGlobals.rstick.x = 0;
				mySpaceGlobals.rstick.y = 0;
				mySpaceGlobals.lstick.x = 0;
				mySpaceGlobals.lstick.y = 0;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			}
			break;
			case 1:
			KPADRead(0, &pad_one_data, 1);
			if(WPADProbe(0, NULL) == 0 && pad_one_data.device_type == 31) {
				mySpaceGlobals.button = pad_one_data.pro.btns_h;
				mySpaceGlobals.rstick.x = pad_one_data.pro.rstick_x;
				mySpaceGlobals.rstick.y = pad_one_data.pro.rstick_y;
				mySpaceGlobals.lstick.x = pad_one_data.pro.lstick_x;
				mySpaceGlobals.lstick.y = pad_one_data.pro.lstick_y;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			} else {
				mySpaceGlobals.button = 0;
				mySpaceGlobals.rstick.x = 0;
				mySpaceGlobals.rstick.y = 0;
				mySpaceGlobals.lstick.x = 0;
				mySpaceGlobals.lstick.y = 0;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			}
			break;
			case 2:
			KPADRead(1, &pad_two_data, 1);
			if(WPADProbe(1, NULL) == 0 && pad_two_data.device_type == 31) {
				mySpaceGlobals.button = pad_two_data.pro.btns_h;
				mySpaceGlobals.rstick.x = pad_two_data.pro.rstick_x;
				mySpaceGlobals.rstick.y = pad_two_data.pro.rstick_y;
				mySpaceGlobals.lstick.x = pad_two_data.pro.lstick_x;
				mySpaceGlobals.lstick.y = pad_two_data.pro.lstick_y;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			} else {
				mySpaceGlobals.button = 0;
				mySpaceGlobals.rstick.x = 0;
				mySpaceGlobals.rstick.y = 0;
				mySpaceGlobals.lstick.x = 0;
				mySpaceGlobals.lstick.y = 0;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			}
			break;
			case 3:
			KPADRead(2, &pad_three_data, 1);
			if(WPADProbe(2, NULL) == 0 && pad_three_data.device_type == 31) {
				mySpaceGlobals.button = pad_three_data.pro.btns_h;
				mySpaceGlobals.rstick.x = pad_three_data.pro.rstick_x;
				mySpaceGlobals.rstick.y = pad_three_data.pro.rstick_y;
				mySpaceGlobals.lstick.x = pad_three_data.pro.lstick_x;
				mySpaceGlobals.lstick.y = pad_three_data.pro.lstick_y;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			} else {
				mySpaceGlobals.button = 0;
				mySpaceGlobals.rstick.x = 0;
				mySpaceGlobals.rstick.y = 0;
				mySpaceGlobals.lstick.x = 0;
				mySpaceGlobals.lstick.y = 0;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			}
			break;
			case 4:
			KPADRead(3, &pad_four_data, 1);
			if(WPADProbe(3, NULL) == 0 && pad_four_data.device_type == 31) {
				mySpaceGlobals.button = pad_four_data.pro.btns_h;
				mySpaceGlobals.rstick.x = pad_four_data.pro.rstick_x;
				mySpaceGlobals.rstick.y = pad_four_data.pro.rstick_y;
				mySpaceGlobals.lstick.x = pad_four_data.pro.lstick_x;
				mySpaceGlobals.lstick.y = pad_four_data.pro.lstick_y;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			} else {
				mySpaceGlobals.button = 0;
				mySpaceGlobals.rstick.x = 0;
				mySpaceGlobals.rstick.y = 0;
				mySpaceGlobals.lstick.x = 0;
				mySpaceGlobals.lstick.y = 0;
				mySpaceGlobals.touched = 0;
				mySpaceGlobals.touchX = 0;
				mySpaceGlobals.touchY = 0;
			}
			break;
			default:
			OSFatal("Attempted to read from invalid controller.");
		}
		/*VPADRead(0, &vpad_data, 1, &error);
		
		//Get the status of the gamepad
		mySpaceGlobals.button = vpad_data.btns_h;
		
		mySpaceGlobals.rstick = vpad_data.rstick;
		mySpaceGlobals.lstick = vpad_data.lstick;
		
		mySpaceGlobals.touched = vpad_data.tpdata.touched;
		if (mySpaceGlobals.touched == 1)
		{
			mySpaceGlobals.touchX = ((vpad_data.tpdata.x / 9) - 11);
			mySpaceGlobals.touchY = ((3930 - vpad_data.tpdata.y) / 16);
		}*/
		
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
		/*if (mySpaceGlobals.button&VPAD_BUTTON_HOME)
		{
			break;
		}*/
		
		if(mySpaceGlobals.selectedController == 0 && (mySpaceGlobals.button & VPAD_BUTTON_HOME)) break;
		if(mySpaceGlobals.selectedController >=1 && mySpaceGlobals.selectedController <= 4 && (mySpaceGlobals.button & WPAD_PRO_BUTTON_HOME)) break;

        AsyncDeleter::triggerDeleteProcess();
	}
}
