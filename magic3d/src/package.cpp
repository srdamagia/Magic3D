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

#include <magic3d/package.h>
#include <magic3d/file.h>
#include <magic3d/log.h>

Magic3D::Package::Package()
{
    packageFile = NULL;
    package = "data.magic3d";
    password = MAGIC3D_PASSWORD;
    compressLevel = Z_DEFAULT_COMPRESSION;
    overwrite = true;
    excludePath = false;
}

Magic3D::Package::~Package()
{

}

void Magic3D::Package::setPackage(std::string package)
{
    this->package = package;
}

std::string Magic3D::Package::getPackage()
{
    return package;
}


void Magic3D::Package::open()
{
    if (!package.empty())
    {
#ifdef USEWIN32IOAPI
        zlib_filefunc64_def ffunc;
        fill_win32_filefunc64A(&ffunc);

        packageFile = unzOpen2_64(package.c_str(), &ffunc);
#else
        packageFile = unzOpen64(package.c_str());
#endif
    }

    if (packageFile == NULL)
    {
        Log::logFormat(eLOG_FAILURE, "Cannot open %s.", package.c_str());
    }
    else
    {
        Log::logFormat(eLOG_SUCCESS, "%s opened.", package.c_str());
    }
}

bool Magic3D::Package::isOpen()
{
    return packageFile != NULL;
}

void Magic3D::Package::close()
{
    if (packageFile != NULL)
    {
        unzClose(packageFile);
        packageFile = NULL;

        Log::logFormat(eLOG_SUCCESS, "%s closed.", package.c_str());
    }
}

void Magic3D::Package::pack(std::string filesPath, void(*callBack)(std::string, bool))
{
    std::string packFileName = package;
    bool zipok = false;
    int err = 0;
    int size_buf = WRITEBUFFERSIZE;
    void* buf = (void*)malloc(size_buf);

    if (buf == NULL)
    {
        Log::logFormat(eLOG_FAILURE, "Error allocating memory - error: %d.", ZIP_INTERNALERROR);
        return;
    }

    if (!packFileName.empty())
    {
        zipok = true;

        if (packFileName.find(".") ==  std::string::npos)
        {
            packFileName += ".magic3d";
        }
    }

    if (zipok)
    {
        zipFile zf;
        int errclose;
#ifdef USEWIN32IOAPI
        zlib_filefunc64_def ffunc;
        fill_win32_filefunc64A(&ffunc);
        zf = zipOpen2_64(packFileName.c_str(), overwrite ? 0 : 2, NULL, &ffunc);
#else
        zf = zipOpen64(fileName.c_str(), overwrite ? 0 : 2);
#        endif

        if (zf == NULL)
        {
            Log::logFormat(eLOG_FAILURE, "Error opening %s.", packFileName.c_str());
            err = ZIP_ERRNO;
        }
        else
        {
            Log::logFormat(eLOG_PLAINTEXT, "Creating %s.", packFileName.c_str());
        }

        std::list<std::string>::const_iterator it_f = files.begin();
        while (it_f != files.end() && (err == ZIP_OK))
        {
            std::string fileNameInPack = *it_f++;
            std::string fullFileName = filesPath + fileNameInPack;
            FILE * fin;
            int size_read;
            zip_fileinfo zi;
            unsigned long crcFile=0;
            int zip64 = 0;

            zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
            zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
            zi.dosDate = 0;
            zi.internal_fa = 0;
            zi.external_fa = 0;
            filetime(fullFileName, &zi.tmz_date, &zi.dosDate);

            if (!password.empty() && err == ZIP_OK)
            {
                err = getFileCrc(fullFileName, buf, size_buf, &crcFile);
            }

            zip64 = isLargeFile(fullFileName);


            std::string savedFile = fileNameInPack;
            while (savedFile.find('\\') == 0 || savedFile.find('/') == 0)
            {
                savedFile = savedFile.substr(1, savedFile.size() - 1);
            }

            /*should the zip file contain any path at all?*/
            if (excludePath)
            {
                size_t pos = savedFile.find_last_of('/');
                if (pos != std::string::npos)
                {
                    pos = pos + 1;
                    savedFile = savedFile.substr(pos, savedFile.size() - pos);
                }
                pos = savedFile.find_last_of('\\');
                if (pos != std::string::npos)
                {
                    pos = pos + 1;
                    savedFile = savedFile.substr(pos, savedFile.size() - pos);
                }
            }

            err = zipOpenNewFileInZip3_64(zf, savedFile.c_str(), &zi, NULL, 0, NULL, 0, NULL/* comment*/, (compressLevel != 0) ? Z_DEFLATED : 0, compressLevel, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password.c_str(), crcFile, zip64);

            if (err != ZIP_OK)
            {
                Log::logFormat(eLOG_FAILURE, "Error in opening %s in package file.", fileNameInPack.c_str());
            }
            else
            {
                fin = fopen64(fullFileName.c_str(), "rb");
                if (fin == NULL)
                {
                    err = ZIP_ERRNO;
                    Log::logFormat(eLOG_FAILURE, "Error in opening %s for reading.", fullFileName.c_str());
                }
            }

            if (err == ZIP_OK)
            {
                do
                {
                    err = ZIP_OK;
                    size_read = (int)fread(buf,1,size_buf,fin);
                    if (size_read < size_buf)
                        if (feof(fin)==0)
                    {
                        Log::logFormat(eLOG_FAILURE, "Error in reading %s.", fullFileName.c_str());
                        err = ZIP_ERRNO;
                    }

                    if (size_read > 0)
                    {
                        err = zipWriteInFileInZip (zf,buf,size_read);
                        if (err < 0)
                        {
                            Log::logFormat(eLOG_FAILURE, "Error in writing %s in the package file.", fileNameInPack.c_str());
                        }

                    }
                } while ((err == ZIP_OK) && (size_read>0));
            }

            if (fin)
            {
                fclose(fin);
            }

            if (err < 0)
            {
                err = ZIP_ERRNO;
            }
            else
            {
                err = zipCloseFileInZip(zf);
                if (err != ZIP_OK)
                {
                    Log::logFormat(eLOG_FAILURE, "Error in closing %s in the package file.", fileNameInPack.c_str());
                }
            }

            if (callBack)
            {
                callBack(fileNameInPack, err == ZIP_OK);
            }
        }

        errclose = zipClose(zf, NULL);
        if (errclose != ZIP_OK)
        {
            Log::logFormat(eLOG_FAILURE, "Error in closing %s.", packFileName.c_str());
        }
        else
        {
            Log::logFormat(eLOG_SUCCESS, "File %s sucessfully packed.", packFileName.c_str());
        }
    }
    else
    {
       Log::logFormat(eLOG_FAILURE, "Can't create file: %s.", packFileName.c_str());
    }

    free(buf);
}

