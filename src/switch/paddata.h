// These files implement WiiU-style wrappers for reading controller/touch data on the Switch
#ifndef SWITCH_PADDATA_H
#define SWITCH_PADDATA_H

#define BIT(x) (1 << (x))

#define PAD_BUTTON_LEFT		BIT(12)
#define PAD_BUTTON_RIGHT	BIT(14)
#define PAD_BUTTON_UP		BIT(13)
#define PAD_BUTTON_DOWN		BIT(15)

#define PAD_BUTTON_MINUS	BIT(11)
#define PAD_BUTTON_PLUS		BIT(10)

#define PAD_BUTTON_A		BIT(0)
#define PAD_BUTTON_B		BIT(2)

// define these as the same as the wiiu constants
// TODO: move wiiu to use these instead
#define VPAD_BUTTON_LEFT		PAD_BUTTON_LEFT
#define VPAD_BUTTON_RIGHT		PAD_BUTTON_RIGHT
#define VPAD_BUTTON_UP			PAD_BUTTON_UP
#define VPAD_BUTTON_DOWN		PAD_BUTTON_DOWN

#define VPAD_BUTTON_MINUS		PAD_BUTTON_MINUS
#define VPAD_BUTTON_PLUS		PAD_BUTTON_PLUS

#define VPAD_BUTTON_A			PAD_BUTTON_A
#define VPAD_BUTTON_B			PAD_BUTTON_B

typedef struct Vec2D {
	float x;
	float y;
} Vec2D;

typedef struct PADData {
	int btns_h;
	
	Vec2D leftStick;
	Vec2D rightStick;

	int isTouched;
	Vec2D touchData;
} PADData;

void PADInit();
void PADDestroy();
void PADRead(struct PADData* data);

typedef PADData VPADStatus;
typedef Vec2D VPADVec2D;

// wiiu wrappers
void VPADRead(int chan, struct PADData *data, int num, int *error);

#endif