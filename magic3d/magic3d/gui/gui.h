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

#ifndef MAGIC3D_GUI_H
#define MAGIC3D_GUI_H

#include <magic3d/sprite.h>

namespace Magic3D
{

class GUI : public Sprite
{
protected:
    float textureX;
    float textureHalfX;
    float textureDoubleX;

    float textureY;
    float textureHalfY;
    float textureDoubleY;

    float pixelX;
    float pixelY;

    float texAreaX;
    float texAreaY;

private:
    bool  focused;

protected:
    GUI(const GUI& gui, std::string name);

public:
    GUI(OBJECT type, std::string name);
    virtual ~GUI();
    virtual void* spawn(std::string name) const;

    Material* getMaterial();

    virtual const Magic3D::Box& getBoundingBox();
    virtual void updateBoundingBox();
};

}

#endif // MAGIC3D_GUI_H
