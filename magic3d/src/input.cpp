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

#include <magic3d/input.h>

Magic3D::Input* Magic3D::Input::instance = NULL;

Magic3D::Input::Input()
{

}

Magic3D::Input::~Input()
{
    while (!events_keyboard.empty())
    {
        EventListener* listener = *events_keyboard.begin();
        events_keyboard.erase(events_keyboard.begin());
        if (listener)
        {
            delete listener;
        }
    }

    while (!events_mouse.empty())
    {
        EventListener* listener = *events_mouse.begin();
        events_mouse.erase(events_mouse.begin());
        if (listener)
        {
            delete listener;
        }
    }

    while (!events_joystick.empty())
    {
        EventListener* listener = *events_joystick.begin();
        events_joystick.erase(events_joystick.begin());
        if (listener)
        {
            delete listener;
        }
    }

    while (!events_touch.empty())
    {
        EventListener* listener = *events_touch.begin();
        events_touch.erase(events_touch.begin());
        if (listener)
        {
            delete listener;
        }
    }

    while (!events_accelerometer.empty())
    {
        EventListener* listener = *events_accelerometer.begin();
        events_accelerometer.erase(events_accelerometer.begin());
        if (listener)
        {
            delete listener;
        }
    }

    while (!events_gyroscope.empty())
    {
        EventListener* listener = *events_gyroscope.begin();
        events_gyroscope.erase(events_gyroscope.begin());
        if (listener)
        {
            delete listener;
        }
    }
}

bool Magic3D::Input::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new Input();
    }

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Input sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Input not started.");
    }

    return result;
}

bool Magic3D::Input::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Input sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Input not finished.");
    }

    return !instance;
}

Magic3D::Input* Magic3D::Input::getInstance()
{
    return instance;
}

void Magic3D::Input::addEventListener(EventListener* listener)
{
    if (listener)
    {
        switch (listener->getInputType())
        {
            case eINPUT_KEYBOARD:      events_keyboard.push_back(listener); break;
            case eINPUT_MOUSE:         events_mouse.push_back(listener); break;
            case eINPUT_JOYSTICK:      events_joystick.push_back(listener); break;
            case eINPUT_TOUCH:         events_touch.push_back(listener); break;
            case eINPUT_ACCELEROMETER: events_accelerometer.push_back(listener); break;
            case eINPUT_GYROSCOPE:     events_gyroscope.push_back(listener); break;
        }
    }
}

void Magic3D::Input::removeEventListener(EventListener* listener)
{
    if (listener)
    {
        std::vector<EventListener*>* events = NULL;
        switch (listener->getInputType())
        {
            case eINPUT_KEYBOARD:      events = &events_keyboard; break;
            case eINPUT_MOUSE:         events = &events_mouse; break;
            case eINPUT_JOYSTICK:      events = &events_joystick; break;
            case eINPUT_TOUCH:         events = &events_touch; break;
            case eINPUT_ACCELEROMETER: events = &events_accelerometer; break;
            case eINPUT_GYROSCOPE:     events = &events_gyroscope; break;
        }
        if (events)
        {
            std::vector<EventListener*>::iterator itor = events->begin();
            while (itor != events->end())
            {
                EventListener* lstr = *itor;
                if (listener == lstr)
                {
                    events->erase(itor);
                    break;
                }
                itor++;
            }
        }
    }
}

