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

#include <magic3d/tween/tween_scale.h>

Magic3D::TweenScale::TweenScale() : Tween(eTWEEN_SCALE)
{
    scale = Vector3(1.0f, 1.0f, 1.0f);
    startScale = scale;
}

Magic3D::TweenScale::~TweenScale()
{

}

Magic3D::TweenScale* Magic3D::TweenScale::spawn()
{
    return (new TweenScale(*this));
}

void Magic3D::TweenScale::reset()
{
    Tween::reset();

    startScale = getPhysicsObject()->getScale();
}

void Magic3D::TweenScale::setScale(Vector3 scale)
{
    this->scale = scale;
}

const Magic3D::Vector3& Magic3D::TweenScale::getScale()
{
    return scale;
}

void Magic3D::TweenScale::tween(float factor)
{
    getPhysicsObject()->setScale(startScale + (scale * factor));
}

Magic3D::XMLElement* Magic3D::TweenScale::save(XMLElement* root)
{
    XMLElement* tween = Tween::save(root);
    if (tween)
    {
        saveVector3(tween, M3D_TWEEN_XML_SCALE, scale);
    }
    return tween;
}

Magic3D::XMLElement* Magic3D::TweenScale::load(XMLElement* root)
{
    Tween::load(root);
    if (root)
    {
        scale = loadVector3(root, M3D_TWEEN_XML_SCALE);
    }
    return root;
}
