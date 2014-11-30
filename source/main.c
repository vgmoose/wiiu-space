#include "main.h"

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "draw.h"
#include "mem.h"

int main()
{
	// Initialize services
	srvInit();			// mandatory
	aptInit();			// mandatory
	hidInit(NULL);	// input (buttons, screen)
	gfxInit();			// graphics
	fsInit();
	sdmcArchive = (FS_archive){0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};
	FSUSER_OpenArchive(NULL, &sdmcArchive);

	gfxSet3D(true);

	gspWaitForVBlank(); //wait to let the app register itself
	
	ballX += 200;
	ballY += 120;
	ballVelX += 0;
	ballVelY -= 0;

	// Main loop
	while (aptMainLoop())
	{
		// Wait next screen refresh
		gspWaitForVBlank();

		// Read which buttons are currently pressed 
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		
		// If START is pressed, break loop and quit
		if (kDown & KEY_START){
			break;
		}

		drawStuff();

		
	}

	// Exit services
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	
	// Return to hbmenu
	return 0;
}

void drawStuff(void)
{
	//Init screen buffers
	screenTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL); 
	screenTopRight = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
	screenBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL); 
	/* Clear Screen */
	clearScreen(screenBottom, GFX_BOTTOM);
	clearScreen(screenTopLeft, GFX_TOP); 
	clearScreen(screenTopRight, GFX_TOP);

	char str[4];
	sprintf(str, "%i", paddle1Score);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, str, BOTTOM_HEIGHT - 5 - 8 - 3, 3);

	char str2[4];
	sprintf(str2, "%i", paddle2Score);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, str2, BOTTOM_HEIGHT - 5 - 8 - 3, BOTTOM_WIDTH - (6*strlen(str2)) - 3);
	if(ballVelX == 0 && first)
	{
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "Press A to Serve Ball", 140, 150);
		gfxDrawText(GFX_TOP, GFX_RIGHT, NULL, "Press A to Serve Ball", 140, 150);
	}

	//Check inputs, apply paddle velocity for P2
	if (kHeld & KEY_CPAD_UP || kHeld & KEY_UP)
		paddle1Y -= paddle1Vel;
	if (kHeld & KEY_CPAD_DOWN || kHeld & KEY_DOWN)
		paddle1Y += paddle1Vel;

	//Make sure we don't run off the screen
	if(paddle1Y > TOP_SCREEN_Y-52)
		paddle1Y = TOP_SCREEN_Y-52;
	else if(paddle1Y < 1)
		paddle1Y = 1;

	//If we are running the AI (blue paddle), set our Y to ball Y before it's min'd and max'd
	if(runAi)
	{
		paddle2Y = ballY - 25;
	}

	//Check inputs, apply paddle velocity for P2
	if (kHeld & KEY_X)
		paddle2Y -= paddle1Vel;
	if (kHeld & KEY_B)
		paddle2Y += paddle1Vel;

	//Make sure opponent (and AI) doesn't run off screen
	if(paddle2Y > TOP_SCREEN_Y-52)
		paddle2Y = TOP_SCREEN_Y-52;
	else if(paddle2Y < 1)
		paddle2Y = 1;

	//Start ball velocity
	if (kDown & KEY_A && ballVelX == 0)
	{
		ballVelX -= 2;
		ballVelY -= 2;
	}

	//Toggle AI
	if (kDown & KEY_SELECT && ballVelX == 0)
	{
		runAi = !runAi;
	}

	drawPaddle(paddle1X,paddle1Y,0);
	drawPaddle(400-20,paddle2Y,runAi);

	ballX += ballVelX;
	ballY += ballVelY;

	if(ballY < 3 || ballY > 240-3 )
	{
		ballVelY *= -1;
		ballY += ballVelY;
		first = 0;
	}

	if((ballX < paddle1X+10 && ballY > paddle1Y && ballY < paddle1Y + 50) || (ballX > paddle2X && ballY > paddle2Y && ballY < paddle2Y + 50))
	{
		ballVelX *= -1;
		ballX += ballVelX;
	}
	else if(ballX < 3)
	{
		reset();
		paddle2Score++;
	}
	else if(ballX > 400-3)
	{
		reset();
		paddle1Score++;
	}

	drawBall((int)ballX,(int)ballY);

	//Screenshot - taken from blargSnes
	if(kDown & KEY_L && kDown & KEY_R)
	{
		u32 timestamp = (u32)(svcGetSystemTick() / 446872);
		char file[256];
		snprintf(file, 256, "/3dspong%08d.bmp", timestamp);
		if(TakeScreenshot(file))
			paddle1R = 0;
		else
			paddle1G = 0;

		count = 1;
	}

	if(count)
		screenWait--;
	if(screenWait <= 0)
	{
		paddle1R = 255;
		paddle1G = 255;
		paddle1B = 255;
		count = 0;
		screenWait = 60*2;
	}

	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();
}

