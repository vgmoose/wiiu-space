// These files implement WiiU-style wrappers for reading controller/touch data on the Switch
#ifndef SWITCH_PLATFORM_H
#define SWITCH_PLATFORM_H
unsigned int OSGetTime();

#define __os_snprintf snprintf
#define OSFatal printf
#endif