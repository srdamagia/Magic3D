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

#include "magic3d/timer.h"
#include "magic3d/log.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>

Magic3D::Timer::Timer()
{
    scale = 1.0f;
    current = 0.0f;
    elapsed = 0.0f;
    elapsedReal = 0.0f;

    timeSinceStart = 0.0f;

#if defined(__WIN32__)
    updateFrequency();
#endif

    reset();
    startTimeReal = startTime;
}

Magic3D::Timer::~Timer()
{

}

void Magic3D::Timer::update()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    elapsed = (float)((i.QuadPart - currentTime) * frequency);
    currentTime = i.QuadPart;
    current = (float)((currentTime - startTime) * frequency);
    timeSinceStart = (float)((currentTime - startTimeReal) * frequency);
#else
    timeval t;

    gettimeofday(&t, NULL);

    elapsed = ((float)(t.tv_sec - currentTime.tv_sec) * 1000.0f + (float)(t.tv_usec - currentTime.tv_usec) * 0.001f) * 0.001f;
    currentTime = t;
    current = ((float)(currentTime.tv_sec - startTime.tv_sec) * 1000.0f + (float)(currentTime.tv_usec - startTime.tv_usec) * 0.001f) * 0.001f;
    timeSinceStart = ((float)(currentTime.tv_sec - startTimeReal.tv_sec) * 1000.0f + (float)(currentTime.tv_usec - startTimeReal.tv_usec) * 0.001f) * 0.001f;
#endif

    elapsedReal = elapsed;
    elapsed = elapsed * scale;
}

float Magic3D::Timer::getCurrentTime()
{
    return current;
}

float Magic3D::Timer::getTimeSinceStart()
{
    return timeSinceStart;
}

void Magic3D::Timer::updateFrequency()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceFrequency(&i);
    frequency = i.QuadPart;
    if (frequency == 0.0)
    {
        Magic3D::Log::log(eLOG_FAILURE, "UpdateFrequency: could not determine the timer frequency\n");
    }
    else
    {
        frequency = 1.0 / frequency;
    }
#endif
}

void Magic3D::Timer::reset()
{
#if defined(__WIN32__)
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    startTime = i.QuadPart;
# else
    gettimeofday(&startTime, NULL);
#endif
    currentTime = startTime;
    referenceMainTime = startTime;
    referenceTime = startTime;
    frames = 0;
}

float Magic3D::Timer::getTicks()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    return (float)(i.QuadPart * frequency);
#else
    timeval t;

    gettimeofday(&t, NULL);

    return ((float)t.tv_sec * 1000.0f + (float)(t.tv_usec) * 0.001f) * 0.001f;
#endif
}

float Magic3D::Timer::getTimeElapsed()
{
    return elapsed;
}

float Magic3D::Timer::getTimeElapsedReal()
{
    return elapsedReal;
}

int Magic3D::Timer::getFPS()
{
    return frames;
}

void Magic3D::Timer::newFrame()
{
    update();

    frames++;
}

void Magic3D::Timer::setReferenceMain()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    referenceMainTime = i.QuadPart;
#else
    gettimeofday(&referenceMainTime, NULL);
#endif
}

float Magic3D::Timer::getSinceReferenceMain(){
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    return (float)((i.QuadPart - referenceMainTime) * frequency) * 1000.0f;
#else
    timeval t;

    gettimeofday(&t, NULL);

    return ((float)(t.tv_sec - referenceMainTime.tv_sec) * 1000.0f + (float)(t.tv_usec - referenceMainTime.tv_usec) * 0.001f);
#endif
}

void Magic3D::Timer::setReference()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    referenceTime = i.QuadPart;
#else
    gettimeofday(&referenceTime, NULL);
#endif
}

float Magic3D::Timer::getSinceReference()
{
#ifdef _WIN32
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    return (float)((i.QuadPart - referenceTime) * frequency) * 1000.0f;
#else
    timeval t;

    gettimeofday(&t, NULL);

    return ((float)(t.tv_sec - referenceTime.tv_sec) * 1000.0f + (float)(t.tv_usec - referenceTime.tv_usec) * 0.001f);
#endif
}

void Magic3D::Timer::setScale(float scale)
{
    this->scale = scale < 0.0f ? 0.0f : scale;
}

float Magic3D::Timer::getScale()
{
    return scale;
}
