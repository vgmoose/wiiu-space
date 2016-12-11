#include <coreinit/filesystem.h>
#include <coreinit/screen.h>
#include <sysapp/launch.h>
#include <vpad/input.h>

#include "program.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"

#include "Application.h"
#include "fs/fs_utils.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "common/common.h"

extern "C" void cleanSlate()
{
	// clear both buffers
	int ii;
	for(ii=0;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
}

/* Entry point */
extern "C" int main(int argc, char **argv)
{
	memoryInitialize();

  mount_sd_fat("sd");

  VPADInit();

	//Call the Screen initilzation function.
	OSScreenInit();

	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
	int buf1_size = OSScreenGetBufferSizeEx(SCREEN_DRC);
	//Set the buffer area.
  void * screenBuffer = MEM1_alloc(buf0_size + buf1_size, 0x100);
	OSScreenSetBufferEx(SCREEN_TV, screenBuffer);
	OSScreenSetBufferEx(SCREEN_DRC, screenBuffer + buf0_size);

	OSScreenEnableEx(SCREEN_TV, 1);
  OSScreenEnableEx(SCREEN_DRC, 1);

	cleanSlate();

  int returnCode = Application::instance()->exec();

  Application::destroyInstance();

	cleanSlate();

  unmount_sd_fat("sd");

  memoryRelease();

  // This is required because HBL will not automatically relaunch after an RPX is finished executing.
  // A check should be added so that this only happens underneath Mii Maker/HBL titles, because we do not
  // want to reload the game if it is running under its own title ID, that will keep you trapped in an
  // infinite loop of the program finishing and starting.
  // Also TODO: Add support for ProcUI and move this to the AppRunning function.
  SYSRelaunchTitle(0,0);

	return returnCode;
}
