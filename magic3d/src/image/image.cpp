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

#include <magic3d/image/image.h>

Magic3D::Image::Image()
{
    dib = NULL;
    dibSize     = 0L;
    colors      = 0L;
    effWidth    = 0L;
    sizeImage   = 0L;
    sizePalette = 0L;
    bpp         = 0;
    width       = 0;
    height      = 0;
}

Magic3D::Image::~Image()
{
    destroy();
}

void Magic3D::Image::create(int width, int height, int bpp)
{
    destroy();

    this->bpp = bpp;
    switch (bpp)
    {
        case 1:	 colors = 2;   break;
        case 4:	 colors = 16;  break;
        case 8:	 colors = 256; break;
        default: colors = 0;
    }

    this->width = width;
    this->height = height;
    effWidth = bpp / 8 * width;
    //effWidth = ((((bpp * width) + 31) / 32) * 4);
    sizeImage = effWidth * height;
    sizePalette = colors * sizeof(ColorRGBApack);

    dibSize = sizeImage + sizePalette;
    dib = new byte[dibSize]; // alloc memory block to store our bitmap

    if (dib)
    {
        ColorRGBApack* pal = getPalette();
        if (pal)
        {
            memset(pal, 0, sizePalette);
        }
    }
}

void Magic3D::Image::destroy()
{
    if (dib)
    {
        delete[] dib;
        dib = 0;
    }
}

Magic3D::ColorRGBApack* Magic3D::Image::getPalette() const
{
    ColorRGBApack* result = NULL;
    if ((dib) && (colors))
    {
        result = (ColorRGBApack*)(byte*)dib;
    }
    return result;
}

void Magic3D::Image::setGrayPalette()
{
    if ((dib != NULL) && (colors != 0))
    {
        ColorRGBApack* pal = getPalette();
        for (dword ni = 0; ni < colors; ni++)
        {
            pal[ni].b = pal[ni].g = pal[ni].r = (byte)(ni * (255 / (colors-1)));
        }
    }
}

void Magic3D::Image::setPaletteColor(byte idx, byte r, byte g, byte b, byte a)
{
    if ((dib) && (colors))
    {
        byte* iDst = (byte*)(dib);
        if (idx < colors)
        {
            long ldx = idx * sizeof(ColorRGBApack);
            iDst[ldx++] = (byte)r;
            iDst[ldx++] = (byte)g;
            iDst[ldx++] = (byte)b;
            iDst[ldx]   = (byte)a;
        }
    }
}

byte Magic3D::Image::getPixelIndex(long x, long y)
{
    byte result = 0;
    if ((dib != NULL) && (colors !=0))
    {
        if ((x >= 0) && (y >= 0) && (x < width) && (y < height))
        {
            if (bpp == 8)
            {
                return ((byte*)dib)[y * effWidth + x];
            }
            else
            {
                byte pos;
                byte dst = ((byte*)dib)[y * effWidth + (x * bpp >> 3)];
                if (bpp == 4)
                {
                    pos = (byte)(4 * (1 - x % 2));
                    dst &= (0x0F << pos);
                    return (byte)(dst >> pos);
                }
                else if (bpp == 1)
                {
                    pos = (byte)(7 - x % 8);
                    dst &= (0x01 << pos);
                    return (byte)(dst >> pos);
                }
            }
        }
    }
    return result;
}

void Magic3D::Image::setPixelIndex(long x, long y, byte i)
{
    if ((dib != NULL) && (colors != 0) && (x >= 0) && (y >= 0) && (x < width) && (y < height))
    {
        if (bpp == 8)
        {
            ((byte*)dib)[y * effWidth + x] = i;
            return;
        }
        else
        {
            byte pos;
            byte* dst = (byte*)dib + y * effWidth + (x * bpp >> 3);
            if (bpp == 4)
            {
                pos = (byte)(4 * (1 - x % 2));
                *dst &= ~(0x0F << pos);
                *dst |= ((i & 0x0F) << pos);
                return;
            }
            else if (bpp == 1)
            {
                pos = (byte)(7 - x % 8);
                *dst &= ~(0x01 << pos);
                *dst |= ((i & 0x01) << pos);
                return;
            }
        }
    }
}

