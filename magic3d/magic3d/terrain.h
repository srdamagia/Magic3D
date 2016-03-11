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

#ifndef MAGIC3D_TERRAIN_H
#define MAGIC3D_TERRAIN_H

#include <magic3d/object.h>

namespace Magic3D
{

#define TERRAIN_FILE                  ".terrain"
#define TERRAIN_MESH_NAME             "terrain"

#define M3D_TERRAIN_XML               "Terrain"
#define M3D_TERRAIN_XML_SIZE          "size"
#define M3D_TERRAIN_XML_LENGTH        "length"
#define M3D_TERRAIN_XML_WIDTH         "width"
#define M3D_TERRAIN_XML_TILE_LENGTH   "tile_length"
#define M3D_TERRAIN_XML_TILE_WIDTH    "tile_width"
#define M3D_TERRAIN_XML_HEIGHT_MAP    "height_map"

#define TERRAIN_HEIGHT_MIN            -256.0f
#define TERRAIN_HEIGHT_MAX            768.0f
#define TERRAIN_COLOR_PACK            256.0f


class Terrain : public Object
{
protected:
    Mesh* terrainMesh;

    std::string heightMap;

    float* bufferData;
    int bufferDataSize;

    vindex* bufferIndex;
    int bufferIndexSize;

    int length;
    int width;
    float tileLength;
    float tileWidth;
    float textureLevels;
    float textureTile;

    bool needUpdate;

    void init();

    Texture* getTerrainTexture();

    Terrain(const Terrain& terrain, std::string name);
private:
    void generateTerrain();

public:
    Terrain(std::string name);
    Terrain(OBJECT type, std::string name);
    virtual ~Terrain();
    virtual void* spawn(std::string name) const;

    virtual bool updateMeshes();
    void addMaterial(Material* material);

    void setSize(int length, int width, float tileLength, float tileWidth);

    int getLength();
    int getWidth();
    float getTileLength();
    float getTileWidth();

    void setHeight(float x, float z, float height);
    float getHeight(float x, float z);
    float getHeight(int index);

    void modify(float x, float z, float radius, float height, bool flatten, bool square);
    void finishModify();

    void setHeightMap(std::string heightMap);
    const std::string& getHeightMap();
    void loadFromHeightMap();
    int saveToHeightMap(std::string name);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_TERRAIN_H
