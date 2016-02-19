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
#include <magic3d/resource/resources_font.h>

Magic3D::Font::Font(std::string name)
{
    charMap.resize(256);
    this->name = name;
    this->lineHeight = 0.0f;
    this->lineHeightPX = 0.0f;
    this->lineOffsetY = 0.0f;
    load();
}

Magic3D::Font::~Font()
{

}

const std::string& Magic3D::Font::getName()
{
    return name;
}

Magic3D::FontChar* Magic3D::Font::getChar(int index)
{
    FontChar* result = NULL;
    if (index >= 0 && index < (int)charMap.size())
    {
        result = &charMap[index];
    }
    return result;
}

float Magic3D::Font::getLineHeight()
{
    return lineHeight;
}

float Magic3D::Font::getLineHeightInPixels()
{
    return lineHeightPX;
}

float Magic3D::Font::getLineOffsetY()
{
    return lineOffsetY;
}

Magic3D::Texture* Magic3D::Font::getTexture(int index)
{
    Texture* result = NULL;
    if (index >= 0 && (unsigned int)index < textures.size())
    {
        result = textures.at(index);
    }
    return result;
}

float Magic3D::Font::getTextureWidth()
{
    float result = 1.0f;
    if ((int)textures.size() > 0)
    {
        Texture* texture = textures.at(0);
        if (texture)
        {
            result = texture->width;
        }
    }
    return result;
}

float Magic3D::Font::getTextureHeight()
{
    float result = 1.0f;
    if ((int)textures.size() > 0)
    {
        Texture* texture = textures.at(0);
        if (texture)
        {
            result = texture->height;
        }
    }
    return result;
}

void Magic3D::Font::load()
{
    XMLDocument* doc = new XMLDocument();

    std::string fileName = ResourceManager::getFonts()->getPath(name + ".fnt");

    bool result = false;
    if (ResourceManager::getInstance()->getPackage())
    {
        Memory mem;
        result = ResourceManager::getInstance()->unpack(fileName, &mem);
        std::string str = mem.getBuffer()->str();
        result = result && doc->Parse(str.c_str(), str.size()) == XML_SUCCESS;
    }
    else
    {
        result = doc->LoadFile(fileName.c_str()) == XML_SUCCESS;
    }
    if (result)
    {
        XMLElement* root = doc->FirstChildElement();

        // should always have a valid root but handle gracefully if it does
        if (root)
        {
            //load(pElem);
            XMLElement* xml = root->FirstChildElement("pages");
            if (xml)
            {
                xml = xml->FirstChildElement("page");
                while (xml)
                {
                    bool created = false;
                    std::string fileName = xml->Attribute("file");
                    Texture* texture = ResourceManager::getTextures()->load(eTEXTURE_FONT, std::string("font_") + getName(), fileName, true, true, created);

                    if (texture)
                    {
                        textures.push_back(texture);
                    }
                    xml = xml->NextSiblingElement("page");
                }
            }

            xml = root->FirstChildElement("common");
            if (xml)
            {
                xml->QueryFloatAttribute("lineHeight", &lineHeightPX);
                lineHeight = lineHeightPX;

                Texture* texture = NULL;

                if ((int)textures.size() > 0)
                {
                    texture = textures.at(0);
                }

                if (texture)
                {
                    lineHeight /= texture->height;
                }
                else
                {
                    lineHeight = 1.0f;
                }
            }
            else
            {
                lineHeightPX = 1.0f;
                lineHeight = lineHeightPX;
            }

            xml = root->FirstChildElement("chars");
            if (xml)
            {
                lineOffsetY = 0.0f;
                xml = xml->FirstChildElement("char");
                while (xml)
                {
                    int id;
                    FontChar c;

                    xml->QueryIntAttribute("id", &id);
                    xml->QueryFloatAttribute("y", &c.y);
                    xml->QueryFloatAttribute("x", &c.x);
                    xml->QueryFloatAttribute("xadvance", &c.advanceX);
                    xml->QueryFloatAttribute("width", &c.width);
                    xml->QueryFloatAttribute("height", &c.height);
                    xml->QueryFloatAttribute("xoffset", &c.offsetX);
                    xml->QueryFloatAttribute("yoffset", &c.offsetY);
                    xml->QueryIntAttribute("page", &c.textureID);

                    /*if (lineOffsetY < (c.height + c.offsetY) - lineHeightPX)
                    {
                        lineOffsetY = (c.height + c.offsetY) - lineHeightPX;
                    }*/

                    if (lineOffsetY < ((c.offsetY + c.height * 0.5f) - lineHeightPX) + 2.0f)
                    {
                        lineOffsetY = ((c.offsetY + c.height * 0.5f) - lineHeightPX) + 2.0f;
                    }

                    Texture* texture = NULL;

                    if ((int)textures.size() > c.textureID)
                    {
                        texture = textures.at(c.textureID);
                    }
                    if (texture)
                    {
                        float width = texture->width;
                        float height = texture->height;

                        c.y        /= height;
                        c.x        /= width;
                        c.advanceX /= width;
                        c.width    /= width;
                        c.height   /= height;
                        c.offsetX  /= width;
                        c.offsetY  /= height;
                    }

                    charMap[id] = c;

                    xml = xml->NextSiblingElement("char");
                }
            }

            xml = root->FirstChildElement("kernings");
            if (xml)
            {
                xml = xml->FirstChildElement("kerning");
                while (xml)
                {
                    int id;
                    int first;
                    float amount;

                    xml->QueryIntAttribute("first", &first);
                    xml->QueryFloatAttribute("amount", &amount);
                    xml->QueryIntAttribute("second", &id);

                    FontChar* c = getChar(id);
                    if (c)
                    {
                        c->kernel[first] = amount;
                    }

                    xml = xml->NextSiblingElement("kerning");
                }
            }

            Log::logFormat(eLOG_RENDERER, "Font \"%s\" loaded: %s", name.c_str(), fileName.c_str());
        }
    }
    else
    {
        Log::logFormat(eLOG_FAILURE, "Font \"%s\" not found: %s", name.c_str(), fileName.c_str());
    }

    if (doc)
    {
        delete doc;
        doc = NULL;
    }
}

//******************************************************************************
Magic3D::ResourcesFont::ResourcesFont() : Resources<Font>()
{
    resourceName = M3D_RESOURCE_FONT;
}

Magic3D::ResourcesFont::~ResourcesFont()
{

}

bool Magic3D::ResourcesFont::remove(std::string name)
{
    bool result = false;
    Font* font = Resources<Font>::get(name);
    Resources<Font>::resources.erase(name);
    if (font)
    {
        typename std::map<std::string, Object*>::const_iterator it_o = ResourceManager::getObjects()->begin();

        while (it_o != ResourceManager::getObjects()->end())
        {
            Object* child = (*it_o++).second;

            if (child->getType() == eOBJECT_GUI_LABEL)
            {
                //static_cast<GUILabel*>(child)->setfont(NULL);
            }
        }

        delete font;

        result = true;
    }

    return result;
}

Magic3D::Font* Magic3D::ResourcesFont::load(std::string name, bool& created)
{
    created = false;
    Font* result = Resources<Font>::get(name);
    if (!result)
    {
        result = new Font(name);
        created = true;
        add(result);
    }
    return result;
}

std::string Magic3D::ResourcesFont::getPath(std::string font)
{
    std::string fileName = Resources<Font>::getPath() + M3D_PATH_FONT + font;

    return fileName;
}

void Magic3D::ResourcesFont::addDefault()
{
    bool created = false;
    load(M3D_DEFAULT_FONT, created);
}
