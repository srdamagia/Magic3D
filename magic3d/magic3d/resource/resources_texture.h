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

#ifndef MAGIC3D_RESOURCES_TEXTURE_H
#define MAGIC3D_RESOURCES_TEXTURE_H

#include <magic3d/resource/resources.h>
#include <magic3d/sprite.h>

namespace Magic3D
{
#define M3D_SPRITE_FILE ".sprite"
#define M3D_DEFAULT_TEXTURE_NAME "placeholder_texture"
#define M3D_DEFAULT_TEXTURE "placeholder.png"

#define M3D_DEFAULT_PARTICLES_NAME "particles_texture"
#define M3D_DEFAULT_PARTICLES "particles.png"

#define M3D_DEFAULT_THEME_NAME "gui_theme"
#define M3D_DEFAULT_THEME "gui.png"

#define M3D_DEFAULT_PROGRESS_NAME "loading_progress"
#define M3D_DEFAULT_PROGRESS "progress.png"

#define M3D_DEFAULT_TRUNKS_NAME "trunks"
#define M3D_DEFAULT_TRUNKS "trunks.png"

#define M3D_DEFAULT_TWIGS_NAME "twigs"
#define M3D_DEFAULT_TWIGS "twigs.png"

#define M3D_DEFAULT_GRASS_NAME "grass"
#define M3D_DEFAULT_GRASS "grass.png"

#define M3D_DEFAULT_WATER_NAME "water"
#define M3D_DEFAULT_WATER "water.png"

#define M3D_DEFAULT_WATER_NORMAL_NAME "water_normal"
#define M3D_DEFAULT_WATER_NORMAL "water_normal.png"

#define M3D_DEFAULT_TERRAIN_NAME "terrain"
#define M3D_DEFAULT_TERRAIN "terrain.png"

#define M3D_PATH_TEXTURE "texture/"
#define M3D_PATH_GUI "gui/"

enum TEXTURE
{
    eTEXTURE_DIFFUSE,
    eTEXTURE_GUI,
    eTEXTURE_FONT,
    eTEXTURE_FBO
};

class Texture
{
private:
    std::string name;

    std::string getSpriteFile();
public:
    std::vector<SpriteSequence*> sequences;

    std::string file;

    unsigned int id;

    int width;
    int height;

    TEXTURE type;

    bool mipmap;
    bool clamp;

    bool needSave;

    Texture(std::string name, unsigned int id, TEXTURE type, int width, int height, bool mipmap, bool clamp, std::string file);
    ~Texture();

    const std::string& getName();

    static Texture* load(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created);
    void setWrap(bool clamp);

    void saveSprite();
    void loadSprite();
};

//template <class M3D_Class>
class ResourcesTexture : public Resources<Texture>
{
public:
    ResourcesTexture();
    virtual ~ResourcesTexture();

    Texture* load(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created);

    std::string getPrefix(TEXTURE type);

    std::string getTexturePath(TEXTURE type, std::string texture);

    virtual void addDefault();

    static tinyxml2::XMLElement* saveTexture(XMLElement* root, const char* name, Texture* texture);
    static Texture* loadTexture(XMLElement* root);
};

}

#endif // MAGIC3D_RESOURCES_TEXTURE_H
