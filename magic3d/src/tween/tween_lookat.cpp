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

#include <magic3d/tween/tween_lookat.h>

Magic3D::TweenLookAt::TweenLookAt() : Tween(eTWEEN_LOOKAT)
{
    position = Vector3(0.0f, 0.0f, 0.0f);

    startRotation = Quaternion::identity();
}

Magic3D::TweenLookAt::~TweenLookAt()
{
}

Magic3D::TweenLookAt* Magic3D::TweenLookAt::spawn()
{
    return (new TweenLookAt(*this));
}

void Magic3D::TweenLookAt::reset()
{
    Tween::reset();

    startRotation = getPhysicsObject()->getRotation();
}

void Magic3D::TweenLookAt::setPosition(Vector3 position)
{
    this->position = position;
}

const Magic3D::Vector3& Magic3D::TweenLookAt::getPosition()
{
    return position;
}

void Magic3D::TweenLookAt::tween(float factor)
{
    getPhysicsObject()->setRotation(Math::quaternion(position * factor) * startRotation);
}

Magic3D::XMLElement* Magic3D::TweenLookAt::save(XMLElement* root)
{
    XMLElement* tween = Tween::save(root);
    if (tween)
    {
        saveVector3(tween, M3D_TWEEN_XML_LOOKAT, position);
    }
    return tween;
}

Magic3D::XMLElement* Magic3D::TweenLookAt::load(XMLElement* root)
{
    Tween::load(root);
    if (root)
    {
        position = loadVector3(root, M3D_TWEEN_XML_LOOKAT);
    }
    return root;
}
