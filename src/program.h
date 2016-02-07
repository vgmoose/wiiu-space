#ifndef PROGRAM_H
#define PROGRAM_H

#include "../../../libwiiu/src/coreinit.h"
#include "../../../libwiiu/src/vpad.h"
#include "../../../libwiiu/src/types.h"
//Using modified version of draw to render at twice the scale to improve framerate

#include "space.h"
struct Services
{
	unsigned int (*OSScreenPutPixelEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, uint32_t color);
	unsigned int coreinit_handle;
	void(*DCFlushRange)(void *buffer, uint32_t length);
	unsigned int(*OSScreenFlipBuffersEx)(unsigned int bufferNum);
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenClearBufferEx)(unsigned int bufferNum, unsigned int temp);
	unsigned int(*OSScreenPutFontEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, void * buffer);
};
void _entryPoint();

#endif /* PROGRAM_H */