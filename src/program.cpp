#include "program.h"
#include "trigmath.h"
#include "draw.h"
#include "images.h"
#include "space.h"

#include "Application.h"
#include "dynamic_libs/padscore_functions.h"
#include "dynamic_libs/ax_functions.h"
#include "fs/fs_utils.h"
#include "system/memory.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "common/common.h"
#include <coreinit/screen.h>

/* Entry point */
extern "C" int _entryPoint()
{	
    int returnCode = Application::instance()->exec();

    Application::destroyInstance();

	return 0;
}