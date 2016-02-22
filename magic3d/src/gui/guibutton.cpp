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
#include <magic3d/object.h>

Magic3D::GUIButton::GUIButton(const GUIButton& guiButton, std::string name) : GUI(guiButton, name)
{
    this->sizeBorder = guiButton.sizeBorder;
    init();
}

Magic3D::GUIButton::GUIButton(std::string name) : GUI(eOBJECT_GUI_BUTTON, name)
{
    sizeBorder = 0.01f;
    width = 0.25f;
    height = 0.25f;

    init();
}

void Magic3D::GUIButton::init()
{
    for (int i = 0; i < 9; i++)
    {
        spriteMesh->getData()->addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z);
    }

    spriteMesh->getData()->createVbo();

    needUpdate = true;
}

Magic3D::GUIButton::~GUIButton()
{

}

void* Magic3D::GUIButton::spawn(std::string name) const
{
    return (new GUIButton(*this, name));
}

void Magic3D::GUIButton::setBorderSize(float size)
{
    sizeBorder = size;
    needUpdate = true;
}

float Magic3D::GUIButton::getBorderSize()
{
    return sizeBorder;
}

bool Magic3D::GUIButton::updateMeshes()
{
    float widthReal = width - sizeBorder * 2.0f;
    float heightReal = height - sizeBorder * 2.0f;

    float widthHalf  = width * 0.5f;
    float heightHalf = height * 0.5f;
    float startX     = -widthHalf;
    float startY     = -heightHalf;

    float texX = 0.0f;
    float texY = 1.0f;

    float size = sizeBorder;

    int index = 0;

    ColorRGBA color;
    Material* material = getMaterial();

    if (material)
    {
        color = material->getColorDiffuse();
    }

    MeshData* data = spriteMesh->getData();

    float* buffer = data->mapBuffer();

    // TOP **********************************************************************
    texX = textureX * 3.0f;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    startX += sizeBorder;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, widthReal, size, color);

    startX += widthReal;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    // AREA **********************************************************************
    startX = -widthHalf; startY += size;
    size = heightReal;
    texX = textureX * 3.0f; texY -= textureY;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    startX += sizeBorder;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, widthReal, size, color);

    startX += widthReal;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    // BOTTOM ********************************************************************
    startX = -widthHalf; startY += size;
    size = sizeBorder;
    texX = textureX * 3.0f; texY -= textureY;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    startX += sizeBorder;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, widthReal, size, color);

    startX += widthReal;
    texX += textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    data->unmapBuffer();

    return true;
}
