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

#include <magic3d/resource/resourcemngr.h>
#include <magic3d/gui/gui.h>

Magic3D::GUI::GUI(const GUI& gui, std::string name) : Sprite(gui, name)
{
    this->textureX = gui.textureX;
    this->textureHalfX = gui.textureHalfX;
    this->textureDoubleX = gui.textureDoubleX;

    this->textureY = gui.textureY;
    this->textureHalfY = gui.textureHalfY;
    this->textureDoubleY = gui.textureDoubleY;

    this->pixelX = gui.pixelX;
    this->pixelY = gui.pixelY;

    this->texAreaX = gui.texAreaX;
    this->texAreaY = gui.texAreaY;

    this->focused = gui.focused;
}

Magic3D::GUI::GUI(OBJECT type, std::string name) : Sprite(type, name)
{
    switch (type)
    {        
        case eOBJECT_JOYSTICK: addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_PARTICLES)); break;
        default: addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_GUI)); break;
    }

    float col = 16.0f;
    float row = 16.0f;

    textureX = 1.0f / col;
    textureHalfX = textureX * 0.5f;
    textureDoubleX = textureX * 2.0f;

    textureY = 1.0f / row;
    textureHalfY = textureY * 0.5f;
    textureDoubleY = textureY * 2.0f;

    pixelX = textureX / col;
    pixelY = textureY / row;

    texAreaX = textureX - 2.0f * pixelX;
    texAreaY = textureY - 2.0f * pixelY;
}

Magic3D::GUI::~GUI()
{

}

void* Magic3D::GUI::spawn(std::string name) const
{
    return (new GUI(*this, name));
}

Magic3D::Material* Magic3D::GUI::getMaterial()
{
    Material* material = NULL;
    if (getMeshes()->size() > 0 && (getMeshes()->at(0) && getMeshes()->at(0)->getMaterials()->size()) > 0)
    {
        material = getMeshes()->at(0)->getMaterials()->at(0);
    }

    return material;
}

const Magic3D::Box& Magic3D::GUI::getBoundingBox()
{
    updateBoundingBox();
    return box;
}

void Magic3D::GUI::updateBoundingBox()
{
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    box = Box(Vector3(-halfWidth, -halfHeight, 0.0f), Vector3(halfWidth, halfHeight, 0.0f));
}