bool Magic3D::Package::unpack(std::string fileName, DataBuffer* memory)
{
    bool result = false;
    if (isOpen())
    {
        if (unzLocateFile(packageFile, fileName.c_str(), 0) == UNZ_OK)
        {
            int err = UNZ_OK;
            void* buf;
            uInt size_buf;

            unz_file_info64 file_info;
            err = unzGetCurrentFileInfo64(packageFile, &file_info, (char*)fileName.c_str(), fileName.size(), NULL, 0, NULL, 0);

            if (err == UNZ_OK)
            {
                size_buf = WRITEBUFFERSIZE;
                buf = (void*)malloc(size_buf);
                if (buf == NULL)
                {
                    Log::logFormat(eLOG_FAILURE, "Error allocating memory %d.", UNZ_INTERNALERROR);
                    return result;
                }

                err = unzOpenCurrentFilePassword(packageFile, password.c_str());
                if (err != UNZ_OK)
                {
                    Log::logFormat(eLOG_FAILURE, "Error %d with package in password.", err);
                }

                do
                {
                    err = unzReadCurrentFile(packageFile, buf, size_buf);
                    if (err < 0)
                    {
                        Log::logFormat(eLOG_FAILURE, "Error %d with package in reading file.", err);
                        break;
                    }
                    if (err > 0)
                    {
                        if (memory->write((const char*)buf, err) == NULL)
                        {
                            Log::logFormat(eLOG_FAILURE, "Error in writing extracted file.");
                            err = UNZ_ERRNO;
                            break;
                        }
                    }
                }
                while (err > 0);


                if (err == UNZ_OK)
                {
                    result = true;
                    err = unzCloseCurrentFile(packageFile);
                    if (err != UNZ_OK)
                    {
                        printf("error %d with zipfile in colose current file.", err);
                    }
                }
                else
                {
                    unzCloseCurrentFile(packageFile); /* don't lose the error */
                }

                free(buf);
            }
            else
            {
                Log::logFormat(eLOG_FAILURE, "Error %d with package in getting information.", err);
            }
        }
        else
        {
            Log::logFormat(eLOG_FAILURE, "File %s not found in the package.", fileName.c_str());
        }
    }

    return result;
}

void Magic3D::Package::addFile(std::string fileName)
{
    files.push_back(fileName);
}

