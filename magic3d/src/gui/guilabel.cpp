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

Magic3D::GUILabel::GUILabel(const GUILabel& guiLabel, std::string name) : GUI(guiLabel, name)
{
    this->text = guiLabel.text;
    this->lastReturns = guiLabel.lastReturns;
    this->font = guiLabel.font;
    this->textSize = guiLabel.textSize;
    this->textAlignment = guiLabel.textAlignment;
    this->textColor = guiLabel.textColor;

    needUpdate = true;
}

Magic3D::GUILabel::GUILabel(std::string name, float size) : GUI(eOBJECT_GUI_LABEL, name)
{
    text = " ";
    lastReturns = 0;
    this->font = NULL;
    this->textSize = size;
    this->textAlignment = eHORIZONTAL_ALIGN_LEFT;
    this->textColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);

    needUpdate = true;
}

Magic3D::GUILabel::~GUILabel()
{
    font = NULL;
}

Magic3D::GUILabel* Magic3D::GUILabel::spawn(std::string name) const
{
    return (new GUILabel(*this, name));
}

void Magic3D::GUILabel::setFont(Font* font)
{
    this->font = font;
}

Magic3D::Font* Magic3D::GUILabel::getFont()
{
    if (!font)
    {
        font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
    }

    return font;
}

void Magic3D::GUILabel::setTextSize(float size)
{
    this->textSize = size;
    needUpdate = true;
}

float Magic3D::GUILabel::getTextSize()
{
    return textSize;
}

void Magic3D::GUILabel::setTextAlignment(HORIZONTAL_ALIGN alignment)
{
    textAlignment = alignment;
}

Magic3D::HORIZONTAL_ALIGN Magic3D::GUILabel::getTextAlignment()
{
    return textAlignment;
}

void Magic3D::GUILabel::setTextColor(ColorRGBA color)
{
    this->textColor = color;
}

const Magic3D::ColorRGBA& Magic3D::GUILabel::getTextColor()
{
    return textColor;
}

void Magic3D::GUILabel::setText(std::string text)
{
    unsigned int size = this->text.length();
    this->text = text;

    if (this->text.length() != size)
    {
        spriteMesh->getData()->clear();
    }
    needUpdate = true;
}

std::string Magic3D::GUILabel::getText()
{
    return text;
}

bool Magic3D::GUILabel::updateMeshes()
{
    Renderer* renderer = Renderer::getInstance();
    MeshData* data = spriteMesh->getData();
    Font* font = getFont();
    std::vector<float> lines;

    if (!font)
    {
        return false;
    }

    float fsize = (textSize / 512.0f) / font->getLineHeightInPixels();
    float lineHeight = fsize * (font->getLineHeightInPixels() + 2.0f);

    int quad = -1;
    int quadCount = data->getVerticesCount() / 4;

    if (quadCount < (int)text.size() - lastReturns)
    {
        lastReturns = 0;
        for (unsigned int i = 0; i < text.size(); i++)
        {
            if (text.at(i) == '\n')
            {
                lastReturns++;
                continue;
            }
            else if (renderer->hasMapBuffer() || (!renderer->hasMapBuffer() && (int)i >= quadCount))
            {
                data->addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z, false);
            }
        }

        data->createVbo();
        quadCount = data->getVerticesCount() / 4;
    }

    width = 0.0f;
    height = lineHeight;

    float startX = 0.0f;
    float startY = 0.0f;

    if (data->getVerticesCount() > 0)
    {
        float* buffer = data->mapBuffer();

        int lastIndex = 0;
        for (unsigned int i = 0; i < text.size(); i++)
        {
            int index = text.at(i);

            if (index < 0)
            {
                index = 256 + index;
            }

            FontChar* fchar = font->getChar(index);            

            if (text.at(i) == '\n')
            {
                lines.push_back(startX);
                if (width < startX)
                {
                    width = startX;
                }
                startX = 0.0f;
                startY += lineHeight;
                height += lineHeight;
                continue;
            }
            else
            {
                quad++;
            }

            float texX = fchar->x;
            float texY = 1.0f - fchar->y;
            float texW = fchar->width;
            float texH = fchar->height;

            float cwidth = fchar->width * font->getTextureWidth() * fsize;
            float cheight = fchar->height * font->getTextureHeight() * fsize;

            float coffsetX = fchar->offsetX * font->getTextureWidth() * fsize;
            float coffsetY = fchar->offsetY * font->getTextureHeight() * fsize;
            float cadvanceX = fchar->advanceX * font->getTextureWidth() * fsize;

            float ckernel = 0.0f;
            std::map<int, float>::const_iterator k = fchar->kernel.find(lastIndex);
            if (k != fchar->kernel.end())
            {
                ckernel = (*k).second * fsize;
            }

            data->setQuad(buffer, quad, startX + coffsetX + ckernel, startY + coffsetY, cwidth, cheight);
            data->setQuadTexture(buffer, quad, texX, texY, texW, texH);

            startX += cadvanceX;
            lastIndex = index;
        }
        quad++;
        for (int i = quad; i < quadCount; i++)
        {
            data->setQuad(buffer, i, startX, startY, 0.0f, 0.0f);
        }

        lines.push_back(startX);
        if (width < startX)
        {
            width = startX;
        }

        width += (2.0f * fsize);

        quad = 0;
        int line = 0;
        for (unsigned int i = 0; i < text.size(); i++)
        {
            if (text.at(i) != '\n')
            {
                float w = width * -0.5f;
                float diff = line < (int)lines.size() ? lines.at(line) : width;
                switch (textAlignment)
                {
                    case eHORIZONTAL_ALIGN_CENTER: w += (width - diff) * 0.5f; break;
                    case eHORIZONTAL_ALIGN_RIGHT:  w += (width - diff); break;
                    default: break;
                }

                data->moveQuad(buffer, quad, w, height * -0.5f - (font->getLineOffsetY() * fsize), 0.0f);
                quad++;
            }
            else
            {
                line++;
            }
        }

        data->unmapBuffer();
    }

    return true;
}

Magic3D::XMLElement* Magic3D::GUILabel::save(XMLElement* root)
{
    Sprite::save(root);
    if (root)
    {
        saveString(root, M3D_GUI_LABEL_XML_TEXT, text);
        if (font)
        {
            saveString(root, M3D_GUI_LABEL_XML_FONT, font->getName());
        }
        else
        {
            saveString(root, M3D_GUI_LABEL_XML_FONT, M3D_DEFAULT_FONT);
        }
        saveInt(root, M3D_GUI_LABEL_XML_ALIGNMENT, textAlignment);
        saveColorRGBA(root, M3D_GUI_LABEL_XML_COLOR, textColor);
        saveFloat(root, M3D_GUI_LABEL_XML_SIZE, textSize);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::GUILabel::load(XMLElement* root)
{
    Sprite::load(root);
    if (root)
    {
        text = loadString(root, M3D_GUI_LABEL_XML_TEXT);
        if (text.compare(M3D_XML_NULL) == 0)
        {
            text.clear();
        }
        bool created;
        std::string fontName = loadString(root, M3D_GUI_LABEL_XML_FONT);
        if (fontName.compare(M3D_XML_NULL) != 0)
        {
            font = ResourceManager::getFonts()->load(fontName, created);
        }
        textAlignment = (HORIZONTAL_ALIGN)loadInt(root, M3D_GUI_LABEL_XML_ALIGNMENT);
        textColor = loadColorRGBA(root, M3D_GUI_LABEL_XML_COLOR);
        textSize = loadFloat(root, M3D_GUI_LABEL_XML_SIZE);
    }
    return root;
}
