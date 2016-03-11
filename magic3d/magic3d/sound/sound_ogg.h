/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2016 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2016
Thiago C. Moraes
http://www.magictech.com.br

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef MAGIC3D_SOUND_OGG_H
#define MAGIC3D_SOUND_OGG_H

#include <magic3d/sound/sound.h>
#include <magic3d/file.h>

namespace Magic3D
{
class SoundOGG : public Sound
{
    Memory*         memoryFile;
    vorbis_info*    psVorbisInfo;
    vorbis_comment* vorbisComment;
    char*           pDecodeBuffer;

    OggVorbis_File  sOggVorbisFile;
    ov_callbacks    oggCallbacks;

    unsigned long   ulFrequency;
    unsigned long   ulFormat;
    unsigned long   ulChannels;
    unsigned long   ulBufferSize;
    unsigned long   ulBytesWritten;

    ALuint          uiBuffers[M3D_SOUND_NUMBUFFERS];
    ALuint          uiSource;
    ALuint          uiBuffer;
    ALint           iState;
    ALint           iLoop;
    ALint           iBuffersProcessed;
    ALint           iTotalBuffersProcessed;
    ALint           iQueuedBuffers;

    bool            loaded;
    bool            started;

    bool stream(ALuint buffer);
    void empty();
    void check();
    void swap(short &s1, short &s2);
    const char* getErrorString(int code);

protected:
    void load();

    SoundOGG(const SoundOGG& sound, std::string name);
public:
    SoundOGG(std::string name);
    ~SoundOGG();
    virtual void* spawn(std::string name) const;

    unsigned long decode(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
    void release();
    virtual void render();
    virtual void play();
    virtual void stop();

    bool isStarted();
    virtual bool isPlaying();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_SOUND_OGG_H
