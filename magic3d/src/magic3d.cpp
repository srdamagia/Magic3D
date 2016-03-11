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

#include <magic3d/magic3d.h>
#include <magic3d/network.h>

Magic3D::Magic3D* Magic3D::Magic3D::instance = NULL;

Magic3D::Magic3D::Magic3D()
{
    applicationPath = "";

    timer   = new Timer();
    fps     = 0;
    timeSec = 0.0f;
    timeSin = 0.0f;

    accumulateCount = 0;
    networkMSAccumulate = 0.0f;
    physicsMSAccumulate = 0.0f;
    updateMSAccumulate  = 0.0f;
    renderMSAccumulate  = 0.0f;

    networkMS = 0.0f;
    physicsMS = 0.0f;
    updateMS  = 0.0f;
    renderMS  = 0.0f;

    running   = false;

    playing = false;
    stoped = false;
}

Magic3D::Magic3D::~Magic3D()
{
    if (timer)
    {
        delete timer;
        timer = NULL;
    }
}

void Magic3D::Magic3D::setApplicationPath(std::string path)
{
    std::string ending = "/";
    getInstance()->applicationPath = path;
    if (!path.empty() && path.compare (path.length() - ending.length(), ending.length(), ending) != 0)
    {
        getInstance()->applicationPath += ending;
    }
}

std::string& Magic3D::Magic3D::getApplicationPath()
{
    return getInstance()->applicationPath;
}

bool Magic3D::Magic3D::start(std::string applicationPath, LogCallBack* callback)
{
    return start(applicationPath, NULL, callback);
}

bool Magic3D::Magic3D::start(std::string applicationPath, Magic3DConfiguration* configuration, LogCallBack* callback)
{
    bool result = true;

    if (!instance)
    {
        instance = new Magic3D();
    }

    if (instance)
    {
        setApplicationPath(applicationPath);
    }

    Log::start();

    Log::getInstance()->setCallBack(callback);

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Magic3D sucessfully started.");

        srand(time(NULL));

        if (configuration)
        {
            instance->configuration = *configuration;
        }
        else
        {
            instance->loadConfiguration();
        }

        result = Input::start() && result;
        result = Renderer::start() && result;
        result = Sound::start() && result;
        result = Physics::start() && result;
        result = ResourceManager::start() && result;
        result = Script::start() && result;
        result = Scene::start() && result;
        result = Config::start() && result;
        result = Network::start() && result;

        instance->setStereoscopy(instance->configuration.STEREOSCOPY);
    }
    else
    {
        Log::log(eLOG_FAILURE, "Magic3D not started.");
    }

    return result;
}

bool Magic3D::Magic3D::finish()
{
    if (instance)
    {
        Network::finish();
        Config::finish();
        Script::finish();
        Scene::finish();
        ResourceManager::finish();
        Physics::finish();
        Renderer::finish();
        Sound::finish();
        Input::finish();

        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Magic3D sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Magic3D not finished.");
    }

    Log::finish();

    return !instance;
}

const Magic3D::Magic3DConfiguration& Magic3D::Magic3D::getConfiguration()
{
    return configuration;
}

void Magic3D::Magic3D::setConfiguration(std::string selected)
{
    configuration.NAME = selected;
}

