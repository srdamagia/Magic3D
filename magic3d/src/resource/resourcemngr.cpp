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
#include <magic3d/resource/resourcemngr.h>

Magic3D::ResourceManager* Magic3D::ResourceManager::instance = NULL;

//******************************************************************************

Magic3D::ResourceManager::ResourceManager()
{
    path = "data/";

    objects = new ResourcesObject();
    models = new ResourcesModel();
    textures = new ResourcesTexture();
    shaders = new ResourcesShader();
    materials = new ResourcesMaterial();
    animations = new ResourcesAnimation();
    fonts = new ResourcesFont();
}

Magic3D::ResourceManager::~ResourceManager()
{
    clear(true);

    delete objects;
    delete models;
    delete materials;
    delete textures;
    delete shaders;
    delete animations;
    delete fonts;
}

void Magic3D::ResourceManager::clear(bool all)
{
    std::vector<ViewPort*>* viewports = Renderer::getInstance()->getViewPorts();
    int count = viewports->size();
    for (int i = 0; i < count; i++)
    {
        viewports->at(i)->setPerspective(NULL);
        viewports->at(i)->setOrthographic(NULL);
    }

    objects->clear(all);
    models->clear(all);
    materials->clear(true);
    textures->clear(all);
    shaders->clear(all);
    //add default (need textures and shaders)
    materials->clear(all);
    animations->clear(all);
    fonts->clear(all);
}

bool Magic3D::ResourceManager::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new ResourceManager();
    }

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Resource Manager sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Resource Manager not started.");
    }

    return result;
}

bool Magic3D::ResourceManager::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Resource Manager sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Resource Manager not finished.");
    }

    return !instance;
}

Magic3D::ResourceManager* Magic3D::ResourceManager::getInstance()
{
    return instance;
}

Magic3D::ResourcesObject* Magic3D::ResourceManager::getObjects()
{
    return instance->objects;
}

Magic3D::ResourcesModel* Magic3D::ResourceManager::getModels()
{
    return instance->models;
}

Magic3D::ResourcesTexture* Magic3D::ResourceManager::getTextures()
{
    return instance->textures;
}

Magic3D::ResourcesShader* Magic3D::ResourceManager::getShaders()
{
    return instance->shaders;
}

Magic3D::ResourcesMaterial* Magic3D::ResourceManager::getMaterials()
{
    return instance->materials;
}

Magic3D::ResourcesAnimation* Magic3D::ResourceManager::getAnimations()
{
    return instance->animations;
}

Magic3D::ResourcesFont* Magic3D::ResourceManager::getFonts()
{
    return instance->fonts;
}

void Magic3D::ResourceManager::setPath(std::string path)
{
    std::string ending = "/";
    this->path = path;
    if (path.compare (path.length() - ending.length(), ending.length(), ending) != 0)
    {
        this->path += ending;
    }

    objects->setPath(&this->path);
    models->setPath(&this->path);
    textures->setPath(&this->path);
    shaders->setPath(&this->path);
    materials->setPath(&this->path);
    animations->setPath(&this->path);
    fonts->setPath(&this->path);
}

std::string Magic3D::ResourceManager::getPath()
{
    return path;
}

void Magic3D::ResourceManager::setUserPath(std::string path)
{
    std::string ending = "/";
    this->pathUser = path;
    if (path.compare (path.length() - ending.length(), ending.length(), ending) != 0)
    {
        this->pathUser += ending;
    }
}

std::string Magic3D::ResourceManager::getUserPath()
{
    return pathUser;
}

std::string Magic3D::ResourceManager::getScriptPath(std::string script)
{
    std::string fileName = getPath() + M3D_PATH_SCRIPT + script;

    return fileName;
}

Magic3D::Texture* Magic3D::ResourceManager::addTexture(std::string name, std::string file, bool mipmap, bool clamp, bool& created)
{
    return addTexture(eTEXTURE_DIFFUSE, name, file, mipmap, clamp, created);
}

Magic3D::Texture* Magic3D::ResourceManager::addTexture(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created)
{
    Texture* tex = textures->load(type, name, file, mipmap, clamp, created);
    return tex;
}

Magic3D::Shader* Magic3D::ResourceManager::addShader(std::string name, bool& created)
{
    Shader* shader = shaders->load(name, created);
    return shader;
}

Magic3D::Sprite* Magic3D::ResourceManager::addSprite(std::string name, bool& created)
{
    Sprite* sprite = (Sprite*)objects->get(name);
    created = !sprite;
    if (!sprite)
    {
        sprite = new Sprite(name);
        objects->add(sprite);
    }

    return sprite;
}

Magic3D::Particles* Magic3D::ResourceManager::addParticles(std::string name, bool& created)
{
    Particles* particles = (Particles*)objects->get(name);
    created = !particles;
    if (!particles)
    {
        particles = new Particles(name);
        objects->add(particles);
    }

    return particles;
}

Magic3D::GUIWindow* Magic3D::ResourceManager::addGUIWindow(std::string name, bool& created)
{
    GUIWindow* window = (GUIWindow*)objects->get(name);
    created = !window;
    if (!window)
    {
        window = new GUIWindow(name);
        objects->add(window);
    }

    return window;
}

Magic3D::GUIButton* Magic3D::ResourceManager::addGUIButton(std::string name, bool& created)
{
    GUIButton* button = (GUIButton*)objects->get(name);
    created = !button;
    if (!button)
    {
        button = new GUIButton(name);
        objects->add(button);
    }

    return button;
}

Magic3D::GUILabel* Magic3D::ResourceManager::addGUILabel(std::string name, int size, bool& created)
{
    GUILabel* label = (GUILabel*)objects->get(name);
    created = !label;
    if (!label)
    {
        label = new GUILabel(name, size);
        objects->add(label);
    }

    return label;
}

Magic3D::Joystick* Magic3D::ResourceManager::addJoystick(std::string name, bool& created)
{
    Joystick* joystick = (Joystick*)objects->get(name);
    created = !joystick;
    if (!joystick)
    {
        joystick = new Joystick(name);
        objects->add(joystick);
    }

    return joystick;
}

Magic3D::Camera* Magic3D::ResourceManager::addCamera(std::string name, bool& created)
{
    Camera* camera = (Camera*)objects->get(name);
    created = !camera;
    if (!camera)
    {
        camera = new Camera(name);
        objects->add(camera);
    }

    return camera;
}

Magic3D::Model* Magic3D::ResourceManager::addModel(std::string name, bool& created)
{
    Model* model = (Model*)objects->get(name);
    created = !model;
    if (!model)
    {
        model = new Model(name);
        objects->add(model);
    }

    return model;
}

Magic3D::ObjectInstance* Magic3D::ResourceManager::addInstance(std::string name, bool& created)
{
    ObjectInstance* instance = (ObjectInstance*)objects->get(name);
    created = !instance;
    if (!instance)
    {
        instance = new ObjectInstance(name);
        objects->add(instance);
    }

    return instance;
}

Magic3D::Light* Magic3D::ResourceManager::addLight(std::string name, bool& created)
{
    Light* light = (Light*)objects->get(name);
    created = !light;
    if (!light)
    {
        light = new Light(name);
        objects->add(light);
    }

    return light;
}

Magic3D::Sound* Magic3D::ResourceManager::addSound(std::string name, bool& created)
{
    Sound* sound = (Sound*)objects->get(name);
    created = !sound;
    if (!sound)
    {
        sound = new Sound(name);
        objects->add(sound);
    }

    return sound;
}
