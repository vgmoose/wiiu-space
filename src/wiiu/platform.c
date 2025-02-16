#include "../platform.h"
#include "../program.h"

#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <coreinit/screen.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <vpad/input.h>

#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>

#include "../draw.h"


void SaveCallback()
{
   OSSavesDone_ReadyToRelease(); // Required
}

void platformInit()
{
    WHBLogCafeInit();
	WHBLogUdpInit();

	OSDynLoad_Module avm_handle = 0;
	OSDynLoad_Acquire("avm.rpl", &avm_handle);
	bool(*AVMSetTVScale)(int width, int height);
	OSDynLoad_FindExport(avm_handle, 0, "AVMSetTVScale", (void **)&AVMSetTVScale);
	AVMSetTVScale(854, 480);  // Not working, hope to find a solution

	OSScreenInit();
	ProcUIInit(&SaveCallback);
}

bool initialized = false;
bool isAppRunning = true;

bool AppRunning(struct SpaceGlobals* mySpaceGlobals)
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
          }
          ProcUIShutdown();
      }
      else if(status == PROCUI_STATUS_RELEASE_FOREGROUND)
      {
          // Free up MEM1 to next foreground app, etc.
          initialized = false;

          screenDeinit();
          ProcUIDrawDoneRelease();
      }
      else if(status == PROCUI_STATUS_IN_FOREGROUND)
      {
         // Reallocate MEM1, reinit screen, etc.
         if(!initialized)
         {
            initialized = true;
            screenInit();

            // redraw the screen upon resume
            mySpaceGlobals->invalid = 1;
         }
      }
   }

   return isAppRunning;
}
