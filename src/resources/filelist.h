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

typedef struct _RecourceFile
{
	const char          *filename;
	const unsigned char *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char	    *CustomFile;
	unsigned int        CustomFileSize;
} RecourceFile;

extern const unsigned char spacegame_mp3[];
extern const unsigned int spacegame_mp3_size;

static RecourceFile RecourceList[] =
{
	{"spacegame.mp3", spacegame_mp3, spacegame_mp3_size, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
