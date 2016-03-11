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

#ifndef MAGIC3D_INPUT_DEFINE_H
#define MAGIC3D_INPUT_DEFINE_H

namespace Magic3D
{

enum INPUT
{
    eINPUT_KEYBOARD,
    eINPUT_MOUSE,
    eINPUT_JOYSTICK,
    eINPUT_TOUCH,
    eINPUT_ACCELEROMETER,
    eINPUT_GYROSCOPE
};

enum EVENT
{
    eEVENT_KEYBOARD_DOWN,
    eEVENT_KEYBOARD_PRESSED,
    eEVENT_KEYBOARD_UP,

    eEVENT_MOUSE_DOWN,
    eEVENT_MOUSE_PRESSED,
    eEVENT_MOUSE_UP,
    eEVENT_MOUSE_MOVE,
    eEVENT_MOUSE_WHEEL,

    eEVENT_JOYSTICK_DOWN,
    eEVENT_JOYSTICK_PRESSED,
    eEVENT_JOYSTICK_UP,

    eEVENT_TOUCH_DOWN,
    eEVENT_TOUCH_PRESSED,
    eEVENT_TOUCH_UP,
    eEVENT_TOUCH_MOVE,
    
    eEVENT_ACCELEROMETER,
    eEVENT_GYROSCOPE
};

}

#endif // MAGIC3D_INPUT_DEFINE_H
