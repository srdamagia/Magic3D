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

#ifndef MAGIC3D_RESOURCES_FONT_H
#define MAGIC3D_RESOURCES_FONT_H

#include <magic3d/resource/resources.h>

namespace Magic3D
{
#define M3D_PATH_FONT "font/"
#define M3D_FONT_FILE ".fnt"

#define M3D_DEFAULT_FONT "default"
#define M3D_DEFAULT_FONT_SIZE 16

struct FontChar
{
    FontChar()
    {
        x = 0.0f;
        y = 0.0f;
        width = 0.0f;
        height = 0.0f;
        offsetX = 0.0f;
        offsetY = 0.0f;
        advanceX = 0.0f;
        textureID = -1;
    }

    float x;
    float y;
    float width;
    float height;
    float offsetX;
    float offsetY;
    float advanceX;
    int textureID;

    std::map<int, float> kernel;
};

class Font
{
private:
    std::string name;

    std::vector<Texture*> textures;
    std::vector<FontChar> charMap;
    float lineHeight;
    float lineHeightPX;
    float lineOffsetY;
public:
    Font(std::string name);
    ~Font();

    const std::string& getName();
    FontChar* getChar(int index);
    float getLineHeight();
    float getLineHeightInPixels();
    float getLineOffsetY();
    Texture* getTexture(int index);
    float getTextureWidth();
    float getTextureHeight();

    void load();
};

//template <class M3D_Class>
class ResourcesFont : public Resources<Font>
{
public:
    ResourcesFont();
    virtual ~ResourcesFont();

    virtual bool remove(std::string name);

    Font* load(std::string name, bool& created);
    std::string getPath(std::string font);

    virtual void addDefault();
};

}

#endif // MAGIC3D_RESOURCES_FONT_H
