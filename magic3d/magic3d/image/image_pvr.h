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

#ifndef MAGIC3D_IMAGE_PVR_H
#define MAGIC3D_IMAGE_PVR_H

#include <magic3d/magic3d.h>

namespace Magic3D
{

class PVR : public Image
{
    // Describes the header of a PVR header-texture
    struct PVRHeader
    {
        unsigned int headerSize;      /*!< size of the structure */
        unsigned int height;          /*!< height of surface to be created */
        unsigned int width;           /*!< width of input surface */
        unsigned int mipMapCount;     /*!< number of mip-map levels requested */
        unsigned int pfFlags;         /*!< pixel format flags */
        unsigned int textureDataSize; /*!< Total size in bytes */
        unsigned int bitCount;        /*!< number of bits per pixel  */
        unsigned int rBitMask;        /*!< mask for red bit */
        unsigned int gBitMask;        /*!< mask for green bits */
        unsigned int bBitMask;        /*!< mask for blue bits */
        unsigned int alphaBitMask;    /*!< mask for alpha channel */
        unsigned int PVR;             /*!< magic number identifying pvr file */
        unsigned int numSurfs;        /*!< the number of surfaces present in the pvr */
    };

    enum
    {
        PVRTC_2 = 24,
        PVRTC_4
    };

    static const unsigned int PVR_MIPMAP      = (1 <<  8);    // has mip map levels
    static const unsigned int PVR_TWIDDLE     = (1 <<  9);    // is twiddled
    static const unsigned int PVR_BUMPMAP     = (1 << 10);    // has normals encoded for a bump map
    static const unsigned int PVR_TILING      = (1 << 11);    // is bordered for tiled pvr
    static const unsigned int PVR_CUBEMAP     = (1 << 12);    // is a cubemap/skybox
    static const unsigned int PVR_FALSEMIPCOL = (1 << 13);    //
    static const unsigned int PVR_VOLUME      = (1 << 14);
    static const unsigned int PVR_PIXELTYPE   = 0xff;         // pixel type is always in the last 16bits of the flags
    static const unsigned int PVR_IDENTIFIER  = 0x21525650;   // the pvr identifier is the characters 'P','V','R'

    static const unsigned int PVR_HEADER_V1_SIZE = 44;

private:
    Texture* texture;
public:
    PVR();
    virtual ~PVR();

    virtual bool decode(DataBuffer* file);
    Texture* getTexture(DataBuffer* file, TEXTURE type, std::string name, std::string fileName, bool mipmap, bool clamp);
};
}

#endif
