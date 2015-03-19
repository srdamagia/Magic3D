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

#include <magic3d/magic3d.h>
#include <magic3d/sound/sound_ogg.h>

bool Magic3D::Sound::enabled = false;

Magic3D::Sound::Sound(const Sound& sound, std::string name) : Object(sound, name)
{
    this->fileName = sound.fileName;

    this->box = sound.box;

    this->gain = sound.gain;
    this->pitch = sound.pitch;
    this->distance = sound.distance;

    this->loop = sound.loop;
    this->needPlay = sound.needPlay;

    this->child = sound.child ? sound.child->spawn(name) : NULL;
}

Magic3D::Sound::Sound(std::string name) : Object(eOBJECT_SOUND, eRENDER_3D, name)
{
    box = Box(Vector3(-0.125f, -0.125f, -0.125f), Vector3(0.125f, 0.125f, 0.125f));

    gain     = 1.0f;
    pitch    = 1.0f;
    distance = 100.0f;
    loop     = true;
    needPlay = false;

    child    = NULL;
}

Magic3D::Sound::~Sound()
{
    if (child)
    {
        child->stop();
        delete child;
        child = NULL;
    }
}

Magic3D::Sound* Magic3D::Sound::spawn(std::string name) const
{
    return (new Sound(*this, name));
}

void Magic3D::Sound::setFileName(std::string name)
{
    fileName = name;
    load();
}

const std::string& Magic3D::Sound::getFileName()
{
    return fileName;
}

void Magic3D::Sound::load()
{
    std::string lower = getFileName();
    const int length = lower.length();
    for(int i=0; i!=length; ++i)
    {
        lower[i] = std::tolower(lower[i]);
    }
    std::string ending = ".ogg";
    if (lower.compare (lower.length() - ending.length(), ending.length(), ending) == 0)
    {
        if (child)
        {
            child->stop();
            delete child;
            child = NULL;
        }
        if (!child)
        {
            child = new SoundOGG(getName());
            child->setFileName(getFileName());
        }
    }
    else
    {

    }
}

bool Magic3D::Sound::start()
{
    Log::log(eLOG_PLAINTEXT, "Start Initializing OpenAL...");
    ALCcontext *context;
    ALCdevice *device;

    //Open device
    device = alcOpenDevice(NULL);
    if (device != NULL) {
        //Create context(s)
        context = alcCreateContext(device,NULL);
        //Set active context
        if (alcMakeContextCurrent(context)) {
            enabled = true;
        }
        char* defaultDeviceName = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
        Log::log(eLOG_RENDERER, defaultDeviceName);
        //delete[] defaultDeviceName;
    } else {
        Log::log(eLOG_FAILURE, "Default sound device not found...");
    }
    Log::log(eLOG_SUCCESS, "OpenAL sucessfully started.");

    return enabled;
}

bool Magic3D::Sound::finish()
{
    ALCcontext *context;
    ALCdevice *device;

    //Unregister extensions

    //Get active context
    context = alcGetCurrentContext();
    //Get device for active context
    device = alcGetContextsDevice(context);
    //Disable context
    alcMakeContextCurrent(NULL);
    //Release context(s)
    alcDestroyContext(context);
    //Close device
    alcCloseDevice(device);

    enabled = false;

    Log::log(eLOG_SUCCESS, "OpenAL sucessfully finished.");

    return true;
}

bool Magic3D::Sound::isEnabled()
{
    return enabled;
}

bool Magic3D::Sound::update()
{
    bool result = false;
    Object::update();
    if (isEnabled() && isVisible())
    {
        if (needPlay)
        {
            play();
            needPlay = false;
        }
        ViewPort* view = Renderer::getInstance()->getCurrentViewPort();
        Camera* camera = view->getPerspective();

        if (camera)
        {
            updateListener(camera->getPositionFromParent(), camera->getDirectionFront(), camera->getDirectionUp());
        }
        if (child)
        {
            child->setParent(getParent());
            child->setMatrix(getMatrix());

            child->setGain(getGain());
            child->setPitch(getPitch());
            child->setDistance(getDistance());
            child->setLooping(isLooping());
        }

        render();

        result = true;
    }
    return result;
}

