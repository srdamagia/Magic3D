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

#ifndef MAGIC3D_PACKAGE_H
#define MAGIC3D_PACKAGE_H

#ifndef _WIN32
    #ifndef __USE_FILE_OFFSET64
        #define __USE_FILE_OFFSET64
    #endif
    #ifndef __USE_LARGEFILE64
        #define __USE_LARGEFILE64
    #endif
    #ifndef _LARGEFILE64_SOURCE
        #define _LARGEFILE64_SOURCE
    #endif
    #ifndef _FILE_OFFSET_BIT
        #define _FILE_OFFSET_BIT 64
    #endif
#endif

#include "type.h"

#if defined(unix) || defined(__APPLE__)
/*# include <unistd.h>
# include <utime.h>
# include <sys/types.h>*/
# include <sys/stat.h>
/*#else
# include <direct.h>
# include <io.h>*/
#endif

extern "C" {
#include <zip.h>
#include <unzip.h>
}

#ifdef _WIN32
    #define USEWIN32IOAPI
    #include "iowin32.h"
#endif

#if defined(__APPLE__) || defined(MAGIC3D_ANDROID)
    #define off64_t off_t
    #define fopen64 fopen
    #define fseeko64 fseeko
    #define ftello64 ftello
#endif

namespace Magic3D
{
#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

class DataBuffer;
class Package
{
private:
    std::list<std::string> files;
    std::string package;
    std::string password;

    unzFile packageFile;

    int compressLevel;
    bool overwrite;
    bool excludePath;

    uLong filetime(std::string& f, tm_zip* tmzip, uLong* dt);
    int check_exist_file(std::string& fileName);
    int getFileCrc(std::string& filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc);
    int isLargeFile(std::string& filename);

public:
    Package();
    virtual ~Package();

    void setPackage(std::string package);
    std::string getPackage();

    void open();
    bool isOpen();
    void close();
    void pack(std::string filesPath, void(*callBack)(std::string, bool));
    bool unpack(std::string fileName, DataBuffer* memory);

    void addFile(std::string fileName);
    void removeFile(std::string fileName);
    void clearFiles();
    std::list<std::string>* getFiles();
};

}

#endif // MAGIC3D_PACKAGE_H

