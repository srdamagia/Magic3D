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

#include "Game.h"

Game* Game::instance = NULL;

// EVENTS *********************************************
class AccelerometerEvent : public Magic3D::EventListener_Accelerometer
{
private:
    Game* game;
public:
    AccelerometerEvent(Game* game) : Magic3D::EventListener_Accelerometer()
    {
        this->game = game;
    }
    
    AccelerometerEvent()
    {
        
    }
    
    void accelerometer(float x, float y, float z)
    {
        
        Magic3D::ViewPort* view =  Magic3D::Renderer::getInstance()->getViewPort(0);
        
        if (view && view->getPerspective())
        {
            //view->perspective->setPosition(view->perspective->getPosition() + Magic3D::Vector3(-x, 0.0f, 0.0f) * 0.1f);
        }
    }
};

class TouchEvent : public Magic3D::EventListener_Touch
{
private:
    Game* game;
    
    int posX; 
    int posY;
public:
    TouchEvent(Game* game) : Magic3D::EventListener_Touch()
    {
        this->game = game;
        
        posX = 0;
        posY = 0;
    }
    
    TouchEvent()
    {
        
    }
    
    void down(int x, int y, int finger) 
    {
        Magic3D::Object* object = NULL;
        
        Magic3D::ViewPort* view =  Magic3D::Renderer::getInstance()->getViewPort(0);
        
        if (view && view->getOrthographic())
        {
            object = view->getOrthographic()->pick(x, y, 0, false);
        }
        
        if (!object && view && view->getPerspective())
        {
            object = view->getPerspective()->pick(x, y, 0, false);
        }
        
        if (object)
        {
            object->setSelected(!object->isSelected());
        }
        
        posX = x;
        posY = y;
    }

    void pressed(int x, int y, int finger) 
    {
        
    }
    
    void up(int x, int y, int finger) 
    {
        posX = 0;
        posY = 0;
    }
    
    void move(int deltaX, int deltaY, int finger) 
    {
        
        Magic3D::ViewPort* view =  Magic3D::Renderer::getInstance()->getViewPort(0);
        
        if (view && view->getPerspective())
        {
            view->getPerspective()->rotate(Magic3D::Vector3((deltaY - posY) * -0.1f, (deltaX - posX) * 0.1f, 0.0f));
            posX = deltaX;
            posY = deltaY;
        }
    }
};
// ***************************************************

Game::Game(std::string path, std::string pathUser, int width, int height)
{
    std::string log = pathUser + "/magic3d.log.html";
    Magic3D::Log::setLogFile(log.c_str());
    
    Magic3D::Magic3DConfiguration configuration;
    
    configuration.WINDOW_WIDTH  = 0;
    configuration.WINDOW_HEIGHT = 0;
    configuration.FULLSCREEN    = false;
    configuration.CURSOR = false;
    configuration.VSYNC = false;
    
    configuration.NAME                 = "iOS";
    configuration.TITLE                = "Game";
    configuration.FBO_MAP_SIZE         = 512;
    configuration.SHADOW_MAP_SIZE      = 1024;
    configuration.REFLECTION_MAP_SIZE  = 512;
    configuration.BLUR_MAP_SIZE        = 256;
    configuration.GLOW_MAP_SIZE        = 64;
    
    configuration.SHOW_FPS         = true;
    configuration.SHOW_INFORMATION = false;
    configuration.SHOW_COORDINATES = false;
    
    configuration.SHADOWS      = true;
    configuration.SOFT_SHADOWS = false;
    configuration.REFLECTIONS  = false;
    configuration.GLOW         = true;
    configuration.STEREOSCOPY  = false;

    configuration.CONSOLE = false;
    configuration.SERVER  = true;
    configuration.ADDRESS = "null";
    configuration.PORT    = 31234;
    configuration.CLIENTS = 64;
    
    if (width > 1500 || height > 1500)
    {
        configuration.SCREEN_FBO_SIZE  = 2048;
        configuration.SCREEN_VERTICES  = 64;
    }
    else if (width > 640 || height > 640)
    {
        configuration.SCREEN_FBO_SIZE  = 1024;
        configuration.SCREEN_VERTICES  = 32;
    }
    else
    {
        configuration.SCREEN_FBO_SIZE  = 512;
        configuration.SCREEN_VERTICES  = 16;
    }
    
    
    if (Magic3D::Magic3D::start("", &configuration) && Magic3D::Renderer::getInstance()->createWindow(true))
    {
        Magic3D::ResourceManager::getInstance()->setPath(path + "/data/", false);
        Magic3D::ResourceManager::getInstance()->setUserPath(pathUser);
        Magic3D::Magic3D::play();
        
        Magic3D::Renderer::getInstance()->getWindow()->resize(width, height);
    
        //Magic3D::Input::getInstance()->addEventListener(new TouchEvent(this));
        //Magic3D::Input::getInstance()->addEventListener(new AccelerometerEvent(this));
    
        Magic3D::Scene::getInstance()->load("main.mge", false);
        
        //Magic3D::Renderer::getInstance()->setRenderMode(Magic3D::eRENDER_MODE_NORMALS);
    }
}

Game::~Game()
{
    
}

Game* Game::getInstance()
{
    return instance;
}

bool Game::start(std::string path, std::string pathUser, int width, int height)
{
    bool result = false;
    
    if (!instance)
    {
        instance = new Game(path, pathUser, width, height);
        result = true;
    }
    
    return result;
}

bool Game::finish()
{
    bool result = false;
    
    Magic3D::Magic3D::stop();
    
    if (instance)
    {
        delete instance;
        instance = NULL;
        result = true;
    }
    
    return result;
}

void Game::AI()
{
    
}
