#include "program.h"
#include "trigmath.h"

void _entryPoint()
{
	/****************************>           Get Handles           <****************************/
	//Get a handle to coreinit.rpl
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	//Get a handle to vpad.rpl */
	unsigned int vpad_handle;
	OSDynLoad_Acquire("vpad.rpl", &vpad_handle);
	/****************************>       External Prototypes       <****************************/
	//VPAD functions
	int(*VPADRead)(int controller, VPADData *buffer, unsigned int num, int *error);
	//OS functions
	void(*_Exit)();
	/****************************>             Exports             <****************************/
	//VPAD functions
	OSDynLoad_FindExport(vpad_handle, 0, "VPADRead", &VPADRead);
	//OS functions
	OSDynLoad_FindExport(coreinit_handle, 0, "_Exit", &_Exit);
	/****************************>             Globals             <****************************/
	struct SpaceGlobals mySpaceGlobals;
	//Flag for restarting the entire game.
	mySpaceGlobals.restart = 1;
	//scale of game
	mySpaceGlobals.scale=1;
	//Default locations for paddles and ball location and movement dx/dy
	mySpaceGlobals.p1X_default=40*mySpaceGlobals.scale;
	mySpaceGlobals.p1Y_default=150*mySpaceGlobals.scale;
	//Sizes of objects
	mySpaceGlobals.p1X_size=20*mySpaceGlobals.scale;
	mySpaceGlobals.p1Y_size=60*mySpaceGlobals.scale;
	//Boundry of play area (screen)
	mySpaceGlobals.xMinBoundry=0*mySpaceGlobals.scale;
	mySpaceGlobals.xMaxBoundry=400*mySpaceGlobals.scale;
	mySpaceGlobals.yMinBoundry=0*mySpaceGlobals.scale;
	mySpaceGlobals.yMaxBoundry=240*mySpaceGlobals.scale;
	
	//Game engine globals
	mySpaceGlobals.direction = 1;
	mySpaceGlobals.button = 0;

	mySpaceGlobals.speed = 1;
	mySpaceGlobals.count = 0;

	//Game engine globals
	mySpaceGlobals.direction = 1;
	mySpaceGlobals.button = 0;
	mySpaceGlobals.angle = 0;

	//Used for collision
	mySpaceGlobals.flag = 0;

	//Flag to determine if p1 should be rendered along with p1's movement direction
	mySpaceGlobals.renderP1Flag = 0;
	//Flags for render states
	mySpaceGlobals.renderResetFlag = 0;
	
	// set the starting time
	int64_t (*OSGetTime)();
    OSDynLoad_FindExport(coreinit_handle, 0, "OSGetTime", &OSGetTime);
	mySpaceGlobals.seed = OSGetTime();
	
	/****************************>            VPAD Loop            <****************************/
	int error;
	VPADData vpad_data;
	VPADTPData vpadtp_data;
	while (1)
	{
		VPADRead(0, &vpad_data, 1, &error);
		
		//Get the status of the gamepad
		mySpaceGlobals.button = vpad_data.btn_hold;
		//If the game has been restarted, reset the game (we do this one time in the beginning to set everything up)
		
		mySpaceGlobals.rstick = vpad_data.rstick;
		mySpaceGlobals.lstick = vpad_data.lstick;
		
		mySpaceGlobals.touched = vpad_data.tpdata.touched;
		mySpaceGlobals.touchX = vpad_data.tpdata.x;
		mySpaceGlobals.touchY = vpad_data.tpdata.y;
		
		if (mySpaceGlobals.restart == 1)
		{
			reset(&mySpaceGlobals);
			mySpaceGlobals.restart = 0;
		}		
		

		//Update location of player1 and 2 paddles
		p1Move(&mySpaceGlobals);
		
		// perform any shooting
		p1Shoot(&mySpaceGlobals);

		//Render the scene
		render(&mySpaceGlobals);

		//Increment the counter (used for physicals calcuations)
		mySpaceGlobals.count+=1;

		//To exit the game
		if (mySpaceGlobals.button&BUTTON_HOME)
		{
			break;
		}
	}
	//WARNING: DO NOT CHANGE THIS. YOU MUST CLEAR THE FRAMEBUFFERS AND IMMEDIATELY CALL EXIT FROM THIS FUNCTION. RETURNING TO LOADER CAUSES FREEZE.
	int ii=0;
	for(ii;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	_Exit();
}