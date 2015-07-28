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

#ifndef MAGIC3D_JOYSTICK_H
#define MAGIC3D_JOYSTICK_H

#include <magic3d/eventlistener.h>
#include <magic3d/gui/gui.h>

namespace Magic3D
{

class Joystick;
class Joystick_Input_Mouse : public EventListener_Mouse
{
private:
    Joystick* joystick;
public:
    Joystick_Input_Mouse(Joystick* joystick);
    virtual void down(int x, int y, int button);
    virtual void pressed(int x, int y, int button);
    virtual void up(int x, int y, int button);
    virtual void move(int deltaX, int deltaY);
    virtual void wheel(int x, int y, int delta);
};

class Joystick_Input_Touch : public EventListener_Touch
{
private:
    Joystick* joystick;
public:
    Joystick_Input_Touch(Joystick* joystick);
    virtual void down(int x, int y, int finger);
    virtual void pressed(int x, int y, int finger);
    virtual void up(int x, int y, int finger);
    virtual void move(int deltaX, int deltaY, int finger);
};

class Joystick : public GUI
{
private:
    Vector3 joyDir;
    Vector3 joyPos;
    Vector3 startPos;
    float force;
    int button;

    Joystick_Input_Mouse* inputMouse;
    Joystick_Input_Touch* inputTouch;

    void init();

protected:
    Joystick(const Joystick& joystick, std::string name);

public:
    Joystick(std::string name);
    virtual ~Joystick();
    virtual Joystick* spawn(std::string name) const;

    Vector3 getDirection();
    float getForce();

    virtual bool updateMeshes();

    void calculateDirection(float x, float y);

    void down(int x, int y, int button);
    void pressed(int x, int y, int button);
    void up(int x, int y, int button);
    void move(int deltaX, int deltaY, int button);
};

}

#endif // MAGIC3D_JOYSTICK_H
