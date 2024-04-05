#include <switch.h>

#include "switch_input.h"

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

void PADRead(struct PADData* data)
{
	// reset buttons
	data->btns_h = 0b00000000;

	data->lstick_x = 0;
	data->lstick_y = 0;
	data->rstick_x = 0;
	data->rstick_y = 0;

    padUpdate(&pad);

    // update pushed buttons
    data->btns_d = padGetButtonsDown(&pad);
    data->btns_h = padGetButtons(&pad);

    // update sticks
    HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);
    HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);
    data->lstick_x = analog_stick_l.x;
    data->lstick_y = analog_stick_l.y;
    data->rstick_x = analog_stick_r.x;
    data->rstick_y = analog_stick_r.y;

    // reset touched flag
    data->touched = 0;

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
                data->touched = 1; // mark as touched
                data->touched_y = state.touches[i].y;
                data->touched_x = state.touches[i].x;
            }
        }
    }
}