#include <switch.h>

#include "platform.h"

unsigned int OSGetTime()
{
    return svcGetSystemTick();
}
void platformInit() {

}
bool AppRunning(struct SpaceGlobals* mySpaceGlobals)
{
    // TODO: allow exiting
    return true;
}