void Magic3D::Magic3D::loadConfiguration()
{
    bool loaded = false;
    XMLDocument* doc = new XMLDocument();

    bool result = doc->LoadFile((Magic3D::getInstance()->getApplicationPath() + CONFIGURATION_FILE).c_str()) == XML_SUCCESS;
    if (result)
    {
        XMLElement* pElem = doc->FirstChildElement();

        // should always have a valid root but handle gracefully if it does
        if (pElem)
        {
            XMLElement* cfg = pElem;

            configuration.TITLE         = Xml::loadString(cfg, CFG_XML_TITLE);
            configuration.WINDOW_WIDTH  = Xml::loadInt(cfg, CFG_XML_WINDOW_WIDTH);
            configuration.WINDOW_HEIGHT = Xml::loadInt(cfg, CFG_XML_WINDOW_HEIGHT);
            configuration.FULLSCREEN    = Xml::loadBool(cfg, CFG_XML_FULLSCREEN);
            configuration.CURSOR        = Xml::loadBool(cfg, CFG_XML_CURSOR);
            configuration.VSYNC         = Xml::loadBool(cfg, CFG_XML_VSYNC);

            configuration.SERVER        = Xml::loadBool(cfg, CFG_XML_SERVER);
            configuration.ADDRESS       = Xml::loadString(cfg, CFG_XML_ADDRESS);
            if (configuration.ADDRESS.compare(M3D_XML_NULL) == 0)
            {
                configuration.ADDRESS.clear();
            }
            configuration.PORT          = Xml::loadInt(cfg, CFG_XML_PORT);
            configuration.CLIENTS       = Xml::loadInt(cfg, CFG_XML_CLIENTS);

            configuration.NAME          = Xml::loadString(cfg, CFG_XML_SELECTED);

            cfg = cfg->FirstChildElement(configuration.NAME.c_str());

            if (cfg != NULL)
            {
                configuration.FBO_MAP_SIZE        = Xml::loadInt(cfg, CFG_XML_FBO_MAP_SIZE);
                configuration.SHADOW_MAP_SIZE     = Xml::loadInt(cfg, CFG_XML_SHADOW_MAP_SIZE);
                configuration.REFLECTION_MAP_SIZE = Xml::loadInt(cfg, CFG_XML_REFLECTION_MAP_SIZE);
                configuration.BLUR_MAP_SIZE       = Xml::loadInt(cfg, CFG_XML_BLUR_MAP_SIZE);
                configuration.GLOW_MAP_SIZE       = Xml::loadInt(cfg, CFG_XML_GLOW_MAP_SIZE);
                configuration.SCREEN_FBO_SIZE     = Xml::loadInt(cfg, CFG_XML_SCREEN_FBO_SIZE);
                configuration.SCREEN_VERTICES     = Xml::loadInt(cfg, CFG_XML_SCREEN_VERTICES);

                configuration.SHOW_FPS             = Xml::loadInt(cfg, CFG_XML_SHOW_FPS);
                configuration.SHOW_INFORMATION     = Xml::loadInt(cfg, CFG_XML_SHOW_INFORMATION);
                configuration.SHOW_COORDINATES     = Xml::loadInt(cfg, CFG_XML_SHOW_COORDINATES);

                configuration.SHADOWS              = Xml::loadInt(cfg, CFG_XML_SHADOWS);
                configuration.SOFT_SHADOWS         = Xml::loadInt(cfg, CFG_XML_SOFT_SHADOWS);
                configuration.REFLECTIONS          = Xml::loadInt(cfg, CFG_XML_REFLECTIONS);
                configuration.GLOW                 = Xml::loadInt(cfg, CFG_XML_GLOW);
                configuration.STEREOSCOPY          = Xml::loadInt(cfg, CFG_XML_STEREOSCOPY);                

                loaded = true;

                Log::log(eLOG_SUCCESS, "Configuration Loaded!");
            }
        }
    }

    if (doc)
    {
        delete doc;
        doc = NULL;
    }

    if (!loaded)
    {
        Log::log(eLOG_FAILURE, "Failed To Load Configuration!");

        configuration.TITLE  = "Magic3D";
        configuration.WINDOW_WIDTH  = 800;
        configuration.WINDOW_HEIGHT = 600;
        configuration.FULLSCREEN    = false;
        configuration.CURSOR        = true;
        configuration.VSYNC         = false;

        configuration.NAME                 = "default";
        configuration.FBO_MAP_SIZE         = 512;
        configuration.SHADOW_MAP_SIZE      = 512;
        configuration.REFLECTION_MAP_SIZE  = 512;
        configuration.BLUR_MAP_SIZE        = 256;
        configuration.GLOW_MAP_SIZE        = 64;
        configuration.SCREEN_FBO_SIZE      = 1024;
        configuration.SCREEN_VERTICES      = 64;

        configuration.SHOW_FPS         = false;
        configuration.SHOW_INFORMATION = false;
        configuration.SHOW_COORDINATES = false;

        configuration.SHADOWS      = false;
        configuration.SOFT_SHADOWS = false;
        configuration.REFLECTIONS  = false;
        configuration.GLOW         = false;
        configuration.STEREOSCOPY  = false;

        configuration.SERVER   = false;
        configuration.ADDRESS  = "";
        configuration.PORT     = 31234;
        configuration.CLIENTS  = 64;
    }
}

void Magic3D::Magic3D::saveConfiguration()
{
    XMLDocument doc;
    XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
    doc.LinkEndChild( decl );

    XMLElement* root = doc.NewElement( "Magic3D" );
    doc.LinkEndChild( root );

    XMLComment* comment = doc.NewComment(" Magic3D Configuration ");
    root->LinkEndChild( comment );

    if (doc.SaveFile((Magic3D::getInstance()->getApplicationPath() + CONFIGURATION_FILE).c_str()) == XML_SUCCESS)
    {
        Log::log(eLOG_SUCCESS, "Configuration Saved!");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Failed To Save Configuration!");
    }
}

