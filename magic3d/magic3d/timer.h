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

#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

#ifdef _WIN32
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
#include <sys/time.h>

typedef long long int64;
typedef unsigned long long uint64;
#endif

namespace Magic3D
{

class Timer
{
public:
    Timer();
    virtual ~Timer();

    void reset();

    float getTicks();
    float getTimeElapsed();
    float getTimeElapsedReal();
    float getCurrentTime();
    float getTimeSinceStart();

    int getFPS();

    void newFrame();

    void setReferenceMain();
    float getSinceReferenceMain();

    void setReference();
    float getSinceReference();

    void setScale(float scale);
    float getScale();

private:
#if defined(__WIN32__)
    double frequency;
    int64 startTimeReal;
    int64 startTime;
    int64 currentTime;
    int64 referenceMainTime;
    int64 referenceTime;
#else
    timeval startTimeReal;
    timeval startTime;
    timeval currentTime;
    timeval referenceMainTime;
    timeval referenceTime;
#endif

    float current;
    float elapsed;
    float elapsedReal;
    float timeSinceStart;

    float scale;

    int   frames;

    void updateFrequency();
    void update();
};

}

#endif