bool Magic3D::Image::mirror()
{
    if (!dib) return false;

    Image* imatmp = new Image();
    if (!imatmp) return false;
    byte *iSrc;
    byte *iDst;
    long wdt = (width - 1) * (bpp >= 24 ? 3 : 1);
    iSrc = (byte*)dib + wdt;
    iDst = (byte*)imatmp->dib;
    long x;
    long y;
    switch (bpp)
    {
        case 24:
        {
            for(y = 0; y < height; y++){
                for(x = 0; x <= wdt; x += 3){
                    *(iDst + x)     =* (iSrc - x);
                    *(iDst + x + 1) =* (iSrc - x + 1);
                    *(iDst + x + 2) =* (iSrc - x + 2);
                }
                iSrc += effWidth;
                iDst += effWidth;
            }
            break;
        }
        case 8:
        {
            for(y = 0; y < height; y++){
                for(x=0; x <= wdt; x++)
                    *(iDst + x) =* (iSrc - x);
                iSrc += effWidth;
                iDst += effWidth;
            }
            break;
        }
        default:
        {
            for(y = 0; y < height; y++)
            {
                for(x=0; x <= wdt; x++)
                {
                    imatmp->setPixelIndex(x, y, getPixelIndex(wdt-x,y));
                }
            }
        }
    }

    transfer(*imatmp);
    delete imatmp;
    return true;
}

void Magic3D::Image::transfer(Image& from)
{
    destroy();

    dib = from.dib;

    from.dib = NULL;
}

bool Magic3D::Image::decode(FILE* file)
{
    File f(file);
    return decode(&f);
}

bool Magic3D::Image::decode(DataBuffer* file)
{
    return file != NULL;
}

byte* Magic3D::Image::getPixels()
{
    byte* result = NULL;
    if (dib)
    {
        result = (byte*)dib;
    }
    return result;
}

long Magic3D::Image::getPixelsSize()
{
    return dibSize;
}

int Magic3D::Image::getBPP()
{
    return bpp;
}

int Magic3D::Image::getWidth()
{
    return width;
}

int Magic3D::Image::getHeight()
{
    return height;
}

Magic3D::ColorRGBApack Magic3D::Image::getPixelColor(byte* pColorData)
{
    ColorRGBApack result;
    switch( getBPP() )
    {
        case 8:
        {            
            result.r = pColorData[0];
            result.g = pColorData[0];
            result.b = pColorData[0];
            result.a = pColorData[0];
            break;
        }
        case 15:
        case 16:
        {
            // MSB           LSB
            // RRRRRGGG GGBBBBBA
            result.r = (pColorData[1] >> 3) & 0x1F;
            result.g = ((pColorData[1] << 2) & 0x1C) | ((pColorData[0] >> 6) & 0x07);
            result.b = (pColorData[0] >> 1) & 0x1F;

            result.a = 255 * ((pColorData[0] & 0x80));

            // Convert 5-bit channels to 8-bit channels by left shifting by three
            // and repeating the first three bits to cover the range [0,255] with
            // even spacing. For example:
            //   channel input bits:        4 3 2 1 0
            //   channel output bits: 4 3 2 1 0 4 3 2
            result.r = (result.r << 3) | (result.r >> 2);
            result.g = (result.g << 3) | (result.g >> 2);
            result.b = (result.b << 3) | (result.b >> 2);
            break;
        }
        case 24:
        {
            result.r = pColorData[0];
            result.g = pColorData[1];
            result.b = pColorData[2];
            result.a = 255;
            break;
        }
        case 32:
        {
            result.r = pColorData[0];
            result.g = pColorData[1];
            result.b = pColorData[2];
            result.a = pColorData[3];
            break;
        }
    }

    return result;
}
