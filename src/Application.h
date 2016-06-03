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
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "system/CThread.h"
#include "game/GameSound.h"
#include "trigmath.h"

class Application : public CThread
{
public:
    static Application * instance() {
        if(!applicationInstance)
            applicationInstance = new Application();
        return applicationInstance;
    }
    static void destroyInstance() {
        if(applicationInstance) {
            delete applicationInstance;
            applicationInstance = NULL;
        }
    }

    GameSound *getBgMusic(void) const {
        return bgMusic;
    }
	
	void playBGM(void);
	void stopBGM(void);

    int exec(void);

    void quit(int code) {
        exitCode = code;
        exitApplication = true;
    }

private:
    Application();
    virtual ~Application();

    static Application *applicationInstance;
    static bool exitApplication;

    void executeThread(void);

    GameSound *bgMusic;
    int exitCode;
};

#endif //_APPLICATION_H
