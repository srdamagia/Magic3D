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

#ifndef MAGIC3D_RESOURCES_MODEL_H
#define MAGIC3D_RESOURCES_MODEL_H

#include <magic3d/resource/resources.h>

namespace Magic3D
{
#define M3D_PLACEHOLDER "placeholder"
#define M3D_PATH_MODEL "model/"

class MeshInfo {
private:
    MeshData* data;
    Material* material;

    bool doubleSide;

    bool selfData;
    bool selfMaterial;
public:

    MeshInfo(MeshData* data, Material* material, bool doubleSide, bool selfData = true, bool selfMaterial = false);
    ~MeshInfo();

    const std::string& getName();

    MeshData* getData();
    Material* getMaterial();
    bool isDoubleSide();
};

class ModelInfo {
private:
    std::string name;

    std::vector<MeshInfo*> meshes;
    Skeleton* skeleton;
public:

    ModelInfo(std::string name);
    ~ModelInfo();

    const std::string& getName();

    void addMesh(MeshData* data, Material* material, bool doubleSide, bool selfData = true, bool selfMaterial = false);
    std::vector<MeshInfo*>* getMeshes();
    void setSkeleton(Skeleton* skeleton);
    Skeleton* getSkeleton();
};

//template <class M3D_Class>
class ResourcesModel : public Resources<ModelInfo>
{
private:
    MeshData* placeholder;
public:
    ResourcesModel();
    virtual ~ResourcesModel();

    virtual std::string getPath(std::string path);

    virtual void addDefault();
};
}

#endif // MAGIC3D_RESOURCES_MODEL_H
