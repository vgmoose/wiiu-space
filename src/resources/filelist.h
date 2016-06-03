/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes 1 files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

#include <gctypes.h>

typedef struct _RecourceFile
{
	const char *filename;
	const u8   *DefaultFile;
	const u32  &DefaultFileSize;
	u8		   *CustomFile;
	u32		    CustomFileSize;
} RecourceFile;

extern const u8 spacegame_mp3[];
extern const u32 spacegame_mp3_size;

static RecourceFile RecourceList[] =
{
	{"spacegame.mp3", spacegame_mp3, spacegame_mp3_size, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
