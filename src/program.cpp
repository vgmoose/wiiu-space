#include <coreinit/title.h>
#include <nsysnet/socket.h>
#include <sysapp/launch.h>

#include "Application.h"
#include "draw.h"
#include "system/exception_handler.h"
#include "system/memory.h"
#include "utils/logger.h"

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

extern "C" bool isRunningInHBL()
{
	// Mii Maker JPN || Mii Maker USA || Mii Maker PAL || Homebrew Launcher
	if(OSGetTitleID() == 0x000500101004A000 || OSGetTitleID() == 0x000500101004A100 || OSGetTitleID() == 0x000500101004A200 || OSGetTitleID() == 0x0005000013374842)
	{
		return true;
	}
	return false;
}

/* Entry point */
extern "C" int main(int argc, char **argv)
{
	socket_lib_init();
	log_init("192.168.0.101");
	//log_print(".pylogu.command.startProgram Space Game");
	log_print("Welcome to Space Game!\n");
	
	log_print("Installing exception handler...\n");
	setup_os_exceptions();
	
	log_print("Launching main thread...\n");
	int returnCode = Application::instance()->exec();
	
	log_print("Main thread exited. Cleaning up after it...\n");
	Application::destroyInstance();

	log_print("Thanks for playing! See ya later.\n");
	//log_print(".pylogu.command.endProgram");
	log_deinit();

	// This is required because HBL will not automatically relaunch after an RPX is finished executing.
	if(isRunningInHBL())
		SYSRelaunchTitle(0,0);

	return returnCode;
}