void Magic3D::Package::removeFile(std::string fileName)
{
    files.remove(fileName);
}

void Magic3D::Package::clearFiles()
{
    files.clear();
}

std::list<std::string>* Magic3D::Package::getFiles()
{
    return &files;
}

#ifdef _WIN32
uLong Magic3D::Package::filetime(std::string& f, tm_zip* tmzip, uLong* dt)
{
    int ret = 0;

    FILETIME ftLocal;
    HANDLE hFind;
    WIN32_FIND_DATAA ff32;

    hFind = FindFirstFileA(f.c_str(), &ff32);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
        FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
        FindClose(hFind);
        ret = 1;

        if (tmzip)
        {

        }
    }

    return ret;
}
#else
#ifdef unix
uLong Magic3D::Package::filetime(std::string& f, tm_zip* tmzip, uLong* dt)
{
    int ret = 0;
    struct stat s;        /* results of stat() */
    struct tm* filedate;
    time_t tm_t = 0;

    if (strcmp(f, "-") != 0)
    {
        char name[MAXFILENAME + 1];
        int len = strlen(f);
        if (len > MAXFILENAME)
        {
           len = MAXFILENAME;
        }

        strncpy(name, f, MAXFILENAME - 1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        name[ MAXFILENAME ] = '\0';

        if (name[len - 1] == '/')
        {
            name[len - 1] = '\0';
        }
        /* not all systems allow stat'ing a file with / appended */
        if (stat(name, &s)==0)
        {
            tm_t = s.st_mtime;
            ret = 1;
        }
    }
    filedate = localtime(&tm_t);

    tmzip->tm_sec  = filedate->tm_sec;
    tmzip->tm_min  = filedate->tm_min;
    tmzip->tm_hour = filedate->tm_hour;
    tmzip->tm_mday = filedate->tm_mday;
    tmzip->tm_mon  = filedate->tm_mon;
    tmzip->tm_year = filedate->tm_year;

    return ret;
}
#else
uLong Magic3D::Package::filetime(std::string& f, tm_zip* tmzip, uLong* dt)
{
    return 0;
}
#endif
#endif

int Magic3D::Package::check_exist_file(std::string& fileName)
{
    FILE* ftestexist;
    int ret = 1;
    ftestexist = fopen64(fileName.c_str(), "rb");
    if (ftestexist == NULL)
    {
        ret = 0;
    }
    else
    {
        fclose(ftestexist);
    }
    return ret;
}

/*void do_help()
{
    printf("Usage : minizip [-o] [-a] [-0 to -9] [-p password] [-j] file.zip [files_to_add]\n\n" \
           "  -o  Overwrite existing file.zip\n" \
           "  -a  Append to existing file.zip\n" \
           "  -0  Store only\n" \
           "  -1  Compress faster\n" \
           "  -9  Compress better\n\n" \
           "  -j  exclude path. store only the file name.\n\n");
}*/

/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
int Magic3D::Package::getFileCrc(std::string& filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc)
{
   unsigned long calculate_crc = 0;
   int err = ZIP_OK;
   FILE * fin = fopen64(filenameinzip.c_str(), "rb");
   unsigned long size_read = 0;
   unsigned long total_read = 0;
   if (fin == NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
    {
        do
        {
            err = ZIP_OK;
            size_read = (int)fread(buf, 1, size_buf, fin);
            if (size_read < size_buf)
            {
                if (feof(fin)==0)
                {
                    Log::logFormat(eLOG_FAILURE, "Error in reading %s.", filenameinzip.c_str());
                    err = ZIP_ERRNO;
                }
            }

            if (size_read > 0)
            {
                calculate_crc = crc32(calculate_crc, (const Bytef*)buf, size_read);
            }
            total_read += size_read;

        } while ((err == ZIP_OK) && (size_read>0));
    }

    if (fin)
    {
        fclose(fin);
    }

    *result_crc = calculate_crc;
    //Log::logFormat(eLOG_RENDERER, "File %s crc %lx.", filenameinzip.c_str(), calculate_crc);
    return err;
}

int Magic3D::Package::isLargeFile(std::string& filename)
{
    int largeFile = 0;
    ZPOS64_T pos = 0;
    FILE* pFile = fopen64(filename.c_str(), "rb");

    if(pFile != NULL)
    {
        /*int n = */fseeko64(pFile, 0, SEEK_END);

        pos = ftello64(pFile);

        Log::logFormat(eLOG_RENDERER, "File %s is %lld bytes.", filename.c_str(), pos);

        if(pos >= 0xffffffff)
        {
            largeFile = 1;
        }

        fclose(pFile);
    }

    return largeFile;
}