void Magic3D::Input::dispatchEvent(INPUT input, EVENT event, int value)
{
    std::vector<EventListener*>* list = NULL;

    switch (input)
    {
        case eINPUT_KEYBOARD:      list = &events_keyboard; break;
        case eINPUT_MOUSE:         list = &events_mouse; break;
        case eINPUT_JOYSTICK:      list = &events_joystick; break;
        case eINPUT_TOUCH:         list = &events_touch; break;
        case eINPUT_ACCELEROMETER: list = &events_accelerometer; break;
        case eINPUT_GYROSCOPE:     list = &events_gyroscope; break;
    }

    std::vector<EventListener*>::const_iterator itor = list->begin();
    while (itor != list->end())
    {
        EventListener* listener = *itor++;

        switch (event)
        {
            case eEVENT_KEYBOARD_DOWN:    ((EventListener_Keyboard*)listener)->down(value); break;
            case eEVENT_KEYBOARD_PRESSED: ((EventListener_Keyboard*)listener)->pressed(value); break;
            case eEVENT_KEYBOARD_UP:      ((EventListener_Keyboard*)listener)->up(value); break;

            case eEVENT_MOUSE_DOWN:       ((EventListener_Mouse*)listener)->down(0, 0, value); break;
            case eEVENT_MOUSE_PRESSED:    ((EventListener_Mouse*)listener)->pressed(0, 0, value); break;
            case eEVENT_MOUSE_UP:         ((EventListener_Mouse*)listener)->up(0, 0, value); break;
            case eEVENT_MOUSE_MOVE:       ((EventListener_Mouse*)listener)->move(0, 0); break;
            case eEVENT_MOUSE_WHEEL:      ((EventListener_Mouse*)listener)->wheel(0, 0, 0); break;

            case eEVENT_JOYSTICK_DOWN:    ((EventListener_Joystick*)listener)->down(value); break;
            case eEVENT_JOYSTICK_PRESSED: ((EventListener_Joystick*)listener)->pressed(value); break;
            case eEVENT_JOYSTICK_UP:      ((EventListener_Joystick*)listener)->up(value); break;

            case eEVENT_TOUCH_DOWN:       ((EventListener_Touch*)listener)->down(0, 0, value); break;
            case eEVENT_TOUCH_PRESSED:    ((EventListener_Touch*)listener)->pressed(0, 0, value); break;
            case eEVENT_TOUCH_UP:         ((EventListener_Touch*)listener)->up(0, 0, value); break;
            case eEVENT_TOUCH_MOVE:       ((EventListener_Touch*)listener)->move(0, 0, value); break;

            case eEVENT_ACCELEROMETER:    ((EventListener_Accelerometer*)listener)->accelerometer(0.0f, 0.0f, 0.0f); break;
            case eEVENT_GYROSCOPE:        ((EventListener_Gyroscope*)listener)->gyroscope(0.0f, 0.0f, 0.0f, 0.0f); break;
        }
    }
}

void Magic3D::Input::dispatchEvent(INPUT input, EVENT event, int x, int y, int value)
{
    std::vector<EventListener*>* list = NULL;

    switch (input)
    {
        case eINPUT_KEYBOARD:      list = &events_keyboard; break;
        case eINPUT_MOUSE:         list = &events_mouse; break;
        case eINPUT_JOYSTICK:      list = &events_joystick; break;
        case eINPUT_TOUCH:         list = &events_touch; break;
        case eINPUT_ACCELEROMETER: list = &events_accelerometer; break;
        case eINPUT_GYROSCOPE:     list = &events_gyroscope; break;
    }

    int fx = x;
    int fy = y;

    Window* window = Renderer::getInstance()->getWindow();

    if (window)
    {
        switch (window->getOrientation())
        {
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                fx = window->getWidth() - x;
                fy = window->getHeight() - y;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            {
                fx = y;
                fy = window->getHeight() - x;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                fx = window->getWidth() - y;
                fy = x;
                break;
            }

            default: break;
        }
    }


    std::vector<EventListener*>::const_iterator itor = list->begin();
    while (itor != list->end())
    {
        EventListener* listener = *itor++;

        switch (event)
        {
            case eEVENT_KEYBOARD_DOWN:    ((EventListener_Keyboard*)listener)->down(value); break;
            case eEVENT_KEYBOARD_PRESSED: ((EventListener_Keyboard*)listener)->pressed(value); break;
            case eEVENT_KEYBOARD_UP:      ((EventListener_Keyboard*)listener)->up(value); break;

            case eEVENT_MOUSE_DOWN:       ((EventListener_Mouse*)listener)->down(fx, fy, value); break;
            case eEVENT_MOUSE_PRESSED:    ((EventListener_Mouse*)listener)->pressed(fx, fy, value); break;
            case eEVENT_MOUSE_UP:         ((EventListener_Mouse*)listener)->up(fx, fy, value); break;
            case eEVENT_MOUSE_MOVE:       ((EventListener_Mouse*)listener)->move(fx, fy); break;
            case eEVENT_MOUSE_WHEEL:      ((EventListener_Mouse*)listener)->wheel(fx, fy, value); break;

            case eEVENT_JOYSTICK_DOWN:    ((EventListener_Joystick*)listener)->down(value); break;
            case eEVENT_JOYSTICK_PRESSED: ((EventListener_Joystick*)listener)->pressed(value); break;
            case eEVENT_JOYSTICK_UP:      ((EventListener_Joystick*)listener)->up(value); break;

            case eEVENT_TOUCH_DOWN:       ((EventListener_Touch*)listener)->down(fx, fy, value); break;
            case eEVENT_TOUCH_PRESSED:    ((EventListener_Touch*)listener)->pressed(fx, fy, value); break;
            case eEVENT_TOUCH_UP:         ((EventListener_Touch*)listener)->up(fx, fy, value); break;
            case eEVENT_TOUCH_MOVE:       ((EventListener_Touch*)listener)->move(fx, fy, value); break;

            case eEVENT_ACCELEROMETER:    ((EventListener_Accelerometer*)listener)->accelerometer(fx, fy, 0.0f); break;
            case eEVENT_GYROSCOPE:        ((EventListener_Gyroscope*)listener)->gyroscope(fx, fy, 0.0f, 0.0f); break;
        }
    }
}

