// These files implement WiiU-style wrappers for reading controller/touch data on the Switch

#define PAD_BUTTON_LEFT		BIT(12)
#define PAD_BUTTON_RIGHT	BIT(14)
#define PAD_BUTTON_UP		BIT(13)
#define PAD_BUTTON_DOWN		BIT(15)

#define PAD_BUTTON_MINUS	BIT(11)
#define PAD_BUTTON_PLUS		BIT(10)

#define PAD_BUTTON_A		BIT(0)
#define PAD_BUTTON_B		BIT(2)

struct PADData {
	int btns_h;
	
	float rstick_x;
	float lstick_x;
	float rstick_y;
	float lstick_y;

	int touched;
	int touched_x;
	int touched_y;
};

void PADInit();
void PADDestroy();
void PADRead(struct PADData* data);