void reset(void)
{
	ballX = 200;
	ballY = 120;
	ballVelX = 0;
	ballVelY = 0;
}

void drawPaddle(int x, int y, u8 ai)
{
	int move = (int)(depthSeparation * CONFIG_3D_SLIDERSTATE);
	drawFillRect(1+x+move,1+y,10+x+move, 50+y, (ai ? 0 : 255), (ai ? 0 : 255), 255, screenTopLeft);
	drawFillRect(1+x-move,1+y,10+x-move, 50+y, (ai ? 0 : 255), (ai ? 0 : 255), 255, screenTopRight);
}

void drawBall(int x, int y)
{
	if(x > TOP_WIDTH)
		x = TOP_WIDTH;
	if(x < 0)
		x = 0;

	if(y > TOP_HEIGHT)
		y = TOP_HEIGHT;
	if(y < 0)
		y = 0;

	int move = (int)(depthSeparation * CONFIG_3D_SLIDERSTATE);
	drawFillCircle(x+move,y,2,paddle1R,paddle1G,paddle1B,screenTopLeft);
	drawFillCircle(x-move,y,2,paddle1R,paddle1G,paddle1B,screenTopRight);
}

//Taken from blargSnes
bool TakeScreenshot(char* path)
{
	int x, y;
	
	Handle file;
	FS_path filePath;
	filePath.type = PATH_CHAR;
	filePath.size = strlen(path) + 1;
	filePath.data = (u8*)path;
	
	Result res = FSUSER_OpenFile(NULL, &file, sdmcArchive, filePath, FS_OPEN_CREATE|FS_OPEN_WRITE, FS_ATTRIBUTE_NONE);
	if (res) 
		return false;
		
	u32 byteswritten;
	
	u32 bitmapsize = 400*480*3;
	u8* tempbuf = (u8*)MemAlloc(0x36 + bitmapsize);
	memset(tempbuf, 0, 0x36 + bitmapsize);
	
	FSFILE_SetSize(file, (u16)(0x36 + bitmapsize));
	
	*(u16*)&tempbuf[0x0] = 0x4D42;
	*(u32*)&tempbuf[0x2] = 0x36 + bitmapsize;
	*(u32*)&tempbuf[0xA] = 0x36;
	*(u32*)&tempbuf[0xE] = 0x28;
	*(u32*)&tempbuf[0x12] = 400; // width
	*(u32*)&tempbuf[0x16] = 480; // height
	*(u32*)&tempbuf[0x1A] = 0x00180001;
	*(u32*)&tempbuf[0x22] = bitmapsize;
	
	u8* framebuf = (u8*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	for (y = 0; y < 240; y++)
	{
		for (x = 0; x < 400; x++)
		{
			int si = ((239 - y) + (x * 240)) * 3;
			int di = 0x36 + (x + ((479 - y) * 400)) * 3;
			
			tempbuf[di++] = framebuf[si++];
			tempbuf[di++] = framebuf[si++];
			tempbuf[di++] = framebuf[si++];
		}
	}
	
	framebuf = (u8*)gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	for (y = 0; y < 240; y++)
	{
		for (x = 0; x < 320; x++)
		{
			int si = ((239 - y) + (x * 240)) * 3;
			int di = 0x36 + ((x+40) + ((239 - y) * 400)) * 3;
			
			tempbuf[di++] = framebuf[si++];
			tempbuf[di++] = framebuf[si++];
			tempbuf[di++] = framebuf[si++];
		}
	}
	
	FSFILE_Write(file, &byteswritten, 0, (u32*)tempbuf, 0x36 + bitmapsize, 0x10001);
	
	FSFILE_Close(file);
	MemFree(tempbuf);
	return true;
}
