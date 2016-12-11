#ifndef RECOURCES_H_
#define RECOURCES_H_


#include <map>
#include <common/types.h>

//! forward declaration
class GameSound;

class Resources
{
public:
    static void Clear();
    static bool LoadFiles(const char * path);
    static const u8 * GetFile(const char * filename);
    static u32 GetFileSize(const char * filename);

    static GameSound * GetSound(const char * filename);
    static void RemoveSound(GameSound * sound);
private:
    static Resources *instance;

    Resources() {}
    ~Resources() {}

    std::map<std::string, std::pair<unsigned int, GameSound *> > soundDataMap;
};

#endif
