#include <3ds.h>

#define TOP_SCREEN_X 400
#define TOP_SCREEN_Y 240

#define BOTTOM_SCREEN_X 320
#define BOTTOM_SCREEN_Y 240

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void reset(void);
void drawPaddle(int x, int y, u8 ai);
void drawBall(int x, int y);
void drawStuff(void);
bool TakeScreenshot(char* path);

u8* screenTopLeft;
u8* screenTopRight;
u8* screenBottom;

FS_archive sdmcArchive;

u32 kDown;
u32 kHeld;
int paddle1X = 5;
int paddle1Y = (TOP_SCREEN_Y/2) - 25;
int paddle2X = TOP_SCREEN_X-20;
int paddle2Y = (TOP_SCREEN_Y/2) - 25;
int paddle1Vel = 4;

int paddle1Score = 0;
int paddle2Score = 0;

float ballX;
float ballY ;
float ballVelX ;
float ballVelY;

u8 first = 1;
u8 runAi = 0;
u8 paddle1R = 0xFF;
u8 paddle1G = 0xFF;
u8 paddle1B = 0xFF;

int screenWait = 60*2;
u8 count = 0;

int depthSeparation = 2;
#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)
