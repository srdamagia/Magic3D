/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2015
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

#ifndef MAGIC3D_SOUND_H
#define MAGIC3D_SOUND_H

#include <magic3d/object.h>
#include <magic3d/log.h>

#define M3D_SOUND_NUMBUFFERS             4
#define	M3D_SOUND_SERVICE_UPDATE_PERIOD 20

namespace Magic3D
{
#define M3D_SOUND_XML          "Sound"
#define M3D_SOUND_XML_GAIN     "gain"
#define M3D_SOUND_XML_PITCH    "pitch"
#define M3D_SOUND_XML_DISTANCE "distance"
#define M3D_SOUND_XML_LOOP     "loop"
#define M3D_SOUND_XML_FILE     "file"
#define M3D_SOUND_XML_PLAYING  "playing"

class Sound : public Object
{
private:
    Sound* child;

    std::string fileName;

    Box box;

    float gain;
    float pitch;
    float distance;

    bool loop;
    bool needPlay;
    static bool enabled;

    void updateChild();

protected:
    virtual void load();

    Sound(const Sound& sound, std::string name);

public:
    Sound(std::string name);
    ~Sound();
    virtual Sound* spawn(std::string name) const;

    void setFileName(std::string name);
    const std::string& getFileName();

    static bool start();
    static bool finish();
    static bool isEnabled();

    virtual bool update();

    virtual const Box& getBoundingBox();

    void updateListener(Vector3 position, Vector3 front, Vector3 up);
    virtual void render();
    virtual void play();
    virtual void stop();

    virtual void setGain(float gain);
    virtual float getGain();

    virtual void setPitch(float pitch);
    virtual float getPitch();

    virtual void setDistance(float distance);
    virtual float getDistance();

    virtual void setLooping(bool loop);
    virtual bool isLooping();

    virtual bool isPlaying();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);

    static void logError(int index);
};

}

#endif // MAGIC3D_SOUND_H
