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

#ifndef MAGIC3D_TEXT_H
#define MAGIC3D_TEXT_H

#include <magic3d/mesh.h>

namespace Magic3D
{

#define M3D_TEXT_XML_TEXT "text_text"
#define M3D_TEXT_XML_FONT "text_font"
#define M3D_TEXT_XML_ALIGNMENT "text_alignment"
#define M3D_TEXT_XML_COLOR "text_color"
#define M3D_TEXT_XML_SIZE "text_size"

class Font;
class TextData : public MeshData
{
private:
    Font* font;
    std::string text;
    int lastReturns;
    float textSize;
    HORIZONTAL_ALIGN textAlignment;
    ColorRGBA textColor;

    float width;
    float height;

    bool invert;
    bool changed;

protected:
    TextData(const TextData& textData);

public:
    TextData(std::string name, float size, bool invert = false);
    virtual ~TextData();
    virtual void* spawn() const;

    virtual bool update(Object* object);
    bool isChanged();

    float getWidth();
    float getHeight();

    void setFont(Font* font);
    Font* getFont();

    void setTextSize(float size);
    float getTextSize();

    void setTextAlignment(HORIZONTAL_ALIGN alignment);
    HORIZONTAL_ALIGN getTextAlignment();

    void setTextColor(ColorRGBA color);
    const ColorRGBA& getTextColor();

    void setText(std::string text);
    std::string getText();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class Text : public Object
{
protected:
    Text(const Text& text, std::string name);
public:
    Text(std::string name, float size);
    virtual ~Text();

    virtual void* spawn(std::string name) const;

    TextData* getText();

    virtual bool update();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};


}

#endif // MAGIC3D_TEXT_H
