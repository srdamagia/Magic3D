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

#ifndef MAGIC3D_EVENTLISTENER_H
#define MAGIC3D_EVENTLISTENER_H

#include <magic3d/input_define.h>

namespace Magic3D
{

class EventListener
{
private:
    INPUT inputType;

public:
    EventListener(INPUT input);
    virtual ~EventListener();

    INPUT getInputType();
};

class EventListener_Keyboard : public EventListener {
public:
    EventListener_Keyboard() : EventListener(eINPUT_KEYBOARD) {}
    virtual ~EventListener_Keyboard() {}

    virtual void down(int key) {if (key) {}}
    virtual void pressed(int key) {if (key) {}}
    virtual void up(int key) {if (key) {}}
};

class EventListener_Mouse : public EventListener {
public:
    EventListener_Mouse() : EventListener(eINPUT_MOUSE) {}
    virtual ~EventListener_Mouse() {}

    virtual void down(int x, int y, int button) {if (x || y || button) {}}
    virtual void pressed(int x, int y, int button) {if (x || y || button) {}}
    virtual void up(int x, int y, int button) {if (x || y || button) {}}
    virtual void move(int x, int y) {if (x || y) {}}
    virtual void wheel(int x, int y, int delta) {if (x || y || delta) {}}
};

class EventListener_Joystick : public EventListener {
public:
    EventListener_Joystick() : EventListener(eINPUT_JOYSTICK) {}
    virtual ~EventListener_Joystick() {}

    virtual void down(int button) {if (button) {}}
    virtual void pressed(int button) {if (button) {}}
    virtual void up(int button) {if (button) {}}
};

class EventListener_Touch : public EventListener {
public:
    EventListener_Touch() : EventListener(eINPUT_TOUCH) {}
    virtual ~EventListener_Touch() {}

    virtual void down(int x, int y, int finger) {if (x || y || finger) {}}
    virtual void pressed(int x, int y, int finger) {if (x || y || finger) {}}
    virtual void up(int x, int y, int finger) {if (x || y || finger) {}}
    virtual void move(int deltaX, int deltaY, int finger) {if (deltaX || deltaY || finger) {}}
};
    
class EventListener_Accelerometer : public EventListener {
public:
    EventListener_Accelerometer() : EventListener(eINPUT_ACCELEROMETER) {}
    virtual ~EventListener_Accelerometer() {}

    virtual void accelerometer(float x, float y, float z) {if (x || y || z) {}}
};

class EventListener_Gyroscope : public EventListener {
public:
    EventListener_Gyroscope() : EventListener(eINPUT_GYROSCOPE) {}
    virtual ~EventListener_Gyroscope() {}

    virtual void gyroscope(float x, float y, float z, float w) {if (x || y || z || w) {}}
};

}

#endif // MAGIC3D_EVENTLISTENER_H
