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

#ifndef SCRIPT_COLOR_H
#define SCRIPT_COLOR_H

#include <magic3d/math.h>
#include <magic3d/script/script_basic.h>

namespace Magic3D
{
class ScriptColor : public ScriptBasic
{
private:
    ColorRGBA color;
public:
    static const char className[];
    static Magic3D::ScriptClass<ScriptColor>::ScriptEnum enums[];
    static Magic3D::ScriptClass<ScriptColor>::ScriptFunction functions[];

    ScriptColor(lua_State *lua);
    ScriptColor(ColorRGBA color);
    virtual ~ScriptColor();

    int setR(lua_State *lua);
    int getR(lua_State *lua);

    int setG(lua_State *lua);
    int getG(lua_State *lua);

    int setB(lua_State *lua);
    int getB(lua_State *lua);

    int setA(lua_State *lua);
    int getA(lua_State *lua);

    int setValue(lua_State *lua);
    ColorRGBA& getValue();
};
}
#endif /* SCRIPT_COLOR_H */
