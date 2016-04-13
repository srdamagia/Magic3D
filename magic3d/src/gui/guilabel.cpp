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

#include <magic3d/magic3d.h>
#include <magic3d/object.h>
#include <magic3d/text.h>

Magic3D::GUILabel::GUILabel(const GUILabel& guiLabel, std::string name) : GUI(guiLabel, name)
{
    this->spriteMesh = getMeshes()->at(0);
    //this->addMesh(spriteMesh);
    needUpdate = true;
}

Magic3D::GUILabel::GUILabel(std::string name, float size) : GUI(eOBJECT_GUI_LABEL, name)
{
    this->spriteMesh = new Mesh(new TextData(name, size), false, true);
    this->spriteMesh->setIlluminated(false);
    this->spriteMesh->setCastShadows(false);
    this->spriteMesh->setReceiveShadows(false);
    this->spriteMesh->setFogged(false);
    this->spriteMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_FONT));
    addMesh(spriteMesh);
    needUpdate = true;
}

Magic3D::GUILabel::~GUILabel()
{
    spriteMesh = NULL;
}

void* Magic3D::GUILabel::spawn(std::string name) const
{
    return (new GUILabel(*this, name));
}

Magic3D::TextData* Magic3D::GUILabel::getText()
{
    return static_cast<TextData*>(spriteMesh->getData());
}

bool Magic3D::GUILabel::updateMeshes()
{
    bool result = true;
    if (getText()->isChanged())
    {
        result = getText()->update(this);
        width = getText()->getWidth();
        height = getText()->getHeight();
    }

    return result;
}

Magic3D::XMLElement* Magic3D::GUILabel::save(XMLElement* root)
{
    Sprite::save(root);
    getText()->save(root);
    return root;
}

Magic3D::XMLElement* Magic3D::GUILabel::load(XMLElement* root)
{
    Sprite::load(root);
    getText()->load(root);
    return root;
}
