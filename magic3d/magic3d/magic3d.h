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

#ifndef MAGIC3D_H
#define MAGIC3D_H

#include <magic3d/timer.h>
#include <magic3d/physics/physics.h>
#include <magic3d/config.h>
#include <magic3d/log.h>
#include <magic3d/eventlistener.h>
#include <magic3d/input.h>
#include <magic3d/scene.h>
#include <magic3d/file.h>
#include <magic3d/image/image.h>
#include <magic3d/resource/resourcemngr.h>
#include <magic3d/renderer/renderer.h>
#include <magic3d/script/script.h>
#include <magic3d/tween/tweens.h>
#include <magic3d/network.h>

//PRECOMPILER MACROS*******************************
//MAGIC3D_OES1
//MAGIC3D_OES2
//MAGIC3D_OES3
//MAGIC3D_LEGACY
//MAGIC3D_LOG
//MAGIC3D_IOS
//MAGIC3D_ANDROID
//MAGIC3D_MEMORY
//*************************************************

namespace Magic3D
{

#define MAGIC3D_VERSION "0.9.0"
#define CONFIGURATION_FILE "config.xml"

#define CFG_XML_SELECTED            "selected"
#define CFG_XML_TITLE               "TITLE"
#define CFG_XML_WINDOW_WIDTH        "WINDOW_WIDTH"
#define CFG_XML_WINDOW_HEIGHT       "WINDOW_HEIGHT"
#define CFG_XML_FULLSCREEN          "FULLSCREEN"
#define CFG_XML_CURSOR              "CURSOR"
#define CFG_XML_FBO_MAP_SIZE        "FBO_MAP_SIZE"
#define CFG_XML_SHADOW_MAP_SIZE     "SHADOW_MAP_SIZE"
#define CFG_XML_REFLECTION_MAP_SIZE "REFLECTION_MAP_SIZE"
#define CFG_XML_BLUR_MAP_SIZE       "BLUR_MAP_SIZE"
#define CFG_XML_GLOW_MAP_SIZE       "GLOW_MAP_SIZE"
#define CFG_XML_SCREEN_FBO_SIZE     "SCREEN_FBO_SIZE"
#define CFG_XML_SCREEN_VERTICES     "SCREEN_VERTICES"
#define CFG_XML_SHOW_FPS            "SHOW_FPS"
#define CFG_XML_SHOW_INFORMATION    "SHOW_INFORMATION"
#define CFG_XML_SHOW_COORDINATES    "SHOW_COORDINATES"
#define CFG_XML_SHADOWS             "SHADOWS"
#define CFG_XML_SOFT_SHADOWS        "SOFT_SHADOWS"
#define CFG_XML_REFLECTIONS         "REFLECTIONS"
#define CFG_XML_GLOW                "GLOW"
#define CFG_XML_STEREOSCOPY         "STEREOSCOPY"
#define CFG_XML_SERVER              "SERVER"
#define CFG_XML_ADDRESS             "ADDRESS"
#define CFG_XML_PORT                "PORT"
#define CFG_XML_CLIENTS             "CLIENTS"

enum RENDERER
{
    RENDERER_OPENGL,
    RENDERER_OPENGL_ES,
    RENDERER_OPENGL_ES2
};

class Input;
class Renderer;
class ResourceManager;

struct Magic3DConfiguration
{
    std::string NAME;
    std::string TITLE;

    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    bool FULLSCREEN;
    bool CURSOR;

    int FBO_MAP_SIZE;
    int SHADOW_MAP_SIZE;
    int REFLECTION_MAP_SIZE;
    int BLUR_MAP_SIZE;
    int GLOW_MAP_SIZE;
    int SCREEN_FBO_SIZE;
    int SCREEN_VERTICES;

    bool SHOW_FPS;
    bool SHOW_INFORMATION;
    bool SHOW_COORDINATES;

    bool SHADOWS;
    bool SOFT_SHADOWS;
    bool REFLECTIONS;
    bool GLOW;
    bool STEREOSCOPY;

    bool SERVER;
    std::string ADDRESS;
    int PORT;
    int CLIENTS;
};

class Magic3D
{
private:
    static Magic3D* instance;

    Magic3DConfiguration configuration;

    Timer* timer;

    std::string applicationPath;
    std::string loadMGE;
    std::vector<std::string> loadMGEAdditive;

    int accumulateCount;
    float networkMSAccumulate;
    float physicsMSAccumulate;
    float updateMSAccumulate;
    float renderMSAccumulate;

    float networkMS;
    float physicsMS;
    float updateMS;
    float renderMS;

    float timeSec;
    float timeSin;

    int fps;

    bool running;
    bool playing;
    bool stoped;

    Magic3D();
    ~Magic3D();

    void loadConfiguration();
    void saveConfiguration();

    float getCurrentTime();
    void setTimeReferenceMain();
    float getTimeSinceReferenceMain();

public:
    static void setApplicationPath(std::string path);
    static std::string& getApplicationPath();
    static bool start(std::string applicationPath, LogCallBack* callback = NULL);
    static bool start(std::string applicationPath, Magic3DConfiguration* configuration, LogCallBack* callback = NULL);
    static bool finish();

    const Magic3DConfiguration& getConfiguration();
    void setConfiguration(std::string selected);

    static bool run();
    static bool isPlaying();
    static void play();
    static void pause();
    static void stop();

    static Magic3D* getInstance();

    void setTimeScale(float scale);
    float getTimeScale();
    float getTicks();
    float getElapsedTime();
    float getElapsedTimeReal();
    float getTimeSinceStart();
    float getTime();
    float getTimeSin();
    int getFPS();

    void setTimeReference();
    float getTimeSinceReference();

    float getNetworkMS();
    float getPhysicsMS();
    float getUpdateMS();
    float getScriptMS();
    float getRenderMS();

    void loadScene(std::string mge);
    void loadSceneAdditive(std::string mge);

    void setStereoscopy(bool enabled);
};

}

#endif // MAGIC3D_H
