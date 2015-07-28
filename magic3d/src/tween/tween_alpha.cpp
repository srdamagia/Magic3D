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

#include <magic3d/tween/tween_alpha.h>

#include <magic3d/object.h>
#include <magic3d/gui/guilabel.h>

Magic3D::TweenAlpha::TweenAlpha() : Tween(eTWEEN_ALPHA)
{
    alpha = 1.0f;
}

Magic3D::TweenAlpha::~TweenAlpha()
{

}

Magic3D::TweenAlpha* Magic3D::TweenAlpha::spawn()
{
    return (new TweenAlpha(*this));
}

void Magic3D::TweenAlpha::reset()
{
    Tween::reset();
}

void Magic3D::TweenAlpha::setAlpha(float alpha)
{
    this->alpha = alpha;
}

float Magic3D::TweenAlpha::getAlpha()
{
    return alpha;
}

void Magic3D::TweenAlpha::tween(float factor)
{
    if (getPhysicsObject()->getType() != eOBJECT_BONE)
    {        
        if (getPhysicsObject()->getType() == eOBJECT_GUI_LABEL)
        {
            GUILabel* label = static_cast<GUILabel*>(getPhysicsObject());

            ColorRGBA color = label->getTextColor();
            color.a = factor * alpha;

            label->setTextColor(color);
        }
        else
        {
            Object* object = static_cast<Object*>(getPhysicsObject());
            std::vector<Mesh*>* meshes = object->getMeshes();
            std::vector<Mesh*>::const_iterator it_m = meshes->begin();

            while (it_m != meshes->end())
            {
                Mesh* mesh = *it_m++;
                std::vector<Material*>* materials = mesh->getMaterials();
                std::vector<Material*>::const_iterator it_mat = materials->begin();

                while (it_mat != materials->end())
                {
                    Material* material = *it_mat++;

                    ColorRGBA color = material->getColorDiffuse();
                    color.a = factor * alpha;

                    material->setColorDiffuse(color);
                }
            }
        }
    }
}

Magic3D::XMLElement* Magic3D::TweenAlpha::save(XMLElement* root)
{
    XMLElement* tween = Tween::save(root);
    if (tween)
    {
        saveFloat(tween, M3D_TWEEN_XML_ALPHA, alpha);
    }
    return tween;
}

Magic3D::XMLElement* Magic3D::TweenAlpha::load(XMLElement* root)
{
    Tween::load(root);
    if (root)
    {
        alpha = loadFloat(root, M3D_TWEEN_XML_ALPHA);
    }
    return root;
}
