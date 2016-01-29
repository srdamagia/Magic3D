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

#ifndef MAGIC3D_RESOURCEMNGR_H
#define MAGIC3D_RESOURCEMNGR_H

#include <magic3d/camera.h>
#include <magic3d/particles.h>
#include <magic3d/light.h>
#include <magic3d/model/model.h>
#include <magic3d/gui/guiwindow.h>
#include <magic3d/gui/guibutton.h>
#include <magic3d/gui/guilabel.h>
#include <magic3d/gui/joystick.h>
#include <magic3d/sound/sound.h>
#include <magic3d/terrain.h>
#include <magic3d/water.h>
#include <magic3d/vegetation.h>

#include <magic3d/resource/resources_object.h>
#include <magic3d/resource/resources_model.h>
#include <magic3d/resource/resources_texture.h>
#include <magic3d/resource/resources_shader.h>
#include <magic3d/resource/resources_material.h>
#include <magic3d/resource/resources_animation.h>
#include <magic3d/resource/resources_font.h>


namespace Magic3D
{
#define M3D_PATH_DEFAULT "data/"
#define M3D_PATH_SOUND "sound/"
#define M3D_PATH_PARTICLES "particles/"
#define M3D_PATH_SCRIPT "script/"

class Package;
class DataBuffer;
class Object;
class ResourceManager
{
private:
    Package* package;
    ResourcesObject* objects;
    ResourcesModel* models;
    ResourcesTexture* textures;
    ResourcesShader* shaders;
    ResourcesMaterial* materials;
    ResourcesAnimation* animations;
    ResourcesFont* fonts;

    static ResourceManager* instance;

    std::string path;
    std::string pathUser;

    ResourceManager();
    ~ResourceManager();
public:
    static bool start();
    static bool finish();

    void clear(bool all = false);

    static ResourceManager* getInstance();

    static ResourcesObject* getObjects();
    static ResourcesModel* getModels();
    static ResourcesTexture* getTextures();
    static ResourcesShader* getShaders();
    static ResourcesMaterial* getMaterials();
    static ResourcesAnimation* getAnimations();
    static ResourcesFont* getFonts();

    void setPath(std::string path, bool dataFile);
    std::string getPath();

    Package* getPackage();
    bool unpack(std::string fileName, DataBuffer* buffer);

    void setUserPath(std::string path);
    std::string getUserPath();

    std::string getScriptPath(std::string script);

    Texture* addTexture(std::string name, std::string file, bool mipmap, bool clamp, bool& created);
    Texture* addTexture(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created);
    Shader* addShader(std::string name, bool& created);
    Sprite* addSprite(std::string name, bool& created);
    Particles* addParticles(std::string name, bool& created);
    GUIWindow* addGUIWindow(std::string name, bool& created);
    GUIButton* addGUIButton(std::string name, bool& created);
    GUILabel* addGUILabel(std::string name, int size, bool& created);
    Joystick* addJoystick(std::string name, bool& created);
    Camera* addCamera(std::string name, bool& created);
    Model* addModel(std::string name, bool& created);
    Terrain* addTerrain(std::string name, bool& created);
    Water* addWater(std::string name, bool& created);
    Vegetation* addVegetation(std::string name, bool& created);
    ObjectInstance* addInstance(std::string name, bool& created);
    Light* addLight(std::string name, bool& created);
    Sound* addSound(std::string name, bool& created);
};

}

#endif // MAGIC3D_RESOURCEMNGR_H
