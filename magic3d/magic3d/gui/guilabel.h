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

#ifndef MAGIC3D_GUI_LABEL_H
#define MAGIC3D_GUI_LABEL_H

#include <magic3d/gui/gui.h>

namespace Magic3D
{

#define M3D_GUI_LABEL_XML_TEXT "gui_label_text"
#define M3D_GUI_LABEL_XML_FONT "gui_label_font"
#define M3D_GUI_LABEL_XML_ALIGNMENT "gui_label_alignment"
#define M3D_GUI_LABEL_XML_COLOR "gui_label_color"
#define M3D_GUI_LABEL_XML_SIZE "gui_label_size"

class Font;
class GUILabel : public GUI
{
private:
    Font* font;
    std::string text;
    int lastReturns;
    float textSize;
    HORIZONTAL_ALIGN textAlignment;
    ColorRGBA textColor;

protected:
    GUILabel(const GUILabel& guiLabel, std::string name);

public:
    GUILabel(std::string name, float size);
    virtual ~GUILabel();
    virtual void* spawn(std::string name) const;

    virtual bool updateMeshes();

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

}

#endif // MAGIC3D_GUI_LABEL_H
