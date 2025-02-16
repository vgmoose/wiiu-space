#include <switch.h>

#include "paddata.h"

PadState pad;

void PADInit()
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    padInitializeDefault(&pad);

    // init touch screen
    hidInitializeTouchScreen();

}

void PADDestroy()
{

}

void VPADRead(int chan, struct PADData *data, int num, int *error) {
	PADRead(data);
}

void PADRead(struct PADData* data)
{
	// reset buttons
	data->btns_h = 0b00000000;

	data->leftStick = (Vec2D){0, 0};
    data->rightStick = (Vec2D){0, 0};

    padUpdate(&pad);

    // update pushed buttons
    // data->btns_d = padGetButtonsDown(&pad);
    data->btns_h = padGetButtons(&pad);

    // update sticks
    HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);
    HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);
    data->leftStick = (Vec2D){analog_stick_l.x, analog_stick_l.y};
    data->rightStick = (Vec2D){analog_stick_r.x, analog_stick_r.y};

    // reset touched flag
    data->isTouched = 0;

    HidTouchScreenState state={0};
    int largest_touch = 0;
    if (hidGetTouchScreenStates(&state, 1)) {
        for(s32 i=0; i<state.count; i++)
        {
            // take the largest touch
            int curTouch = state.touches[i].diameter_x * state.touches[i].diameter_y;
            if (curTouch > largest_touch)
            {
                largest_touch = curTouch;
                data->isTouched = 1; // mark as touched
                data->touchData.x = state.touches[i].x;
                data->touchData.y = state.touches[i].y;
            }
        }
    }
}