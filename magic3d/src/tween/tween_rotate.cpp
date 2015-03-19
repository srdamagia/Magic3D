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

#include <magic3d/tween/tween_rotate.h>

Magic3D::TweenRotate::TweenRotate() : Tween(eTWEEN_ROTATE)
{
    euler = Vector3(0.0f, 0.0f, 1.0f);

    startRotation = Quaternion::identity();
}

Magic3D::TweenRotate::~TweenRotate()
{
}

Magic3D::TweenRotate* Magic3D::TweenRotate::spawn()
{
    return (new TweenRotate(*this));
}

void Magic3D::TweenRotate::reset()
{
    Tween::reset();

    startRotation = getPhysicsObject()->getRotation();
}

void Magic3D::TweenRotate::setEuler(Vector3 euler)
{
    this->euler = euler;
}

const Magic3D::Vector3& Magic3D::TweenRotate::getEuler()
{
    return euler;
}

void Magic3D::TweenRotate::tween(float factor)
{
    getPhysicsObject()->setRotation(Math::quaternion(euler * factor) * startRotation);
}

Magic3D::XMLElement* Magic3D::TweenRotate::save(XMLElement* root)
{
    XMLElement* tween = Tween::save(root);
    if (tween)
    {
        saveVector3(tween, M3D_TWEEN_XML_ROTATE, euler);
    }
    return tween;
}

Magic3D::XMLElement* Magic3D::TweenRotate::load(XMLElement* root)
{
    Tween::load(root);
    if (root)
    {
        euler = loadVector3(root, M3D_TWEEN_XML_ROTATE);
    }
    return root;
}
