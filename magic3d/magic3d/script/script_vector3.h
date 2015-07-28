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

#ifndef SCRIPT_VECTOR_H
#define SCRIPT_VECTOR_H

#include <magic3d/math.h>
#include <magic3d/script/script_basic.h>

namespace Magic3D
{
class ScriptVector3 : public ScriptBasic
{
private:
    Vector3 vector;
public:
    static const char className[];
    static Magic3D::ScriptClass<ScriptVector3>::ScriptEnum enums[];
    static Magic3D::ScriptClass<ScriptVector3>::ScriptFunction functions[];

    ScriptVector3(lua_State *lua);
    ScriptVector3(Vector3 vector);
    virtual ~ScriptVector3();

    int setX(lua_State *lua);
    int getX(lua_State *lua);

    int setY(lua_State *lua);
    int getY(lua_State *lua);

    int setZ(lua_State *lua);
    int getZ(lua_State *lua);

    int setValue(lua_State *lua);

    int sum(lua_State *lua);
    int subtract(lua_State *lua);
    int multiply(lua_State *lua);
    int scale(lua_State *lua);

    int dot(lua_State *lua);
    int cross(lua_State *lua);
    int distance(lua_State *lua);
    int normalized(lua_State *lua);

    int angle(lua_State *lua);
    int slerp(lua_State *lua);

    int getEulerRight(lua_State *lua);
    int getEulerUp(lua_State *lua);
    int getEulerFront(lua_State *lua);
    int getEulerLeft(lua_State *lua);
    int getEulerDown(lua_State *lua);
    int getEulerBack(lua_State *lua);

    Vector3& getValue();

    virtual std::string toString();
};
}
#endif /* SCRIPT_VECTOR_H */