const Magic3D::Box& Magic3D::Sound::getBoundingBox()
{
    return box;
}

void Magic3D::Sound::updateListener(Vector3 position, Vector3 front, Vector3 up)
{
    float orientation[6];
    orientation[0] = front.getX();
    orientation[1] = front.getY();
    orientation[2] = front.getZ();
    orientation[3] = up.getX();
    orientation[4] = up.getY();
    orientation[5] = up.getZ();

    alListener3f(AL_POSITION, position.getX(), position.getY(), position.getZ());
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, orientation);
    alListenerf(AL_GAIN, Scene::getInstance()->getVolumeMaster());
}

void Magic3D::Sound::render()
{
    if (child)
    {
        child->render();
    }
}

void Magic3D::Sound::play()
{
    if (child)
    {
        child->play();
    }
}

void Magic3D::Sound::stop()
{
    if (child)
    {
        child->stop();
    }
}

void Magic3D::Sound::setGain(float gain)
{
    this->gain = gain;
}

float Magic3D::Sound::getGain()
{
    return gain;
}

void Magic3D::Sound::setPitch(float pitch)
{
    this->pitch = pitch;
}

float Magic3D::Sound::getPitch()
{
    return pitch;
}

void Magic3D::Sound::setDistance(float distance)
{
    this->distance = distance;
}

float Magic3D::Sound::getDistance()
{
    return distance;
}

void Magic3D::Sound::setLooping(bool loop)
{
    this->loop = loop;
}

bool Magic3D::Sound::isLooping()
{
    return loop;
}

bool Magic3D::Sound::isPlaying()
{
    bool result = false;
    if (child)
    {
        result = child->isPlaying();
    }
    return result;
}

Magic3D::XMLElement* Magic3D::Sound::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* sound = root->GetDocument()->NewElement(M3D_SOUND_XML);
        root->LinkEndChild(sound);

        saveFloat( sound, M3D_SOUND_XML_GAIN,     gain);
        saveFloat( sound, M3D_SOUND_XML_PITCH,    pitch);
        saveFloat( sound, M3D_SOUND_XML_DISTANCE, distance);
        saveInt(   sound, M3D_SOUND_XML_LOOP,     loop ? 1 : 0);
        saveString(sound, M3D_SOUND_XML_FILE,     fileName);
        saveInt(   sound, M3D_SOUND_XML_PLAYING,  isPlaying());
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Sound::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* sound = root->FirstChildElement(M3D_SOUND_XML);

        gain     = loadFloat( sound, M3D_SOUND_XML_GAIN);
        pitch    = loadFloat( sound, M3D_SOUND_XML_PITCH);
        distance = loadFloat( sound, M3D_SOUND_XML_DISTANCE);
        loop     = loadInt(   sound, M3D_SOUND_XML_LOOP);
        setFileName(loadString(sound, M3D_SOUND_XML_FILE));

        needPlay = loadInt(sound, M3D_SOUND_XML_PLAYING);
    }
    return root;
}

void Magic3D::Sound::logError(int index)
{
    int error = alGetError();

    bool show = error != AL_NO_ERROR;

    if (show)
    {
        std::stringstream sstm;
        sstm << "OpenAL ERROR: " << index << " - ";

        switch (error)
        {
            case AL_INVALID_NAME:      sstm << "AL_INVALID_NAME"; break;
            case AL_INVALID_ENUM:      sstm << "AL_INVALID_ENUM"; break;
            case AL_INVALID_VALUE:     sstm << "AL_INVALID_VALUE"; break;
            case AL_INVALID_OPERATION: sstm << "AL_INVALID_OPERATION"; break;
            case AL_OUT_OF_MEMORY :    sstm << "AL_OUT_OF_MEMORY"; break;
        }

        Log::log(eLOG_FAILURE, sstm.str().c_str());
    }
}