bool Magic3D::Magic3D::run()
{
    if (!getInstance()->running)
    {
        getInstance()->running = true;

        getInstance()->timer->newFrame();

        if (getInstance()->getCurrentTime() >= 1.0f)
        {
            getInstance()->fps = getInstance()->timer->getFPS();
            getInstance()->timer->reset();
        }

        float elapsed = getInstance()->timer->getTimeElapsed();
        getInstance()->timeSec += elapsed * 0.001f;
        getInstance()->timeSin += elapsed * 0.001f;

        if (fabs(getInstance()->timeSec) >= 1.0f)
        {
            getInstance()->timeSec = -(1.0 - (getInstance()->timeSec - 1.0));
        }

        float pi = 2.0f * M_PI;
        if (getInstance()->timeSin >= pi)
        {
            getInstance()->timeSin = getInstance()->timeSin - pi;
        }

        if (!Scene::getInstance()->isLoading())
        {
            if (!getInstance()->loadMGE.empty())
            {
                Scene::getInstance()->clear(true);
                Scene::getInstance()->load(getInstance()->loadMGE, false);
                getInstance()->loadMGE.clear();
            }

            if (!getInstance()->loadMGEAdditive.empty())
            {
                Scene::getInstance()->load(getInstance()->loadMGEAdditive.back(), true);
                getInstance()->loadMGEAdditive.pop_back();
            }
        }

        if (getInstance()->playing)
        {
            getInstance()->accumulateCount++;

            getInstance()->setTimeReferenceMain();
            Network::getInstance()->update();
            getInstance()->networkMSAccumulate += getInstance()->getTimeSinceReferenceMain();

            getInstance()->setTimeReferenceMain();
            Script::getInstance()->update();
            getInstance()->updateMSAccumulate += getInstance()->getTimeSinceReferenceMain();

            getInstance()->setTimeReferenceMain();
            Physics::getInstance()->update();
            getInstance()->physicsMSAccumulate += getInstance()->getTimeSinceReferenceMain();

            getInstance()->setTimeReferenceMain();
            Scene::getInstance()->update();
            getInstance()->updateMSAccumulate += getInstance()->getTimeSinceReferenceMain();

            getInstance()->setTimeReferenceMain();
            Renderer::getInstance()->render(Scene::getInstance());
            getInstance()->renderMSAccumulate += getInstance()->getTimeSinceReferenceMain();

            if (getInstance()->accumulateCount >= 30)
            {
                getInstance()->networkMS = getInstance()->networkMSAccumulate / (float)getInstance()->accumulateCount;
                getInstance()->physicsMS = getInstance()->physicsMSAccumulate / (float)getInstance()->accumulateCount;
                getInstance()->updateMS  = getInstance()->updateMSAccumulate  / (float)getInstance()->accumulateCount;
                getInstance()->renderMS  = getInstance()->renderMSAccumulate  / (float)getInstance()->accumulateCount;

                getInstance()->networkMSAccumulate = 0.0f;
                getInstance()->physicsMSAccumulate = 0.0f;
                getInstance()->updateMSAccumulate  = 0.0f;
                getInstance()->renderMSAccumulate  = 0.0f;

                getInstance()->accumulateCount = 0;
            }
        }
        else
        {
            if (Scene::getInstance()->isLoading())
            {
                getInstance()->setTimeReferenceMain();
                Scene::getInstance()->load();
                getInstance()->updateMS = getInstance()->getTimeSinceReferenceMain();
            }
            else
            {
                if (!Script::getInstance()->isPlaying() && getInstance()->loadMGEAdditive.empty())
                {
                    #if !defined(_MGE_)
                        Scene::getInstance()->playObjectsTweens();
                        Script::getInstance()->play();
                    #endif
                    play();
                }
            }
        }

        getInstance()->running = false;
    }

    Window* window = Renderer::getInstance()->getWindow();
    return window && window->isActive() && !getInstance()->stoped;
}

bool Magic3D::Magic3D::isPlaying()
{
    return getInstance()->playing && !getInstance()->stoped;
}

void Magic3D::Magic3D::play()
{
    getInstance()->playing = true;
    getInstance()->stoped = false;
}

