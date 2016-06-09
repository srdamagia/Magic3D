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

#include <magic3d/image/image_pvr.h>
#include <magic3d/renderer/opengl/renderer_opengl.h>

Magic3D::PVR::PVR() : Image()
{
    texture = NULL;
}

Magic3D::PVR::~PVR()
{

}

bool Magic3D::PVR::decode(DataBuffer* file)
{
    if (file)
    {

    }
    return false;
}

Magic3D::Texture* Magic3D::PVR::getTexture(DataBuffer* file, TEXTURE type, std::string name, std::string fileName, bool mipmap, bool clamp)
{
#ifdef MAGIC3D_IOS
    if (file)
    {
        // Get the file size
        file->seek(0, SEEK_END);
        dibSize = file->tell();
        file->seek(0, SEEK_SET);

        // read the data, append a 0 byte as the data might represent a string
        dib = new byte[dibSize + 1];
        dib[dibSize] = '\0';
        size_t bytesRead = file->read(dib, dibSize);

        if (bytesRead != dibSize)
        {
            delete [] dib;
            dib = NULL;
            dibSize = 0;
        }

        if (dib)
        {
            PVRHeader* pvrHeader = (PVRHeader*)dib;
            unsigned int u32NumSurfs;
            bool alpha = pvrHeader->alphaBitMask == 0;

            // perform checks for old PVR
            if(pvrHeader->headerSize != sizeof(PVRHeader))
            {	// Header V1
                if(pvrHeader->headerSize == PVR_HEADER_V1_SIZE)
                {
                    if(pvrHeader->pfFlags & PVR_CUBEMAP)
                    {
                        u32NumSurfs = 6;
                    }
                    else
                    {
                        u32NumSurfs = 1;
                    }
                }
                else
                {
                    Log::log(eLOG_FAILURE, "not a valid pvr.");
                    return false;
                }
            }
            else
            {	// Header V2
                if(pvrHeader->numSurfs < 1)
                {	// encoded with old version of PVRTexTool before zero numsurfs bug found.
                    if(pvrHeader->pfFlags & PVR_CUBEMAP)
                    {
                        u32NumSurfs = 6;
                    }
                    else
                    {
                        u32NumSurfs = 1;
                    }
                }
                else
                {
                    u32NumSurfs = pvrHeader->numSurfs;
                }
            }

            /* Only accept PVRTC */
            if ( (((pvrHeader->pfFlags & PVR_PIXELTYPE) == PVRTC_2) || ((pvrHeader->pfFlags & PVR_PIXELTYPE) == PVRTC_4)) )
            {
                GLuint textureName;
                GLenum textureFormat = 0;
                GLenum target = 0;

                switch(pvrHeader->pfFlags & PVR_PIXELTYPE)
                {
                    case PVRTC_2:
                    {
                        textureFormat = alpha ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;	// PVRTC2
                        break;
                    }

                    case PVRTC_4:
                    {
                        textureFormat = alpha ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;	// PVRTC4
                        break;
                    }
                    default:
                    {
                        Log::logFormat(eLOG_FAILURE, "%s - Pixel type not supported.", fileName.c_str());
                        break;
                    }
                }

                glGenTextures(1, &textureName);
                check_gl_error();

                if(pvrHeader->pfFlags & PVR_CUBEMAP)
                {
                    target = GL_TEXTURE_CUBE_MAP;

                }
                else
                {
                    target = GL_TEXTURE_2D;
                }
                
                glActiveTexture(GL_TEXTURE0);
                check_gl_error();

                glBindTexture(target, textureName);
                
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                check_gl_error();
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                check_gl_error();
                
                if (pvrHeader->headerSize > 0)
                {
                    bool errors = false;
                    unsigned long totalSize = pvrHeader->headerSize;

                    for(unsigned int i = 0; i < u32NumSurfs; i++)
                    {
                        byte*        data = ((byte*)dib + pvrHeader->headerSize) + pvrHeader->textureDataSize * i;
                        int          levels = (pvrHeader->pfFlags & PVR_MIPMAP) && mipmap && !alpha ? pvrHeader->mipMapCount : 0;
                        unsigned int sizeX = pvrHeader->width;
                        unsigned int sizeY = pvrHeader->height;
                        unsigned int compressedSize = 0;
                        
                        for(int level = 0; level <= levels; sizeX = Math::max(sizeX / 2, 1), sizeY = Math::max(sizeY / 2, 1), level++)
                        {
                            compressedSize = ( Math::max(sizeX, 8) * Math::max(sizeY, 8) * pvrHeader->bitCount + 7) / 8;

                            if(pvrHeader->pfFlags & PVR_CUBEMAP)
                            {
                                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, textureFormat, sizeX, sizeY, 0, compressedSize, data);
                            }
                            else
                            {
                                glCompressedTexImage2D(GL_TEXTURE_2D, level, textureFormat, sizeX, sizeY, 0, compressedSize, data);
                            }
                            check_gl_error();

                            totalSize += compressedSize;
                            data += compressedSize;
                        }
                    }
                    
                    if (!errors && totalSize <= dibSize)
                    {
                        texture = new Texture(name, textureName, type, pvrHeader->width, pvrHeader->height, mipmap && !alpha, clamp, fileName);
                    }

                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    check_gl_error();
                    
                    if (mipmap)
                    {
                        if (pvrHeader->mipMapCount && !alpha)
                        {
                            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, pvrHeader->mipMapCount);
                            check_gl_error();
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                            check_gl_error();
                        }
                        else
                        {
                            glGenerateMipmap(target);
                            check_gl_error();
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            check_gl_error();
                        }
                    }
                    else
                    {
                        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
                        check_gl_error();
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        check_gl_error();
                    }
                    
                    if (clamp)
                    {
                        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        check_gl_error();
                        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        check_gl_error();
                    }
                    else
                    {
                        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        check_gl_error();
                        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        check_gl_error();
                    }
                }
            }
        }
    }
#else
    if (file || type || !name.empty() || fileName.empty() || mipmap || clamp)
    {

    }
#endif
    
    return texture;
}
