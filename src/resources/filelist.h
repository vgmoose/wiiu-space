/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes 3 files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

typedef struct _RecourceFile
{
	const char *filename;
	const unsigned char   *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char		   *CustomFile;
	unsigned int		    CustomFileSize;
} RecourceFile;

extern const unsigned char boom_wav[];
extern const unsigned int boom_wav_size;

extern const unsigned char pew_wav[];
extern const unsigned int pew_wav_size;

extern const unsigned char spacegame_mp3[];
extern const unsigned int spacegame_mp3_size;

static RecourceFile RecourceList[] =
{
	{"boom.wav", boom_wav, boom_wav_size, NULL, 0},
	{"pew.wav", pew_wav, pew_wav_size, NULL, 0},
	{"spacegame.mp3", spacegame_mp3, spacegame_mp3_size, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
