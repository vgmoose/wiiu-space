#include "music.h"

#ifdef MUSIC
#include <mpg123.h>
#include <coreinit/cache.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/thread.h>
#include <sndcore2/core.h>
#include <sndcore2/voice.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>


// based off immaterial's Music.cpp: https://github.com/glastonbridge/immaterial-wiiu-demo/blob/952a683ae05e676ea839c90a5178653616b325b1/sound/Music.cpp
// and the SDL2 port's wiiu_audio.cpp: https://github.com/GaryOderNichts/SDL/blob/wiiu-sdl2-2.26/src/audio/wiiu/SDL_wiiuaudio.c

typedef struct {
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    int16_t *bufferLeft;
    int16_t *bufferRight;
    AXVoice *voiceLeft;
    AXVoice *voiceRight;
    AXVoiceOffsets bufferInfo;
    int channels;
    long rate;

    int16_t *songData;       // fully decoded interleaved PCM data
} MusicPlayer;

MusicPlayer player;
#endif

void initMusicPlayer(char* filePath) {
#ifdef MUSIC
    int err;

    WHBLogPrint("Initializing music player...");

    AXInitParams init = {AX_INIT_RENDERER_48KHZ, 0, 0};
    AXInitWithParams(&init);

    mpg123_init();
    player.mh = mpg123_new(NULL, &err);
    player.buffer_size = mpg123_outblock(player.mh);
    player.buffer = (unsigned char*) malloc(player.buffer_size * sizeof(unsigned char));

    // open the path and read it all into memory
    FILE *fp = fopen(filePath, "rb");
    if (!fp) {
        WHBLogPrintf("Failed to open file %s", filePath);
        return;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* dataBuffer = (unsigned char*)malloc(fileSize);
    size_t bytesRead = fread(dataBuffer, 1, fileSize, fp);
    fclose(fp);

    // give the data buffer to mpg123 as a feed
    mpg123_open_feed(player.mh);
    mpg123_feed(player.mh, dataBuffer, bytesRead);
    free(dataBuffer);

    mpg123_getformat(player.mh, &player.rate, &player.channels, &err);

    // we need enoughn space for the song, which is interleaved left/right audio (assuming 2 channels)
    size_t capacity = mpg123_length(player.mh) * player.channels;

    size_t totalSamples = 0;
    player.songData = malloc(capacity * sizeof(int16_t));
  
    // read audio data from our memory buffer into the mp3 player buffer
    size_t done;
    while (mpg123_read(player.mh, player.buffer, player.buffer_size, &done) == MPG123_OK && done > 0) {
        size_t numSamples = done / sizeof(int16_t);
        memcpy(player.songData + totalSamples, player.buffer, done);
        totalSamples += numSamples;
    }

    size_t totalFrames = totalSamples / 2;
    player.bufferLeft = malloc(totalFrames * sizeof(int16_t));
    player.bufferRight = malloc(totalFrames * sizeof(int16_t));

    for (size_t i = 0; i < totalFrames; i++) {
        player.bufferLeft[i] = player.songData[i * 2];
        player.bufferRight[i] = player.songData[i * 2 + 1];
    }
    DCFlushRange(player.bufferLeft, totalFrames * sizeof(int16_t));
    DCFlushRange(player.bufferRight, totalFrames * sizeof(int16_t));

    player.voiceLeft = AXAcquireVoice(31, NULL, NULL);
    player.voiceRight = AXAcquireVoice(31, NULL, NULL);

    // setup voice buffers with the full de-interleaved data
    player.bufferInfo.dataType = AX_VOICE_FORMAT_LPCM16;
    player.bufferInfo.loopingEnabled = AX_VOICE_LOOP_ENABLED;
    player.bufferInfo.loopOffset = 0; // TODO: make music loop smoothly with this
    player.bufferInfo.endOffset = totalFrames;
    player.bufferInfo.currentOffset = 0;

    player.bufferInfo.data = player.bufferLeft;
    AXSetVoiceOffsets(player.voiceLeft, &player.bufferInfo);
    player.bufferInfo.data = player.bufferRight;
    AXSetVoiceOffsets(player.voiceRight, &player.bufferInfo);

    // playback rates are adjusted from the mp3 (44khz) to the device's 48khz
    AXSetVoiceSrcType(player.voiceLeft, AX_VOICE_SRC_TYPE_LINEAR);
    AXSetVoiceSrcRatio(player.voiceLeft, (float)player.rate / 48000.0f);
    AXSetVoiceSrcType(player.voiceRight, AX_VOICE_SRC_TYPE_LINEAR);
    AXSetVoiceSrcRatio(player.voiceRight, (float)player.rate / 48000.0f);

    AXVoiceVeData volumeInfo = {0x8000, 0}; // second arg unused
    AXSetVoiceVe(player.voiceLeft, &volumeInfo);
    AXSetVoiceVe(player.voiceRight, &volumeInfo);

    // setting the volume for both left/right channels and the tv/gamepad
    // https://github.com/glastonbridge/immaterial-wiiu-demo/blob/952a683ae05e676ea839c90a5178653616b325b1/sound/Music.cpp#L111
    // first sets 0'th index to full volume, then undoes that and sets the 1'th index to full volume (for right channel?)
    // and then the other 4 for TV are ignored
    AXVoiceDeviceMixData deviceVolumeData[6];
    memset(&deviceVolumeData, 0, sizeof(deviceVolumeData));
    deviceVolumeData[0].bus[0].volume = 0x8000; // left to full, for both
    AXSetVoiceDeviceMix(player.voiceLeft, AX_DEVICE_TYPE_TV, 0, deviceVolumeData);
    AXSetVoiceDeviceMix(player.voiceLeft, AX_DEVICE_TYPE_DRC, 0, deviceVolumeData);
    deviceVolumeData[0].bus[0].volume = 0;
    deviceVolumeData[1].bus[0].volume = 0x8000; // left to 0, right to full
    AXSetVoiceDeviceMix(player.voiceRight, AX_DEVICE_TYPE_TV, 0, deviceVolumeData);
    AXSetVoiceDeviceMix(player.voiceRight, AX_DEVICE_TYPE_DRC, 0, deviceVolumeData);

    WHBLogPrint("Music player initialized");
#endif
}

void playMusic() {
#ifdef MUSIC
    // start our two channels, each of which has been connected to the song and tv/gamepad
    AXSetVoiceCurrentOffset(player.voiceLeft, 0);
    AXSetVoiceState(player.voiceLeft, AX_VOICE_STATE_PLAYING);
    AXSetVoiceCurrentOffset(player.voiceRight, 0);
    AXSetVoiceState(player.voiceRight, AX_VOICE_STATE_PLAYING);
#endif
}

void updateMusic() {
    // Could be used to advance the music each frame
}

void stopMusic() {
#ifdef MUSIC
    AXSetVoiceState(player.voiceLeft, AX_VOICE_STATE_STOPPED);
    AXSetVoiceState(player.voiceRight, AX_VOICE_STATE_STOPPED);

    free(player.buffer);
    free(player.bufferLeft);
    free(player.bufferRight);
    free(player.songData);
    AXFreeVoice(player.voiceLeft);
    AXFreeVoice(player.voiceRight);
    mpg123_close(player.mh);
    mpg123_delete(player.mh);
    mpg123_exit();
    AXQuit();
#endif
}