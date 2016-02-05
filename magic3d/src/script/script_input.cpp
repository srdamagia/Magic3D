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

#include <magic3d/script/script_input.h>

void Magic3D::Script_Input_Keyboard::down(int key)
{
    std::string function(M3D_SCRIPT_KEYBOARD_DOWN);
    if (Script::getInstance()->on_keyboard_down && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(key);
        Script::getInstance()->call(function, 1);
    }
}

void Magic3D::Script_Input_Keyboard::pressed(int key)
{
    std::string function(M3D_SCRIPT_KEYBOARD_PRESSED);
    if (Script::getInstance()->on_keyboard_pressed && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(key);
        Script::getInstance()->call(function, 1);
    }
}

void Magic3D::Script_Input_Keyboard::up(int key)
{
    std::string function(M3D_SCRIPT_KEYBOARD_UP);
    if (Script::getInstance()->on_keyboard_up && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(key);
        Script::getInstance()->call(function, 1);
    }
}

//********************************************************************************

void Magic3D::Script_Input_Mouse::down(int x, int y, int button)
{
    std::string function(M3D_SCRIPT_MOUSE_DOWN);
    if (Script::getInstance()->on_mouse_down && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Mouse::pressed(int x, int y, int button)
{
    std::string function(M3D_SCRIPT_MOUSE_PRESSED);
    if (Script::getInstance()->on_mouse_pressed && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Mouse::up(int x, int y, int button)
{
    std::string function(M3D_SCRIPT_MOUSE_UP);
    if (Script::getInstance()->on_mouse_up && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Mouse::move(int x, int y)
{
    std::string function(M3D_SCRIPT_MOUSE_MOVE);
    if (Script::getInstance()->on_mouse_move && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        //Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 2);
    }
}

void Magic3D::Script_Input_Mouse::wheel(int x, int y, int delta)
{
    std::string function(M3D_SCRIPT_MOUSE_WHEEL);
    if (Script::getInstance()->on_mouse_wheel && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(delta);
        Script::getInstance()->call(function, 3);
    }
}

//********************************************************************************

void Magic3D::Script_Input_Joystick::down(int button)
{
    std::string function(M3D_SCRIPT_JOYSTICK_DOWN);
    if (Script::getInstance()->on_joystick_down && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 1);
    }
}

void Magic3D::Script_Input_Joystick::pressed(int button)
{
    std::string function(M3D_SCRIPT_JOYSTICK_PRESSED);
    if (Script::getInstance()->on_joystick_pressed && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 1);
    }
}

void Magic3D::Script_Input_Joystick::up(int button)
{
    std::string function(M3D_SCRIPT_JOYSTICK_UP);
    if (Script::getInstance()->on_joystick_up && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(button);
        Script::getInstance()->call(function, 1);
    }
}

//********************************************************************************

void Magic3D::Script_Input_Touch::down(int x, int y, int finger)
{
    std::string function(M3D_SCRIPT_TOUCH_DOWN);
    if (Script::getInstance()->on_touch_down && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(finger);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Touch::pressed(int x, int y, int finger)
{
    std::string function(M3D_SCRIPT_TOUCH_PRESSED);
    if (Script::getInstance()->on_touch_pressed && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(finger);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Touch::up(int x, int y, int finger)
{
    std::string function(M3D_SCRIPT_TOUCH_UP);
    if (Script::getInstance()->on_touch_up && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(x);
        Script::getInstance()->pushInteger(y);
        Script::getInstance()->pushInteger(finger);
        Script::getInstance()->call(function, 3);
    }
}

void Magic3D::Script_Input_Touch::move(int deltaX, int deltaY, int finger)
{
    std::string function(M3D_SCRIPT_TOUCH_MOVE);
    if (Script::getInstance()->on_touch_move && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushInteger(deltaX);
        Script::getInstance()->pushInteger(deltaY);
        Script::getInstance()->pushInteger(finger);
        Script::getInstance()->call(function, 3);
    }
}

//********************************************************************************

void Magic3D::Script_Input_Accelerometer::accelerometer(float x, float y, float z)
{
    std::string function(M3D_SCRIPT_ACCELEROMETER);
    if (Script::getInstance()->on_accelerometer && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushNumber(x);
        Script::getInstance()->pushNumber(y);
        Script::getInstance()->pushNumber(z);
        Script::getInstance()->call(function, 3);
    }
}

//********************************************************************************

void Magic3D::Script_Input_Gyroscope::gyroscope(float x, float y, float z, float w)
{
    std::string function(M3D_SCRIPT_GYROSCOPE);
    if (Script::getInstance()->on_gyroscope && Script::getInstance()->startCall(function))
    {
        Script::getInstance()->pushNumber(x);
        Script::getInstance()->pushNumber(y);
        Script::getInstance()->pushNumber(z);
        Script::getInstance()->pushNumber(w);
        Script::getInstance()->call(function, 4);
    }
}
