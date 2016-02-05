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

#ifndef MAGIC3D_SCRIPT_INPUT_H
#define MAGIC3D_SCRIPT_INPUT_H

#include <magic3d/eventlistener.h>
#include <magic3d/script/script.h>

namespace Magic3D
{

#define M3D_SCRIPT_KEYBOARD_DOWN    "on_keyboard_down"
#define M3D_SCRIPT_KEYBOARD_PRESSED "on_keyboard_pressed"
#define M3D_SCRIPT_KEYBOARD_UP      "on_keyboard_up"

#define M3D_SCRIPT_MOUSE_DOWN       "on_mouse_down"
#define M3D_SCRIPT_MOUSE_PRESSED    "on_mouse_pressed"
#define M3D_SCRIPT_MOUSE_UP         "on_mouse_up"
#define M3D_SCRIPT_MOUSE_MOVE       "on_mouse_move"
#define M3D_SCRIPT_MOUSE_WHEEL      "on_mouse_wheel"

#define M3D_SCRIPT_JOYSTICK_DOWN    "on_joystick_down"
#define M3D_SCRIPT_JOYSTICK_PRESSED "on_joystick_pressed"
#define M3D_SCRIPT_JOYSTICK_UP      "on_joystick_up"

#define M3D_SCRIPT_TOUCH_DOWN       "on_touch_down"
#define M3D_SCRIPT_TOUCH_PRESSED    "on_touch_pressed"
#define M3D_SCRIPT_TOUCH_UP         "on_touch_up"
#define M3D_SCRIPT_TOUCH_MOVE       "on_touch_move"

#define M3D_SCRIPT_ACCELEROMETER    "on_accelerometer"
#define M3D_SCRIPT_GYROSCOPE        "on_gyroscope"

class Script_Input_Keyboard : public EventListener_Keyboard {
public:
    virtual void down(int key);
    virtual void pressed(int key);
    virtual void up(int key);
};

class Script_Input_Mouse : public EventListener_Mouse {
public:
    virtual void down(int x, int y, int button);
    virtual void pressed(int x, int y, int button);
    virtual void up(int x, int y, int button);
    virtual void move(int x, int y);
    virtual void wheel(int x, int y, int delta);
};

class Script_Input_Joystick : public EventListener_Joystick {
public:
    virtual void down(int button);
    virtual void pressed(int button);
    virtual void up(int button);
};

class Script_Input_Touch : public EventListener_Touch {
public:
    virtual void down(int x, int y, int finger);
    virtual void pressed(int x, int y, int finger);
    virtual void up(int x, int y, int finger);
    virtual void move(int deltaX, int deltaY, int finger);
};

class Script_Input_Accelerometer : public EventListener_Accelerometer {
public:
    virtual void accelerometer(float x, float y, float z);
};

class Script_Input_Gyroscope : public EventListener_Gyroscope {
public:
    virtual void gyroscope(float x, float y, float z, float w);
};

}

#endif // MAGIC3D_SCRIPT_INPUT_H
