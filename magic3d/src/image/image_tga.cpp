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

#include <magic3d/image/image_tga.h>
#include <magic3d/log.h>

Magic3D::TGA::TGA() : Image()
{
    memset(&header, 0, sizeof(TGAHeader));
}

Magic3D::TGA::~TGA()
{

}

bool Magic3D::TGA::decode(DataBuffer* file)
{
    bool result = false;
    if (file)
    {
        if (file->read(&header, sizeof(header)) == 0)
        {
            Log::log(eLOG_FAILURE, "Not a TGA");
        }

        bool compressed;
        switch (header.type){
            case TGA_Map:
            case TGA_RGB:
            case TGA_Mono: compressed = false; break;

            case TGA_RLEMap:
            case TGA_RLERGB:
            case TGA_RLEMono: compressed = true; break;
            default: {
                Log::log(eLOG_FAILURE, "Unknown TGA image type");
                return false;
            }
        }

        if (header.width == 0 || header.height == 0 || header.bpp == 0 || header.cmapLength > 256)
        {
            Log::log(eLOG_FAILURE, "bad TGA header");
            return false;
        }

        if (header.bpp != 8 && header.bpp != 15 && header.bpp != 16 && header.bpp != 24 && header.bpp != 32)
        {
            Log::log(eLOG_FAILURE, "bad TGA header");
            return false;
        }

        if (header.idLength > 0)
        {
            file->seeki(header.idLength, SEEK_CUR);
        }

        create(header.width, header.height, header.bpp);

        if (isValid())
        {
            if (header.cmapType != 0)
            {
                ColorRGBpack pal[256];
                file->read(pal, header.cmapLength * sizeof(ColorRGBpack));
                for (int i = 0; i < header.cmapLength; i++)
                {
                    setPaletteColor((byte)i, pal[i].r, pal[i].g, pal[i].b);
                }
            }

            if (header.type == TGA_Mono || header.type == TGA_RLEMono)
            {
                setGrayPalette();
            }

            bool xReversed = ((header.desc & 16) == 16);
            bool yReversed = ((header.desc & 32) == 32);

            byte* dest;
            int itY = 0;
            for (int y = 0; y < header.height; y++){
                if (file && !file->eof())
                {
                    if (yReversed)
                    {
                        itY = header.height - y - 1;
                    }
                    else
                    {
                        itY = y;
                    }

                    if ((itY >= 0) && (itY <= (int)header.height))
                    {
                        dest = (byte*)dib + effWidth * itY;

                        if (compressed)
                        {
                            expandCompressedLine(dest, file, header.width);
                        }
                        else
                        {
                            expandUncompressedLine(dest, file, header.width);
                        }
                    }
                }
            }

            if (xReversed)
            {
                mirror();
            }
        }
    }

    return result;
}

void Magic3D::TGA::expandCompressedLine(byte* dest, DataBuffer* file, int width)
{
    byte rle;
    size_t size = header.bpp == 32 ? 4 : 3;
    for (int x = 0; x < width; )
    {
        file->read(&rle, 1);
        if (rle & 128)
        {
            rle -= 127;
            if ((x + rle) > width)
            {
                rle = width - x;
            }
            switch (header.bpp)
            {
                case 24:
                case 32:
                {
                    ColorRGBApack color;
                    file->read(&color, size);
                    for (int ix = 0; ix < rle; ix++)
                    {
                        ColorRGBApack pixel;
                        pixel.r = color.b;
                        pixel.g = color.g;
                        pixel.b = color.r;
                        pixel.a = color.a;
                        memcpy(&dest[size * ix], &pixel, size);
                    }
                    break;
                }
                    /*case 32:
        {
            ColorRGBApack color;
            file->read(&color, 4, 1);
            for (int ix = 0; ix < rle; ix++)
            {
            memcpy(&dest[3 * ix], &color, 4);
            }
            break;
        }
        case 24:
        {
            ColorRGBpack triple;
            file->read(&triple, 3, 1);
            for (int ix = 0; ix < rle; ix++)
            {
            memcpy(&dest[3 * ix], &triple, 3);
            }
            break;
        }*/
                case 15:
                case 16:
                {
                    word pixel;
                    file->read(&pixel, 2);
                    ColorRGBpack triple;
                    triple.r = (byte)(( pixel & 0x1F ) * 8);
                    triple.g = (byte)(( pixel >> 2 ) & 0x0F8);
                    triple.b = (byte)(( pixel >> 7 ) & 0x0F8);
                    for (int ix = 0; ix < rle; ix++){
                        memcpy(&dest[3 * ix], &triple, 3);
                    }
                    break;
                }
                case 8:
                {
                    byte pixel;
                    file->read(&pixel, 1);
                    for (int ix = 0; ix < rle; ix++)
                    {
                        dest[ix] = pixel;
                    }
                    break;
                }
            }
        }
        else
        {
            rle += 1;
            expandUncompressedLine(dest, file, rle);
        }
        if (bpp >= 24)
        {
            dest += rle * size;
        }
        else
        {
            dest += rle;
        }
        x += rle;
    }
}

void Magic3D::TGA::expandUncompressedLine(byte* dest, DataBuffer* file, int width)
{
    switch (header.bpp)
    {
        case 8:	file->read(dest, width); break;
        case 15:
        case 16:
        {
            byte* dst = dest;
            word pixel;
            for (int x = 0; x < width; x++)
            {
                file->read(&pixel, 2);
                *dst++ = (byte)(( pixel & 0x1F ) * 8);   // blue
                *dst++ = (byte)(( pixel >> 2 ) & 0x0F8); // green
                *dst++ = (byte)(( pixel >> 7 ) & 0x0F8); // red
            }
            break;
        }
        case 24: //file->read(dest, 3 * width, 1); break;
        case 32:
        {
            size_t size = header.bpp == 32 ? 4 : 3;
            byte* dst = dest;
            for (int x = 0; x < width; x++)
            {
                ColorRGBApack pixel;
                file->read(&pixel, size);
                *dst++ = pixel.b;
                *dst++ = pixel.g;
                *dst++ = pixel.r;
                if (header.bpp == 32)
                {
                    *dst++ = pixel.a;
                }
            }
            break;
        }
    }
}
