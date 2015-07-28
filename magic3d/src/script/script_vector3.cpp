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

#include <magic3d/script/script_vector3.h>
#include <magic3d/magic3d.h>
#include <magic3d/math.h>

const char Magic3D::ScriptVector3::className[] = "Vector3";

Magic3D::ScriptClass<Magic3D::ScriptVector3>::ScriptEnum Magic3D::ScriptVector3::enums[] = {{NULL, NULL}};
Magic3D::ScriptClass<Magic3D::ScriptVector3>::ScriptFunction Magic3D::ScriptVector3::functions[] = {
    ScriptClassFunction(ScriptVector3, setX, "void setX(float x)", "Set the X element of the vector."),
    ScriptClassFunction(ScriptVector3, getX, "float getX()", "Get the X element of the vector."),

    ScriptClassFunction(ScriptVector3, setY, "void setY(float y)", "Set the Y element of the vector."),
    ScriptClassFunction(ScriptVector3, getY, "float getY()", "Get the Y element of the vector."),

    ScriptClassFunction(ScriptVector3, setZ, "void setZ(float z)", "Set the Z element of the vector."),
    ScriptClassFunction(ScriptVector3, getZ, "float getZ()", "Get the Z element of the vector."),

    ScriptClassFunction(ScriptVector3, setValue, "void setValue(float x, float y, float z)", "Set all elements of the vector."),

    ScriptClassFunction(ScriptVector3, sum,      "Vector3 sum(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, subtract, "Vector3 subtract(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, multiply, "Vector3 multiply(float scalar)", ""),
    ScriptClassFunction(ScriptVector3, scale,    "Vector3 scale(Vector3 scale)", ""),

    ScriptClassFunction(ScriptVector3, dot,        "float dot(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, cross,      "Vector3 cross(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, distance,   "float distance(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, normalized, "Vector3 normalized()", ""),

    ScriptClassFunction(ScriptVector3, angle,      "float angle(Vector3 vector)", ""),
    ScriptClassFunction(ScriptVector3, slerp,      "float slerp(Vector3 vector, float factor)", ""),

    ScriptClassFunction(ScriptVector3, getEulerRight, "", ""),
    ScriptClassFunction(ScriptVector3, getEulerUp,    "", ""),
    ScriptClassFunction(ScriptVector3, getEulerFront, "", ""),
    ScriptClassFunction(ScriptVector3, getEulerLeft,  "", ""),
    ScriptClassFunction(ScriptVector3, getEulerDown,  "", ""),
    ScriptClassFunction(ScriptVector3, getEulerBack,  "", ""),
    {NULL, NULL, NULL, NULL}
};

Magic3D::ScriptVector3::ScriptVector3(lua_State *lua) : ScriptBasic(className)
{
    setValue(lua);
}

Magic3D::ScriptVector3::ScriptVector3(Vector3 vector) : ScriptBasic(className)
{
    this->vector = vector;
}

Magic3D::ScriptVector3::~ScriptVector3()
{

}

int Magic3D::ScriptVector3::setX(lua_State *lua)
{
    vector.setX(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptVector3::getX(lua_State *lua)
{
    lua_pushnumber(lua, vector.getX());
    return 1;
}

int Magic3D::ScriptVector3::setY(lua_State *lua)
{
    vector.setY(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptVector3::getY(lua_State *lua)
{
    lua_pushnumber(lua, vector.getY());
    return 1;
}

int Magic3D::ScriptVector3::setZ(lua_State *lua)
{
    vector.setZ(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptVector3::getZ(lua_State *lua)
{
    lua_pushnumber(lua, vector.getZ());
    return 1;
}

int Magic3D::ScriptVector3::setValue(lua_State *lua)
{
    vector.setX(luaL_checknumber(lua, 1));
    vector.setY(luaL_checknumber(lua, 2));
    vector.setZ(luaL_checknumber(lua, 3));
    return 0;
}

int Magic3D::ScriptVector3::sum(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* vec = new ScriptVector3(getValue() + vector->getValue());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::subtract(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* vec = new ScriptVector3(getValue() - vector->getValue());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::multiply(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(getValue() * luaL_checknumber(lua, 1));

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::scale(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* vec = new ScriptVector3(mulPerElem(getValue(), vector->getValue()));

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::dot(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    float dp = Vectormath::Aos::dot(getValue(), vector->getValue());

    lua_pushnumber(lua, dp);
    return 1;
}

int Magic3D::ScriptVector3::cross(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* vec = new ScriptVector3(Vectormath::Aos::cross(getValue(), vector->getValue()));

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::distance(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    float d = dist(Point3(getValue()), Point3(vector->getValue()));

    lua_pushnumber(lua, d);
    return 1;
}

int Magic3D::ScriptVector3::normalized(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(normalize(getValue()));

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::angle(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* normal = ScriptClass<ScriptVector3>::check(lua, 2);
    float ang = Math::angle(getValue(), vector->getValue(), normal->getValue());

    lua_pushnumber(lua, ang);
    return 1;
}

int Magic3D::ScriptVector3::slerp(lua_State *lua)
{
    ScriptVector3* rot = ScriptClass<ScriptVector3>::check(lua, 1);
    Quaternion q = Math::quaternion(rot->getValue());
    Quaternion r = Math::quaternion(getValue());
    ScriptVector3* vec = new ScriptVector3(Math::euler(Vectormath::Aos::slerp(luaL_checknumber(lua, 2), r, q)));

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerRight(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(matrix.getCol0());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerUp(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(matrix.getCol1());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerFront(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(matrix.getCol2());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerLeft(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(-matrix.getCol0());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerDown(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(-matrix.getCol1());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptVector3::getEulerBack(lua_State *lua)
{
    Matrix3 matrix = Matrix3(Math::quaternion(getValue()));

    ScriptVector3* vec = new ScriptVector3(-matrix.getCol2());
    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

Magic3D::Vector3& Magic3D::ScriptVector3::getValue()
{
    return vector;
}

std::string Magic3D::ScriptVector3::toString()
{
    std::stringstream str;
    str << "x: ";
    str << vector.getX();
    str << ", y: ";
    str << vector.getY();
    str << ", z: ";
    str << vector.getZ();

    return str.str();
}
