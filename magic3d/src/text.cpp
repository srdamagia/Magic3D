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

Magic3D::TextData::TextData(const TextData& textData) : MeshData(textData)
{
    this->text = textData.text;
    this->lastReturns = textData.lastReturns;
    this->font = textData.font;
    this->textSize = textData.textSize;
    this->textAlignment = textData.textAlignment;
    this->textColor = textData.textColor;

    this->width = textData.width;
    this->height = textData.height;

    this->invert = textData.invert;
    this->changed = true;
}

Magic3D::TextData::TextData(std::string name, float size, bool invert) : MeshData(eMESH_TRIANGLES, name)
{
    text = " ";
    lastReturns = 0;
    this->font = NULL;
    this->textSize = size;
    this->textAlignment = eHORIZONTAL_ALIGN_LEFT;
    this->textColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);

    width = 0.0f;
    height = 0.0f;

    this->invert = invert;
    changed = false;
}

Magic3D::TextData::~TextData()
{
    font = NULL;
}

void* Magic3D::TextData::spawn() const
{
    return (new TextData(*this));
}

void Magic3D::TextData::setFont(Font* font)
{
    this->font = font;
    changed = true;
}

Magic3D::Font* Magic3D::TextData::getFont()
{
    if (!font)
    {
        font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
    }

    return font;
}

void Magic3D::TextData::setTextSize(float size)
{
    this->textSize = size;
    changed = true;
}

float Magic3D::TextData::getTextSize()
{
    return textSize;
}

void Magic3D::TextData::setTextAlignment(HORIZONTAL_ALIGN alignment)
{
    textAlignment = alignment;
    changed = true;
}

Magic3D::HORIZONTAL_ALIGN Magic3D::TextData::getTextAlignment()
{
    return textAlignment;
}

void Magic3D::TextData::setTextColor(ColorRGBA color)
{
    this->textColor = color;
    changed = true;
}

const Magic3D::ColorRGBA& Magic3D::TextData::getTextColor()
{
    return textColor;
}

void Magic3D::TextData::setText(std::string text)
{
    unsigned int size = this->text.length();
    this->text = text;

    if (this->text.length() != size)
    {
        clear();
    }
    changed = true;
}

std::string Magic3D::TextData::getText()
{
    return text;
}

bool Magic3D::TextData::update(Object* object)
{
    if (object)
    {

    }
    Renderer* renderer = Renderer::getInstance();
    Font* font = getFont();
    std::vector<float> lines;

    if (!font)
    {
        return false;
    }

    float fsize = (textSize / 512.0f) / font->getLineHeightInPixels();
    float lineHeight = fsize * (font->getLineHeightInPixels() + 2.0f);

    int quadCount = getVerticesCount() / 4;

    if (quadCount < (int)text.size() - lastReturns)
    {
        lastReturns = 0;
        size_t tsize = text.size();
        for (size_t i = 0; i < tsize; i++)
        {
            if (text.at(i) == '\n')
            {
                lastReturns++;
                continue;
            }
            else if (renderer->hasMapBuffer() || ((int)i >= quadCount))
            {
                addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z, false);
            }
        }

        createVbo();
        quadCount = getVerticesCount() / 4;
    }

    width = 0.0f;
    height = lineHeight;

    if (getVerticesCount() > 0)
    {        
        int quad = -1;
        float startX = 0.0f;
        float startY = 0.0f;
        float* buffer = mapBuffer();

        int lastIndex = 0;
        size_t tsize = text.size();
        for (size_t i = 0; i < tsize; i++)
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
                if (invert)
                {
                    startY -= lineHeight;
                }
                else
                {
                    startY += lineHeight;
                }
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

            setQuad(buffer, quad, startX + coffsetX + ckernel, startY + coffsetY * 0.5f, cwidth, cheight);
            if (invert)
            {
                setQuadTexture(buffer, quad, texX, texY - texH, texW, -texH);
            }
            else
            {
                setQuadTexture(buffer, quad, texX, texY, texW, texH);
            }


            startX += cadvanceX;
            lastIndex = index;
        }
        quad++;
        for (int i = quad; i < quadCount; i++)
        {
            setQuad(buffer, i, startX, startY, 0.0f, 0.0f);
        }

        lines.push_back(startX);
        if (width < startX)
        {
            width = startX;
        }

        width += (2.0f * fsize);

        quad = 0;
        int line = 0;
        tsize = text.size();
        for (size_t i = 0; i < tsize; i++)
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

                if (invert)
                {
                    moveQuad(buffer, quad, w, height * 0.5f - lineHeight, 0.0f);
                }
                else
                {
                    moveQuad(buffer, quad, w, height * -0.5f, 0.0f);
                }
                quad++;
            }
            else
            {
                line++;
            }
        }

        unmapBuffer();
    }

    changed = false;

    box.corners[0] = Vector3(-width * 0.5f, -height * 0.5f, 0.0f);
    box.corners[1] = Vector3(width * 0.5f, height * 0.5f, 0.0f);
    return true;
}

