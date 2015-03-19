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
#include <magic3d/resource/resources_model.h>


Magic3D::MeshInfo::MeshInfo(MeshData* data, Material* material, bool doubleSide, bool selfData, bool selfMaterial)
{
    this->data = data;
    this->material = material;
    this->doubleSide = doubleSide;

    this->selfData = selfData;
    this->selfMaterial = selfMaterial;
}

Magic3D::MeshInfo::~MeshInfo()
{
    if (data && selfData)
    {
        delete data;
    }
    data = NULL;

    if (material && selfMaterial)
    {
        delete material;
    }
    material = NULL;
}

const std::string& Magic3D::MeshInfo::getName()
{
    return data->getName();
}

Magic3D::MeshData* Magic3D::MeshInfo::getData()
{
    return data;
}

Magic3D::Material* Magic3D::MeshInfo::getMaterial()
{
    return material;
}

bool Magic3D::MeshInfo::isDoubleSide()
{
    return doubleSide;
}

//******************************************************************************

Magic3D::ModelInfo::ModelInfo(std::string name)
{
    this->name = name;
    skeleton = NULL;
}

Magic3D::ModelInfo::~ModelInfo()
{
    while (!meshes.empty()) {
        MeshInfo* meshInfo = *meshes.begin();
        meshes.erase(meshes.begin());
        if (meshInfo)
        {
            delete meshInfo;
        }
    }

    if (skeleton)
    {
        delete skeleton;
        skeleton = NULL;
    }
}

const std::string& Magic3D::ModelInfo::getName()
{
    return name;
}

void Magic3D::ModelInfo::addMesh(MeshData* data, Material* material, bool doubleSide, bool selfData, bool selfMaterial)
{
    if (data)
    {
        meshes.push_back(new MeshInfo(data, material, doubleSide, selfData, selfMaterial));
    }
}

std::vector<Magic3D::MeshInfo*>* Magic3D::ModelInfo::getMeshes()
{
    return &meshes;
}

void Magic3D::ModelInfo::setSkeleton(Skeleton* skeleton)
{
    this->skeleton = skeleton;
}

Magic3D::Skeleton* Magic3D::ModelInfo::getSkeleton()
{
    return skeleton;
}

//******************************************************************************

Magic3D::ResourcesModel::ResourcesModel() : Resources<ModelInfo>()
{
    resourceName = M3D_RESOURCE_MODEL;

    placeholder = new MeshData(eMESH_TRIANGLES, M3D_PLACEHOLDER);

    placeholder->addQuad(0.0f, 0.0f, -0.5f, -1.0f, -1.0f, eAXIS_Z);
    placeholder->addQuad(0.0f, 0.0f,  0.5f,  1.0f, -1.0f, eAXIS_Z);

    placeholder->addQuad(-0.5f, 0.0f, 0.0f,  1.0f, -1.0f, eAXIS_X);
    placeholder->addQuad( 0.5f, 0.0f, 0.0f, -1.0f, -1.0f, eAXIS_X);

    placeholder->addQuad(0.0f, -0.5f, 0.0f,  1.0f, -1.0f, eAXIS_Y);
    placeholder->addQuad(0.0f,  0.5f, 0.0f,  1.0f,  1.0f, eAXIS_Y);
}

Magic3D::ResourcesModel::~ResourcesModel()
{
    if (placeholder)
    {
        delete placeholder;
        placeholder = NULL;
    }
}

std::string Magic3D::ResourcesModel::getPath(std::string model)
{
    std::string fileName = Resources<ModelInfo>::getPath() + M3D_PATH_MODEL + model;

    return fileName;
}

void Magic3D::ResourcesModel::addDefault()
{
    ModelInfo* modelInfo = new ModelInfo(M3D_PLACEHOLDER);
    modelInfo->addMesh(placeholder, NULL, false, false);
    add(modelInfo);
}
