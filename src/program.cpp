#include <coreinit/filesystem.h>
#include <coreinit/screen.h>
#include <nsysnet/socket.h>
#include <sysapp/launch.h>
#include <vpad/input.h>
#include <unistd.h>

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

#include <coreinit/title.h>

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
	socket_lib_init();
	log_init("192.168.0.101");
	log_print(".pylogu.command.startProgram Space Game");
	log_print("Starting Space Game...\n");
	
	log_print("Initializing MEM1...\n");
	memoryInitialize();

	log_print("Mounting SD Card...\n");
	mount_sd_fat("sd");

	log_print("Initializing GamePad...\n");
	VPADInit();
	
	log_print("Launching main thread...\n");
	int returnCode = Application::instance()->exec();
	
	log_print("Main thread exited. Cleaning up after it...\n");
	Application::destroyInstance();

	log_print("Unmounting SD Card...\n");
	unmount_sd_fat("sd");
	
	log_print("Freeing MEM1...\n");
	memoryRelease();

	log_print("Thanks for playing! See ya later.\n");
	log_print(".pylogu.command.endProgram");
	log_deinit();

	// This is required because HBL will not automatically relaunch after an RPX is finished executing.
	if(OSGetTitleID() == 0x000500101004A000 || OSGetTitleID() == 0x000500101004A000 || OSGetTitleID() == 0x000500101004A200 || OSGetTitleID() == 0x0005000013374842)
		SYSRelaunchTitle(0,0);

	return returnCode;
}
