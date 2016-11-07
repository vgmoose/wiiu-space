#include <coreinit/thread.h>


#define BUS_SPEED                       248625000
#define SECS_TO_TICKS(sec)              (((unsigned long long)(sec)) * (BUS_SPEED/4))
#define MILLISECS_TO_TICKS(msec)        (SECS_TO_TICKS(msec) / 1000)
#define MICROSECS_TO_TICKS(usec)        (SECS_TO_TICKS(usec) / 1000000)

void usleep(unsigned int microsecs)
{
    OSSleepTicks(MICROSECS_TO_TICKS(microsecs));
}

void sleep(unsigned int secs)
{
    OSSleepTicks(SECS_TO_TICKS(secs));
}
