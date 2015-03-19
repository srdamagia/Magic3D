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

#include <magic3d/model/loader_dae.h>
#include <magic3d/resource/resourcemngr.h>
#include <magic3d/scene.h>

Magic3D::Model::Model(const Model& model, std::string name) : Object(model, name)
{
    this->loader = NULL;

    this->fileName = model.fileName;

    this->box = model.box;

    this->uniqueUpdate = model.uniqueUpdate;

    this->skeleton = model.skeleton->spawn(this);
}

Magic3D::Model::Model(std::string name) : Object(eOBJECT_MODEL, eRENDER_3D, name)
{
    skeleton = new Skeleton(this, M3D_SKELETON_DEFAULT_NAME);
    load(ResourceManager::getModels()->get(M3D_PLACEHOLDER));

    loader = NULL;

    uniqueUpdate = false;
}

Magic3D::Model::~Model()
{
    if (skeleton)
    {
        delete skeleton;
        skeleton = NULL;
    }

    if (loader)
    {
        delete loader;
        loader = NULL;
    }
}

Magic3D::Model* Magic3D::Model::spawn(std::string name) const
{
    return (new Model(*this, name));
}

bool Magic3D::Model::update()
{
    bool needUpdate = Scene::getInstance()->getUniqueUpdateFlag() != uniqueUpdate;
    bool result = Object::update();
    if (needUpdate && (isInParentFrustum() || isInEffectFrustum()) && getSkeleton())
    {
        uniqueUpdate = !uniqueUpdate;
        result = result && getSkeleton()->update();
        if (result)
        {
            result = result && updateMeshes();
        }
        Object::updateBoundingBox(false);
        box = Object::getBoundingBox();
    }
    return result;
}

void Magic3D::Model::setFileName(std::string name)
{
    fileName = name;
}

const std::string& Magic3D::Model::getFileName()
{
    return fileName;
}

bool Magic3D::Model::load()
{
    bool result = false;
    if (loader)
    {
        if (!loader->load())
        {
            delete loader;
            loader = NULL;
        }
        else
        {
            result = true;
        }
    }
    else
    {
        setCollisionMeshIndex(-1);

        if (!fileName.empty())
        {
            ModelInfo* modelInfo = ResourceManager::getModels()->get(fileName);

            if (!modelInfo)
            {
                std::string lower = fileName;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                std::string ending = ".dae";
                if (lower.compare (lower.length() - ending.length(), ending.length(), ending) == 0)
                {
                    loader = new LoaderDAE();
                    loader->load(this);
                }
                else
                {
                    //LoaderOBJ.load(this);
                }
                result = true;
            }
            else
            {
                result = load(modelInfo);
            }
        }
    }

    return result;
}

bool Magic3D::Model::load(ModelInfo* modelInfo)
{
    bool result = false;
    if (modelInfo)
    {
        clearMeshes();
        std::vector<MeshInfo*>::const_iterator it_m = modelInfo->getMeshes()->begin();
        std::vector<MeshInfo*>::const_iterator it_end = modelInfo->getMeshes()->end();
        while (it_m != it_end)
        {
            MeshInfo* meshInfo = *it_m++;

            Mesh* mesh = new Mesh(meshInfo->getData(), meshInfo->isDoubleSide());

            addMesh(mesh);

            Magic3D::Material* material = meshInfo->getMaterial();
            if (!material)
            {
                material = ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL);
            }
            mesh->addMaterial(material);
        }

        if (modelInfo->getSkeleton())
        {
            skeleton->copyFrom(modelInfo->getSkeleton());
        }

        updateBoundingBox();
    }

    return result;
}

const Magic3D::Box& Magic3D::Model::getBoundingBox()
{
    return box;
}

void Magic3D::Model::updateBoundingBox()
{
    Object::updateBoundingBox(true);
    box = Object::getBoundingBox();
}

Magic3D::Skeleton* Magic3D::Model::getSkeleton(bool empty)
{
    return skeleton && (empty || skeleton->getBoneCount() > 0) ? skeleton : NULL;
}

Magic3D::XMLElement* Magic3D::Model::save(XMLElement* root)
{
    Object::save(root);

    if (root)
    {
        XMLElement* model = root->GetDocument()->NewElement(M3D_MODEL_XML);
        root->LinkEndChild(model);

        saveString(model, M3D_MODEL_XML_FILE, getFileName());

        if (getSkeleton())
        {
            getSkeleton()->save(model);
        }
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Model::load(XMLElement* root)
{
    if (root)
    {
        XMLElement* model = root->FirstChildElement(M3D_MODEL_XML);

        std::string name = loadString(model, M3D_MODEL_XML_FILE);
        if (name.compare(M3D_XML_NULL) != 0)
        {
            setFileName(name);
        }

        if (getSkeleton())
        {
            getSkeleton()->load(model);
        }
    }

    Object::load(root);

    updateBoundingBox();

    return root;
}