void Magic3D::Input::dispatchEvent(INPUT input, EVENT event, float x, float y, float z)
{
    std::vector<EventListener*>* list = NULL;

    switch (input)
    {
        case eINPUT_KEYBOARD:      list = &events_keyboard; break;
        case eINPUT_MOUSE:         list = &events_mouse; break;
        case eINPUT_JOYSTICK:      list = &events_joystick; break;
        case eINPUT_TOUCH:         list = &events_touch; break;
        case eINPUT_ACCELEROMETER: list = &events_accelerometer; break;
        case eINPUT_GYROSCOPE:     list = &events_gyroscope; break;
    }

    float fx = x;
    float fy = y;
    float fz = z;

    Window* window = Renderer::getInstance()->getWindow();

    if (window)
    {
        switch (window->getOrientation())
        {
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                fx = -x;
                fy = -y;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            {
                fx = -y;
                fy = x;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                fx = y;
                fy = -x;
                break;
            }

            default: break;
        }
    }


    std::vector<EventListener*>::const_iterator itor = list->begin();
    while (itor != list->end())
    {
        EventListener* listener = *itor++;

        switch (event)
        {
            case eEVENT_KEYBOARD_DOWN:    ((EventListener_Keyboard*)listener)->down(fz); break;
            case eEVENT_KEYBOARD_PRESSED: ((EventListener_Keyboard*)listener)->pressed(fz); break;
            case eEVENT_KEYBOARD_UP:      ((EventListener_Keyboard*)listener)->up(fz); break;

            case eEVENT_MOUSE_DOWN:       ((EventListener_Mouse*)listener)->down(fx, fy, fz); break;
            case eEVENT_MOUSE_PRESSED:    ((EventListener_Mouse*)listener)->pressed(fx, fy, fz); break;
            case eEVENT_MOUSE_UP:         ((EventListener_Mouse*)listener)->up(fx, fy, fz); break;
            case eEVENT_MOUSE_MOVE:       ((EventListener_Mouse*)listener)->move(fx, fy); break;
            case eEVENT_MOUSE_WHEEL:       ((EventListener_Mouse*)listener)->wheel(fx, fy, fz); break;

            case eEVENT_JOYSTICK_DOWN:    ((EventListener_Joystick*)listener)->down(fz); break;
            case eEVENT_JOYSTICK_PRESSED: ((EventListener_Joystick*)listener)->pressed(fz); break;
            case eEVENT_JOYSTICK_UP:      ((EventListener_Joystick*)listener)->up(fz); break;

            case eEVENT_TOUCH_DOWN:       ((EventListener_Touch*)listener)->down(fx, fy, fz); break;
            case eEVENT_TOUCH_PRESSED:    ((EventListener_Touch*)listener)->pressed(fx, fy, fz); break;
            case eEVENT_TOUCH_UP:         ((EventListener_Touch*)listener)->up(fx, fy, fz); break;
            case eEVENT_TOUCH_MOVE:       ((EventListener_Touch*)listener)->move(fx, fy, fz); break;

            case eEVENT_ACCELEROMETER:    ((EventListener_Accelerometer*)listener)->accelerometer(fx, fy, fz); break;
            case eEVENT_GYROSCOPE:        ((EventListener_Gyroscope*)listener)->gyroscope(fx, fy, fz, 0.0f); break;
        }
    }
}

