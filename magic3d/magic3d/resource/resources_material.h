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

#ifndef MAGIC3D_RESOURCES_MATERIAL_H
#define MAGIC3D_RESOURCES_MATERIAL_H

#include <magic3d/resource/resources.h>

namespace Magic3D
{
#define M3D_PATH_MATERIAL "material/"
#define M3D_MATERIAL_FILE ".material"

#define M3D_DEFAULT_MATERIAL_SCREEN "screen"
#define M3D_DEFAULT_MATERIAL "default"
#define M3D_DEFAULT_MATERIAL_GUI "gui"
#define M3D_DEFAULT_MATERIAL_FONT "font"
#define M3D_DEFAULT_MATERIAL_LENSFLARE "lensflare"
#define M3D_DEFAULT_MATERIAL_PARTICLES "particles"
#define M3D_DEFAULT_MATERIAL_LOADING_FONT "loading_font"
#define M3D_DEFAULT_MATERIAL_LOADING_SPRITE "loading_sprite"
#define M3D_DEFAULT_MATERIAL_TRUNK "trunk"
#define M3D_DEFAULT_MATERIAL_TWIG "twig"
#define M3D_DEFAULT_MATERIAL_GRASS "grass"
#define M3D_DEFAULT_MATERIAL_WATER "water"
#define M3D_DEFAULT_MATERIAL_TERRAIN "terrain"


//template <class M3D_Class>
class ResourcesMaterial : public Resources<Material>
{
public:
    ResourcesMaterial();
    virtual ~ResourcesMaterial();

    virtual bool remove(std::string name);

    Material* load(std::string name, bool& created);
    std::string getPath(std::string material);

    virtual void addDefault();
};

}

#endif // MAGIC3D_RESOURCES_MATERIAL_H
