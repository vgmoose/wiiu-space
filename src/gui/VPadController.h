/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef VPAD_CONTROLLER_H_
#define VPAD_CONTROLLER_H_

#include <vpad/input.h>
#include "GuiController.h"

class VPadController : public GuiController
{
public:
    //!Constructor
    VPadController(int channel)
        : GuiController(channel)
    {
        memset(&vpad, 0, sizeof(vpad));
    }

    //!Destructor
    virtual ~VPadController()  {}

    bool update(int width, int height)
    {
        lastData = data;

        VPADReadError vpadError = VPAD_READ_NO_SAMPLES;
        VPADRead(0, &vpad, 1, &vpadError);

        if(vpadError == VPAD_READ_SUCCESS)
        {
            data.buttons_r = vpad.release;
            data.buttons_h = vpad.hold;
            data.buttons_d = vpad.trigger;
            data.validPointer = !vpad.tpNormal.validity;
            data.touched = vpad.tpNormal.touched;
            //! calculate the screen offsets
            data.x = -(width >> 1) + (int)((vpad.tpFiltered1.x * width) >> 12);
            data.y = (height >> 1) - (int)(height - ((vpad.tpFiltered1.y * height) >> 12));
            return true;
        }
        return false;
    }

private:
    VPADStatus vpad;
};

#endif