void Magic3D::Input::dispatchEvent(INPUT input, EVENT event, float x, float y, float z, float w)
{
    std::vector<EventListener*>* list = NULL;

    switch (input)
    {
        case eINPUT_KEYBOARD:      list = &events_keyboard; break;
        case eINPUT_MOUSE:         list = &events_mouse; break;
        case eINPUT_JOYSTICK:      list = &events_joystick; break;
        case eINPUT_TOUCH:         list = &events_touch; break;
        case eINPUT_ACCELEROMETER: list = &events_accelerometer; break;
        case eINPUT_GYROSCOPE:     list = &events_gyroscope; break;
    }

    float fx = x;
    float fy = y;
    float fz = z;

    Window* window = Renderer::getInstance()->getWindow();

    if (window)
    {
        switch (window->getOrientation())
        {
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                fx = -x;
                fy = -y;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            {
                fx = -y;
                fy = x;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                fx = y;
                fy = -x;
                break;
            }

            default: break;
        }
    }


    std::vector<EventListener*>::const_iterator itor = list->begin();
    while (itor != list->end())
    {
        EventListener* listener = *itor++;

        switch (event)
        {
            case eEVENT_KEYBOARD_DOWN:    ((EventListener_Keyboard*)listener)->down(fz); break;
            case eEVENT_KEYBOARD_PRESSED: ((EventListener_Keyboard*)listener)->pressed(fz); break;
            case eEVENT_KEYBOARD_UP:      ((EventListener_Keyboard*)listener)->up(fz); break;

            case eEVENT_MOUSE_DOWN:       ((EventListener_Mouse*)listener)->down(fx, fy, fz); break;
            case eEVENT_MOUSE_PRESSED:    ((EventListener_Mouse*)listener)->pressed(fx, fy, fz); break;
            case eEVENT_MOUSE_UP:         ((EventListener_Mouse*)listener)->up(fx, fy, fz); break;
            case eEVENT_MOUSE_MOVE:       ((EventListener_Mouse*)listener)->move(fx, fy); break;
            case eEVENT_MOUSE_WHEEL:       ((EventListener_Mouse*)listener)->wheel(fx, fy, fz); break;

            case eEVENT_JOYSTICK_DOWN:    ((EventListener_Joystick*)listener)->down(fz); break;
            case eEVENT_JOYSTICK_PRESSED: ((EventListener_Joystick*)listener)->pressed(fz); break;
            case eEVENT_JOYSTICK_UP:      ((EventListener_Joystick*)listener)->up(fz); break;

            case eEVENT_TOUCH_DOWN:       ((EventListener_Touch*)listener)->down(fx, fy, fz); break;
            case eEVENT_TOUCH_PRESSED:    ((EventListener_Touch*)listener)->pressed(fx, fy, fz); break;
            case eEVENT_TOUCH_UP:         ((EventListener_Touch*)listener)->up(fx, fy, fz); break;
            case eEVENT_TOUCH_MOVE:       ((EventListener_Touch*)listener)->move(fx, fy, fz); break;

            case eEVENT_ACCELEROMETER:    ((EventListener_Accelerometer*)listener)->accelerometer(fx, fy, fz); break;
            case eEVENT_GYROSCOPE:        ((EventListener_Gyroscope*)listener)->gyroscope(x, y, z, w); break;
        }
    }
}