void Magic3D::Magic3D::pause()
{
    getInstance()->playing = false;
}

void Magic3D::Magic3D::stop()
{
    getInstance()->playing = false;
    getInstance()->stoped = true;
}

Magic3D::Magic3D* Magic3D::Magic3D::getInstance()
{
    return instance;
}

void Magic3D::Magic3D::setTimeScale(float scale)
{
    timer->setScale(scale);
}

float Magic3D::Magic3D::getTimeScale()
{
    return timer->getScale();
}

float Magic3D::Magic3D::getCurrentTime()
{
    return timer->getCurrentTime();
}

float Magic3D::Magic3D::getTimeSinceStart()
{
    return timer->getTimeSinceStart();
}

float Magic3D::Magic3D::getTime()
{
    return timeSec;
}

float Magic3D::Magic3D::getTimeSin()
{
    return sin(timeSin);
}

float Magic3D::Magic3D::getTicks()
{
    return timer->getTicks();
}

float Magic3D::Magic3D::getElapsedTime()
{
    return timer->getTimeElapsed();
}

float Magic3D::Magic3D::getElapsedTimeReal()
{
    return timer->getTimeElapsedReal();
}

int Magic3D::Magic3D::getFPS()
{
    return fps;
}

void Magic3D::Magic3D::setTimeReferenceMain()
{
    timer->setReferenceMain();
}

float Magic3D::Magic3D::getTimeSinceReferenceMain()
{
    return timer->getSinceReferenceMain();
}

void Magic3D::Magic3D::setTimeReference()
{
    timer->setReference();
}

float Magic3D::Magic3D::getTimeSinceReference()
{
    return timer->getSinceReference();
}

float Magic3D::Magic3D::getNetworkMS()
{
    return networkMS;
}

float Magic3D::Magic3D::getPhysicsMS()
{
    return physicsMS;
}

float Magic3D::Magic3D::getUpdateMS()
{
    return updateMS;
}

float Magic3D::Magic3D::getRenderMS()
{
    return renderMS;
}

void Magic3D::Magic3D::loadScene(std::string mge)
{
    loadMGE = mge;
}

void Magic3D::Magic3D::loadSceneAdditive(std::string mge)
{
    loadMGEAdditive.push_back(mge);
}

void Magic3D::Magic3D::setStereoscopy(bool enabled)
{
    configuration.STEREOSCOPY = enabled;

    if (Renderer::getInstance())
    {
        std::vector<ViewPort*>* viewports = Renderer::getInstance()->getViewPorts();

        Camera* ortho = NULL;
        Camera* persp = NULL;

        if (!viewports->empty())
        {
            ortho = viewports->at(0)->getOrthographic();
            persp = viewports->at(0)->getPerspective();
        }

        Renderer::getInstance()->removeAllViewPorts();
        if (Magic3D::getInstance()->getConfiguration().STEREOSCOPY)
        {
            ViewPort* main = new ViewPort(Vector4(0.0f, 0.0f, 0.5f, 1.0f), 0, 0);
            main->setOrthographic(ortho);
            main->setPerspective(persp);
            viewports->push_back(main);
            ViewPort* stereoscopy = new ViewPort(Vector4(0.5f, 0.0f, 0.5f, 1.0f), 0, 0);
            viewports->push_back(stereoscopy);
            stereoscopy->setParent(main);
        }
        else
        {
            ViewPort* view = new ViewPort(Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0);
            view->setOrthographic(ortho);
            view->setPerspective(persp);
            viewports->push_back(view);
        }
    }
}

std::string Magic3D::Magic3D::getApplicationPath(std::string path)
{
    std::string applicationPath = path;
#if defined(__APPLE__)
    size_t pos = applicationPath.find("/Contents/MacOS");
    applicationPath = applicationPath.substr(0, pos);
    pos = applicationPath.find_last_of('/');
    applicationPath = applicationPath.substr(0, pos + 1);
#else
    size_t pos = applicationPath.find_last_of('/');
    if (pos == std::string::npos)
    {
        pos = applicationPath.find_last_of('\\');
    }
    if (pos == std::string::npos)
    {
        applicationPath.clear();
    }
    else
    {
        applicationPath = applicationPath.substr(0, pos + 1);
        pos = applicationPath.find('\\');
        while (pos != applicationPath.npos)
        {
            applicationPath.replace(pos, 1, "/");
            pos = applicationPath.find('\\');
        }
    }
#endif
    return applicationPath;
}
