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

#include <time.h>
#include <magic3d/magic3d.h>

Magic3D::Log* Magic3D::Log::instance = NULL;
char* Magic3D::Log::logFile = NULL;

Magic3D::Log::Log()
{
    callback = NULL;
    clear();
}

Magic3D::Log::~Log()
{
    callback = NULL;
    if (logFile)
    {
        delete[] logFile;
        logFile = NULL;
    }
}

void Magic3D::Log::start()
{
    if (!instance)
    {
        instance = new Log();
    }
}

void Magic3D::Log::finish()
{
    if (instance)
    {
        delete instance;
    }
}

Magic3D::Log* Magic3D::Log::getInstance()
{
    return instance;
}

void Magic3D::Log::setLogFile(const char* fileName)
{
    int length = strlen(fileName);
    logFile = new char[length + 1];
    sprintf(logFile, "%s", fileName);
}

bool Magic3D::Log::clear() {
#ifdef MAGIC3D_LOG
    FILE*  pFile;
    time_t ttime;
    tm* pCurTime;
    char cLogStart[128];

    if (logFile == NULL)
    {
        setLogFile(std::string(Magic3D::Magic3D::getApplicationPath() + "magic3d.log.html").c_str());
    }

    time(&ttime);
    pCurTime = localtime(&ttime);
    strftime(cLogStart, 128, "Magic3D Log Started on %m/%d/%Y at %H:%M:%S", pCurTime);

    pFile = fopen(logFile, "wt");

    if(!pFile) {
        return false;
    }

    fprintf(pFile, "<HTML>\n<TITLE>%s</TITLE>\n"
            "<BODY BGCOLOR= \"#000000\">\n"
            "<FONT COLOR= \"#FFFFFF\">%s</FONT><BR><BR>\n"
            "</BODY>\n</HTML>", cLogStart, cLogStart);

    fclose(pFile);
#endif
    return true;
}

void Magic3D::Log::write(LOG type, const char* text) {
#ifdef MAGIC3D_LOG
    FILE* pFile;
    time_t ttime;
    tm* pCurTime;

    char szLogTime[32];

    time(&ttime);
    pCurTime = localtime(&ttime);
    strftime(szLogTime, 32, "<PRE>%H:%M:%S    ", pCurTime);

    pFile = fopen(logFile, "r+");
    if (pFile) {
        fseek(pFile, -16, SEEK_END);
        const char* color = getColor(type);
        const char* format = "<FONT COLOR= \"%s\">%s %s</PRE></FONT>\n";
        fprintf( pFile, format, color, szLogTime, text);
        fprintf( pFile, "</BODY>\n</HTML>");
        if (callback)
        {
            int size = strlen(format) + strlen(color) + strlen(szLogTime) + strlen(text);
            char* logCallBack = new char[size + 1];
            sprintf(logCallBack, format, color, szLogTime, text);
            callback->log(type, logCallBack);
            delete[] logCallBack;
        }
        fclose( pFile );
    }
#endif
}

void Magic3D::Log::logFormat(LOG type, const char* text, ...)
{
    char szParsedString[4096];
    va_list va;
    va_start(va, text);    
    vsprintf(szParsedString, text, va);
    getInstance()->write(type, szParsedString);
    va_end(va);
}

void Magic3D::Log::log(LOG type, const char* text)
{
    getInstance()->write(type, text);
}

void Magic3D::Log::logMatrix(LOG type, float* matrix)
{
    float* value = matrix;
    for (int r = 0; r < 4; r++)
    {
        float v0 = *value; value++;
        float v1 = *value; value++;
        float v2 = *value; value++;
        float v3 = *value; value++;
        logFormat(type, "%8.9f, %8.9f, %8.9f, %8.9f", v0, v1, v2, v3);
    }
}

void Magic3D::Log::debug(const char* file, int line) {
    logFormat(eLOG_FAILURE, "File: %s - Line: %d", file, line);
}


const char* Magic3D::Log::getColor(LOG type) {
    switch( type )	{
        case eLOG_SUCCESS:   return "#00FF00"; break;
        case eLOG_FAILURE:   return "#FF0000"; break;
        case eLOG_PLAINTEXT: return "#FFFFFF"; break;
        case eLOG_RENDERER:  return "#4995B1"; break;
    }

    return NULL;
}

void Magic3D::Log::setCallBack(LogCallBack* callback)
{
    getInstance()->callback = callback;
}
