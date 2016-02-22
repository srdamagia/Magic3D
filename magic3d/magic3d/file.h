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

#ifndef MAGIC3D_FILE_H
#define MAGIC3D_FILE_H

#include <magic3d/magic3d.h>

namespace Magic3D
{

class DataBuffer
{
public:
    DataBuffer(){}
    virtual ~DataBuffer(){}
    virtual size_t read(void *buffer, size_t size) = 0;
    virtual size_t write(const void *buffer, size_t size) = 0;

    virtual bool seeki(long offset, int origin) = 0;
    virtual bool seeko(long offset, int origin) = 0;
    virtual long telli() = 0;
    virtual long tello() = 0;
    virtual long sizei() = 0;
    virtual long sizeo() = 0;
    virtual bool flush() = 0;
    virtual bool eof() = 0;
    virtual bool put(char c) = 0;
    virtual int get() = 0;
};

class File : public DataBuffer
{
protected:
    FILE *file;
    std::string fileName;
    bool closeFile;

public:
    File(FILE* file = NULL)
    {
        this->file = file;
        closeFile = false;
    }

    virtual ~File()
    {
        close();
    }

    bool open(std::string fileName, const char *mode)
    {
        bool result = false;
        this->fileName = fileName;
        file = fopen(fileName.c_str(), mode);
        if (file)
        {
            result = true;
            closeFile = true;
        }

        return result;
    }

    virtual bool close()
    {
        int iErr = 0;
        if ( (file) && (closeFile) )
        {
            iErr = fclose(file);
            file = NULL;
            fileName.clear();
        }
        return (bool)(iErr == 0);
    }

    virtual size_t read(void *buffer, size_t size)
    {
        size_t result = 0;
        if (file)
        {
            result = fread(buffer, 1, size, file);
        }
        return result;
    }

    virtual size_t write(const void *buffer, size_t size)
    {
        size_t result = 0;
        if (file)
        {
            result = fwrite(buffer, 1, size, file);
        }
        return result;
    }

    virtual bool seek(long offset, int origin)
    {
        bool result = false;
        if (file)
        {
            result = (bool)(fseek(file, offset, origin) == 0);
        }
        return result;
    }
    virtual bool seeki(long offset, int origin) {return seek(offset, origin);}
    virtual bool seeko(long offset, int origin) {return seek(offset, origin);}

    virtual long tell()
    {
        long result = 0L;
        if (file)
        {
            result = ftell(file);
        }
        return result;
    }
    virtual long telli() {return tell();}
    virtual long tello() {return tell();}


    virtual long size()
    {
        long result = -1L;
        if (file)
        {
            long pos;
            pos = ftell(file);
            fseek(file, 0, SEEK_END);
            result = ftell(file);
            fseek(file, pos, SEEK_SET);
        }
        return result;
    }
    virtual long sizei() {return size();}
    virtual long sizeo() {return size();}

    virtual bool flush()
    {
        bool result = false;
        if (!file)
        {
            result = (bool)(fflush(file) == 0);
        }
        return result;
    }

    virtual bool eof()
    {
        bool result = true;
        if (file)
        {
            result = (bool)(feof(file) != 0);
        }
        return result;
    }

    virtual long error()
    {
        long result = -1L;
        if (file)
        {
            result = ferror(file);
        }
        return result;
    }

    virtual bool put(char c)
    {
        bool result = false;
        if (file)
        {
            result = (bool)(fputc(c, file) == c);
        }
        return result;
    }

    virtual int get()
    {
        int result = EOF;
        if (file)
        {
            result = getc(file);
        }
        return result;
    }

    const std::string& getFileName()
    {
        return fileName;
    }
};

class Memory : public DataBuffer
{
protected:
    std::stringstream buffer;

public:
    Memory()
    {

    }

    virtual ~Memory()
    {

    }

    virtual size_t read(void *buffer, size_t size)
    {
        this->buffer.read((char*)buffer, size);
        return size;
    }

    virtual size_t write(const void *buffer, size_t size)
    {
        this->buffer.write((const char*)buffer, size);
        return size;
    }

    virtual bool seeki(long offset, int origin)
    {
        this->buffer.seekg(offset, (std::ios_base::seekdir)origin);
        return true;
    }

    virtual bool seeko(long offset, int origin)
    {
        this->buffer.seekp(offset, (std::ios_base::seekdir)origin);
        return true;
    }

    virtual long telli()
    {
        return buffer.tellg();
    }

    virtual long tello()
    {
        return buffer.tellp();
    }

    virtual long sizei()
    {
        long result = -1L;
        long pos;
        pos = telli();
        seeki(0, buffer.end);
        result = telli();
        seeki(pos, buffer.beg);
        return result;
    }

    virtual long sizeo()
    {
        long result = -1L;
        long pos;
        pos = tello();
        seeko(0, buffer.end);
        result = tello();
        seeko(pos, buffer.beg);
        return result;
    }

    virtual bool flush()
    {
        buffer.flush();
        return true;
    }

    virtual bool eof()
    {
        return buffer.eof();
    }

    virtual bool put(char c)
    {
        buffer.put(c);
        return true;
    }

    virtual int get()
    {
        return buffer.get();
    }

    virtual  std::basic_stringbuf<std::stringstream::char_type, std::stringstream::traits_type, std::stringstream::allocator_type>* getBuffer()
    {
        return buffer.rdbuf();
    }
};

}

#endif
