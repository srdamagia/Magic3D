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

#ifndef MAGIC3D_IMAGE_PNG_H
#define MAGIC3D_IMAGE_PNG_H

#include <magic3d/magic3d.h>

extern "C" {
#include <png.h>
}

namespace Magic3D
{

class PNG : public Image
{
private:
    static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        DataBuffer* file = (DataBuffer*)png_get_io_ptr(png_ptr);
        size_t data_read = file->read(data, length);
        if (data_read != length)
        {
            png_error(png_ptr, "Read Error");
        }
    }
    static void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
    {
#ifdef DEBUG
        Log::logFormat(eLOG_FAILURE, "png error: %s", error_msg);
#endif
        strncpy((char*)png_get_error_ptr(png_ptr), error_msg, 255);
        longjmp(png_jmpbuf(png_ptr), 1);
    }
    void expand2to4bpp(byte* prow);
public:
    PNG();
    virtual ~PNG();

    virtual bool decode(DataBuffer* file);
};
}

#endif
