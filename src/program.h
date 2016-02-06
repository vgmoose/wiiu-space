#ifndef PROGRAM_H
#define PROGRAM_H

#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
//Using modified version of draw to render at twice the scale to improve framerate
#include "../../../libwiiu/src/draw.h"

#include "space.h"
struct Services
{
	unsigned int (*OSScreenPutPixelEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
};
extern unsigned int (*GlobalPutPixel)(unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
void _entryPoint();

#endif /* PROGRAM_H */