bool Magic3D::TextData::isChanged()
{
    return changed;
}

float Magic3D::TextData::getWidth()
{
    return width;
}

float Magic3D::TextData::getHeight()
{
    return height;
}

Magic3D::XMLElement* Magic3D::TextData::save(XMLElement* root)
{
    if (root)
    {
        saveString(root, M3D_TEXT_XML_TEXT, text);
        if (font)
        {
            saveString(root, M3D_TEXT_XML_FONT, font->getName());
        }
        else
        {
            saveString(root, M3D_TEXT_XML_FONT, M3D_DEFAULT_FONT);
        }
        saveInt(root, M3D_TEXT_XML_ALIGNMENT, textAlignment);
        saveColorRGBA(root, M3D_TEXT_XML_COLOR, textColor);
        saveFloat(root, M3D_TEXT_XML_SIZE, textSize);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::TextData::load(XMLElement* root)
{
    if (root)
    {
        text = loadString(root, M3D_TEXT_XML_TEXT);
        if (text.compare(M3D_XML_NULL) == 0)
        {
            text.clear();
        }        
        std::string fontName = loadString(root, M3D_TEXT_XML_FONT);
        if (fontName.compare(M3D_XML_NULL) != 0)
        {
            bool created;
            font = ResourceManager::getFonts()->load(fontName, created);
        }
        textAlignment = (HORIZONTAL_ALIGN)loadInt(root, M3D_TEXT_XML_ALIGNMENT);
        textColor = loadColorRGBA(root, M3D_TEXT_XML_COLOR);
        textSize = loadFloat(root, M3D_TEXT_XML_SIZE);
        changed = true;
    }
    return root;
}

//******************************************************************************
Magic3D::Text::Text(const Text& text, std::string name) : Object(text, name)
{

}

Magic3D::Text::Text(std::string name, float size) : Object(eOBJECT_TEXT, eRENDER_3D, name)
{
    setZOrder(true);
    Mesh* textMesh = new Mesh(new TextData("name", size, true), true, true);
    textMesh->setIlluminated(false);
    textMesh->setCastShadows(false);
    textMesh->setReceiveShadows(false);
    addMesh(textMesh);

    textMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_FONT));
}

Magic3D::Text::~Text()
{

}

void* Magic3D::Text::spawn(std::string name) const
{
    return new Text(*this, name);
}

Magic3D::TextData* Magic3D::Text::getText()
{
    return static_cast<TextData*>(getMeshes()->at(0)->getData());
}

bool Magic3D::Text::update()
{
    if (getText()->isChanged())
    {
        updateBoundingBox(true);
    }

    return Object::update();
}

Magic3D::XMLElement* Magic3D::Text::save(XMLElement* root)
{
    if (root)
    {
        Object::save(root);
        getText()->save(root);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Text::load(XMLElement* root)
{
    if (root)
    {
        Object::load(root);
        getText()->load(root);
    }
    return root;
}
