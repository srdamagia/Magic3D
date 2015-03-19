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

#include <magic3d/script/script_color.h>
#include <magic3d/magic3d.h>

const char Magic3D::ScriptColor::className[] = "Color";

Magic3D::ScriptClass<Magic3D::ScriptColor>::ScriptEnum Magic3D::ScriptColor::enums[] = {{NULL, NULL}};
Magic3D::ScriptClass<Magic3D::ScriptColor>::ScriptFunction Magic3D::ScriptColor::functions[] = {
    ScriptClassFunction(ScriptColor, setR, "void setR(float r)", "Set the R element of the color."),
    ScriptClassFunction(ScriptColor, getR, "float getR()", "Get the R element of the color."),

    ScriptClassFunction(ScriptColor, setG, "void setG(float g)", "Set the G element of the color."),
    ScriptClassFunction(ScriptColor, getG, "float getG()", "Get the G element of the color."),

    ScriptClassFunction(ScriptColor, setB, "void setB(float b)", "Set the B element of the color."),
    ScriptClassFunction(ScriptColor, getB, "float getB()", "Get the B element of the color."),

    ScriptClassFunction(ScriptColor, setA, "void setA(float a)", "Set the A element of the color."),
    ScriptClassFunction(ScriptColor, getA, "float getA()", "Get the A element of the color."),

    ScriptClassFunction(ScriptColor, setValue, "void setValue(float r, float g, float b, float a)", "Set all elements of the color."),
    {NULL, NULL, NULL, NULL}
};

Magic3D::ScriptColor::ScriptColor(lua_State *lua) : ScriptBasic(className)
{
    setValue(lua);
}

Magic3D::ScriptColor::ScriptColor(ColorRGBA color) : ScriptBasic(className)
{
    this->color = color;
}

Magic3D::ScriptColor::~ScriptColor()
{

}

int Magic3D::ScriptColor::setR(lua_State *lua)
{
    color.r = luaL_checknumber(lua, 1);
    return 0;
}

int Magic3D::ScriptColor::getR(lua_State *lua)
{
    lua_pushnumber(lua, color.r);
    return 1;
}

int Magic3D::ScriptColor::setG(lua_State *lua)
{
    color.g = luaL_checknumber(lua, 1);
    return 0;
}

int Magic3D::ScriptColor::getG(lua_State *lua)
{
    lua_pushnumber(lua, color.g);
    return 1;
}

int Magic3D::ScriptColor::setB(lua_State *lua)
{
    color.b  = luaL_checknumber(lua, 1);
    return 0;
}

int Magic3D::ScriptColor::getB(lua_State *lua)
{
    lua_pushnumber(lua, color.b);
    return 1;
}

int Magic3D::ScriptColor::setA(lua_State *lua)
{
    color.a = luaL_checknumber(lua, 1);
    return 0;
}

int Magic3D::ScriptColor::getA(lua_State *lua)
{
    lua_pushnumber(lua, color.a);
    return 1;
}

int Magic3D::ScriptColor::setValue(lua_State *lua)
{
    color.r = luaL_checknumber(lua, 1);
    color.g = luaL_checknumber(lua, 2);
    color.b = luaL_checknumber(lua, 3);
    color.a = luaL_checknumber(lua, 3);
    return 0;
}

Magic3D::ColorRGBA& Magic3D::ScriptColor::getValue()
{
    return color;
}
