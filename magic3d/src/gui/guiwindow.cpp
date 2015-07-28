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

Magic3D::GUIWindow::GUIWindow(const GUIWindow& guiWindow, std::string name) : GUI(guiWindow, name)
{
    this->sizeBorder = guiWindow.sizeBorder;
    this->sizeTitle = guiWindow.sizeTitle;
    init();
}

Magic3D::GUIWindow::GUIWindow(std::string name) : GUI(eOBJECT_GUI_WINDOW, name)
{
    sizeBorder = 0.01f;
    sizeTitle  = 0.05f;
    width = 0.5f;
    height = 0.5f;

    init();
}

void Magic3D::GUIWindow::init()
{
    for (int i = 0; i < 15; i++)
    {
        spriteMesh->getData()->addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z);
    }

    spriteMesh->getData()->createVbo();

    needUpdate = true;
}

Magic3D::GUIWindow::~GUIWindow()
{

}

Magic3D::GUIWindow* Magic3D::GUIWindow::spawn(std::string name) const
{
    return (new GUIWindow(*this, name));
}

void Magic3D::GUIWindow::setBorderSize(float size)
{
    sizeBorder = size;
    needUpdate = true;
}

float Magic3D::GUIWindow::getBorderSize()
{
    return sizeBorder;
}

void Magic3D::GUIWindow::setTitleSize(float size)
{
    sizeTitle = size;
    needUpdate = true;
}

float Magic3D::GUIWindow::getTitleSize()
{
    return sizeTitle;
}

bool Magic3D::GUIWindow::updateMeshes()
{    
    float widthReal = width - sizeBorder * 2.0f;
    float heightReal = height - sizeBorder * 3.0f - sizeTitle;

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

    // TTILE ********************************************************************
    startX = -widthHalf; startY += size;
    size = sizeTitle;
    texX = textureX * 4.0f; texY -= textureDoubleY + textureY;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);

    startX += sizeBorder;
    texX -= textureX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, widthReal, size, color);

    startX += widthReal;
    texX += textureDoubleX;
    data->setQuadTexture(buffer, index, texX + pixelX, texY - pixelY, texAreaX, texAreaY);
    data->setQuad(buffer, index++, startX, startY, sizeBorder, size, color);
    texY += textureDoubleY + textureY;

    // MIDDLE ********************************************************************
    startX = -widthHalf; startY += size;
    size = sizeBorder;
    texX = 0.0f; texY -= textureY;
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
    texX = 0.0f; texY -= textureY;
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
    texX = 0.0f; texY -= textureY;
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
