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

#ifndef MAGIC3D_WATER_H
#define MAGIC3D_WATER_H

#include <magic3d/object.h>

namespace Magic3D
{

#define WATER_MESH_NAME               "water"

#define M3D_WATER_XML               "Water"
#define M3D_WATER_XML_SIZE          "size"
#define M3D_WATER_XML_LENGTH        "length"
#define M3D_WATER_XML_WIDTH         "width"
#define M3D_WATER_XML_TILE_LENGTH   "tile_length"
#define M3D_WATER_XML_TILE_WIDTH    "tile_width"


class Water : public Object
{
protected:
    Mesh* waterMesh;

    int length;
    int width;
    float tileLength;
    float tileWidth;

    bool needUpdate;

    void init();

    Texture* getWaterTexture();

    Water(const Water& water, std::string name);
private:    

public:
    Water(std::string name);
    Water(OBJECT type, std::string name);
    virtual ~Water();
    virtual void* spawn(std::string name) const;

    void generateWater();

    virtual bool updateMeshes();
    void addMaterial(Material* material);

    void setSize(int length, int width, float tileLength, float tileWidth);

    int getLength();
    int getWidth();
    float getTileLength();
    float getTileWidth();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_WATER_H
