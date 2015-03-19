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

#include <magic3d/tween/tween_translate.h>

Magic3D::TweenTranslate::TweenTranslate() : Tween(eTWEEN_TRANSLATE)
{
    position = Vector3(1.0f, 1.0f, 1.0f);
    startPosition = position;
}

Magic3D::TweenTranslate::~TweenTranslate()
{

}

Magic3D::TweenTranslate* Magic3D::TweenTranslate::spawn()
{
    return (new TweenTranslate(*this));
}

void Magic3D::TweenTranslate::reset()
{
    Tween::reset();

    startPosition = getPhysicsObject()->getPosition();
}

void Magic3D::TweenTranslate::setPosition(Vector3 position)
{
    this->position = position;
}

const Magic3D::Vector3& Magic3D::TweenTranslate::getPosition()
{
    return position;
}

void Magic3D::TweenTranslate::tween(float factor)
{
    getPhysicsObject()->setPosition(startPosition + (position * factor));
}

Magic3D::XMLElement* Magic3D::TweenTranslate::save(XMLElement* root)
{
    XMLElement* tween = Tween::save(root);
    if (tween)
    {
        saveVector3(tween, M3D_TWEEN_XML_TRANSLATE, position);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::TweenTranslate::load(XMLElement* root)
{
    Tween::load(root);
    if (root)
    {
        position = loadVector3(root, M3D_TWEEN_XML_TRANSLATE);
    }
    return root;
}
