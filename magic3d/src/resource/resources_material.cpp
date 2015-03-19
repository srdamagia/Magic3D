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
#include <magic3d/resource/resources_material.h>

Magic3D::ResourcesMaterial::ResourcesMaterial() : Resources<Material>()
{
    resourceName = M3D_RESOURCE_MATERIAL;
}

Magic3D::ResourcesMaterial::~ResourcesMaterial()
{

}

bool Magic3D::ResourcesMaterial::remove(std::string name)
{
    bool result = false;
    Material* material = Resources<Material>::get(name);
    Resources<Material>::resources.erase(name);
    if (material)
    {
        typename std::map<std::string, Object*>::const_iterator it_o = ResourceManager::getObjects()->begin();

        while (it_o != ResourceManager::getObjects()->end())
        {
            Object* child = (*it_o++).second;

            int size = child->getMeshes()->size();
            for (int i = 0; i < size; i++)
            {
                Mesh* mesh = child->getMeshes()->at(i);

                int s = mesh->getMaterials()->size() - 1;
                for (int m = s; m >= 0 ; m--)
                {
                    if (mesh->getMaterials()->at(m) == material)
                    {
                        mesh->getMaterials()->erase(mesh->getMaterials()->begin() + m);
                    }
                }
            }
        }

        delete material;

        result = true;
    }

    return result;
}

Magic3D::Material* Magic3D::ResourcesMaterial::load(std::string name, bool& created)
{
    created = false;
    Material* result = Resources<Material>::get(name);
    if (!result)
    {
        result = new Material(name);
        if (!result->load())
        {
            created = true;
            result->save();
        }
        add(result);
    }
    return result;
}

std::string Magic3D::ResourcesMaterial::getPath(std::string material)
{
    std::string fileName = Resources<Material>::getPath() + M3D_PATH_MATERIAL + material;

    return fileName;
}

void Magic3D::ResourcesMaterial::addDefault()
{
    bool created = false;
    Material* material = load(M3D_DEFAULT_MATERIAL_SCREEN, created);
    Renderer::getInstance()->setMaterialScreen(material);

    material = load(M3D_DEFAULT_MATERIAL, created);
    Renderer::getInstance()->setMaterialDefault(material);

    load(M3D_DEFAULT_MATERIAL_GUI, created);
    load(M3D_DEFAULT_MATERIAL_FONT, created);
    load(M3D_DEFAULT_MATERIAL_LENSFLARE, created);
    load(M3D_DEFAULT_MATERIAL_PARTICLES, created);
    load(M3D_DEFAULT_MATERIAL_LOADING_FONT, created);
    load(M3D_DEFAULT_MATERIAL_LOADING_SPRITE, created);
}
