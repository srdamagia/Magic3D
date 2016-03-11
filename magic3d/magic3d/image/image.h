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

#ifndef MAGIC3D_IMAGE_H
#define MAGIC3D_IMAGE_H

#include <magic3d/magic3d.h>

namespace Magic3D
{
class Image
{
protected:
    byte* dib;

    long  dibSize;

    dword colors;
    dword effWidth;
    dword sizeImage;
    dword sizePalette;

    int   bpp;
    int   width;
    int  height;

public:
    Image();
    virtual ~Image();

    void create(int width, int height, int bpp);
    void destroy();
    ColorRGBApack* getPalette() const;
    void setGrayPalette();
    void setPaletteColor(byte idx, byte r, byte g, byte b, byte a = 0);
    byte getPixelIndex(long x, long y);
    void setPixelIndex(long x, long y, byte i);
    ColorRGBApack getPixelColor(byte* pColorData);
    bool mirror();
    void transfer(Image &from);
    virtual bool decode(FILE* file);
    virtual bool decode(DataBuffer* file);

    byte* getPixels();
    long getPixelsSize();
    int getBPP();
    int getWidth();
    int getHeight();

    bool isValid() const {return dib != 0;}
};
}

#endif
