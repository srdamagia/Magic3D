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

#ifndef MAGIC3D_IMAGE_TGA_H
#define MAGIC3D_IMAGE_TGA_H

#include <magic3d/magic3d.h>

namespace Magic3D
{

class TGA : public Image
{
    enum TGA_COLOR_MAP
    {
        TGA_Null     = 0,
        TGA_Map      = 1,
        TGA_RGB      = 2,
        TGA_Mono     = 3,
        TGA_RLEMap   = 9,
        TGA_RLERGB   = 10,
        TGA_RLEMono  = 11,
        TGA_CompMap  = 32,
        TGA_CompMap4 = 33
    };

    #pragma pack(push,1)
    struct TGAHeader
    {
        byte idLength;
        byte cmapType;
        byte type;

        word cmapIndex;
        word cmapLength;
        byte cmapEntrySize;

        word x;
        word y;
        word width;
        word height;
        byte bpp;
        byte desc;
    } __attribute__((__packed__));
    #pragma pack(pop)

private:
    TGAHeader header;
    void expandCompressedLine(byte* dest, DataBuffer *file, int width);
    void expandUncompressedLine(byte* dest, DataBuffer *file, int width);
public:
    TGA();
    virtual ~TGA();

    virtual bool decode(DataBuffer* file);
};
}

#endif
