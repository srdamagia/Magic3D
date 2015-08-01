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

#include <magic3d/image/image_png.h>

Magic3D::PNG::PNG() : Image()
{

}

Magic3D::PNG::~PNG()
{

}

bool Magic3D::PNG::decode(DataBuffer* file)
{
    bool result = false;

    if (file)
    {
        png_byte header[8];

        //read the header
        file->read(header, 8);

        //test if png
        int is_png = !png_sig_cmp(header, 0, 8);
        if (!is_png) {
            return false;
        }

        //create png struct
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
            return false;
        }

        //create png info struct
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
            return false;
        }

        //create png info struct
        png_infop end_info = png_create_info_struct(png_ptr);
        if (!end_info) {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
            return false;
        }

        //png error stuff, not sure libpng man suggests this.
        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        //init png reading
        char szLastError[256];
        png_set_read_fn(png_ptr, file, (png_rw_ptr)user_read_data);
        png_set_error_fn(png_ptr, szLastError, (png_error_ptr)user_error_fn,NULL);

        //let libpng know you already read the first 8 bytes
        png_set_sig_bytes(png_ptr, 8);

        // read all the info up to the image data
        png_read_info(png_ptr, info_ptr);

        int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        int pixel_depth = bit_depth * png_get_channels(png_ptr, info_ptr);
        int color_type = png_get_color_type(png_ptr, info_ptr);

        png_set_expand(png_ptr);
        //png_set_bgr(png_ptr);

        if (color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(png_ptr);
        }

       if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
       {
           png_set_expand_gray_1_2_4_to_8(png_ptr);
       }

       if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
       {
           png_set_tRNS_to_alpha(png_ptr);
       }

        // Update the png info struct.
        png_read_update_info(png_ptr, info_ptr);

        create(png_get_image_width(png_ptr, info_ptr), png_get_image_height(png_ptr, info_ptr), pixel_depth);

        // Row size in bytes.
        int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        //row_pointers is for pointing to image_data for reading the png with libpng
        png_bytep *row_pointers = new png_bytep[height];
        if (!row_pointers) {
            //clean up memory and close stuff
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }
        // set the individual row_pointers to point at the correct offsets of image_data
        for (int i = 0; i < height; ++i)
            row_pointers[height - 1 - i] = dib + i * rowbytes;

        //read the png into image_data through row_pointers
        png_read_image(png_ptr, row_pointers);

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] row_pointers;

        result = true;
    }

    return result;
}
