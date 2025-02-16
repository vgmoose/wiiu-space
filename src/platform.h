#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdbool.h>
#include "program.h"
#if !defined(__WIIU__)
#include "switch/platform.h"
#endif
void platformInit();
bool AppRunning(struct SpaceGlobals* mySpaceGlobals);

#endif /* PLATFORM_H */