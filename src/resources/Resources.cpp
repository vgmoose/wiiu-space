#include <gctypes.h>
#include <malloc.h>
#include <string.h>
#include "Resources.h"
#include "filelist.h"
#include "system/AsyncDeleter.h"
#include "fs/fs_utils.h"
#include "game/GameSound.h"

Resources * Resources::instance = NULL;

void Resources::Clear()
{
	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(RecourceList[i].CustomFile)
		{
			free(RecourceList[i].CustomFile);
			RecourceList[i].CustomFile = NULL;
		}

		if(RecourceList[i].CustomFileSize != 0)
			RecourceList[i].CustomFileSize = 0;
	}

	if(instance)
        delete instance;

    instance = NULL;
}

bool Resources::LoadFiles(const char * path)
{
	if(!path)
		return false;

	bool result = false;
	Clear();

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
        std::string fullpath(path);
        fullpath += "/";
        fullpath += RecourceList[i].filename;

        u8 * buffer = NULL;
        u32 filesize = 0;

        LoadFileToMem(fullpath.c_str(), &buffer, &filesize);

        RecourceList[i].CustomFile = buffer;
        RecourceList[i].CustomFileSize = (u32) filesize;
        result |= (buffer != 0);
	}

	return result;
}

const u8 * Resources::GetFile(const char * filename)
{
	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			return (RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile);
		}
	}

	return NULL;
}

u32 Resources::GetFileSize(const char * filename)
{
	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			return (RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize);
		}
	}
	return 0;
}

GameSound * Resources::GetSound(const char * filename)
{
    if(!instance)
        instance = new Resources;

    std::map<std::string, std::pair<unsigned int, GameSound *> >::iterator itr = instance->soundDataMap.find(std::string(filename));
    if(itr != instance->soundDataMap.end())
    {
        itr->second.first++;
        return itr->second.second;
    }

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			const u8 * buff = RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile;
			const u32 size = RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize;

			if(buff == NULL)
                return NULL;

            GameSound * sound = new GameSound(buff, size);
            instance->soundDataMap[std::string(filename)].first = 1;
            instance->soundDataMap[std::string(filename)].second = sound;

            return sound;
		}
	}

	return NULL;
}

void Resources::RemoveSound(GameSound * sound)
{
    std::map<std::string, std::pair<unsigned int, GameSound *> >::iterator itr;

    for(itr = instance->soundDataMap.begin(); itr != instance->soundDataMap.end(); itr++)
    {
        if(itr->second.second == sound)
        {
            itr->second.first--;

            if(itr->second.first == 0)
            {
                AsyncDeleter::pushForDelete( itr->second.second );
                instance->soundDataMap.erase(itr);
            }
            break;
        }
    }
}
