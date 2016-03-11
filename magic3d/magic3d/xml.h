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

#ifndef MAGIC3D_XML_H
#define MAGIC3D_XML_H

#include <magic3d/type.h>
#include <magic3d/math.h>

namespace Magic3D
{

#define M3D_XML_NULL "null"

#define M3D_VECTOR_XML_X "x"
#define M3D_VECTOR_XML_Y "y"
#define M3D_VECTOR_XML_Z "z"
#define M3D_VECTOR_XML_W "w"

#define M3D_COLOR_XML_R "r"
#define M3D_COLOR_XML_G "g"
#define M3D_COLOR_XML_B "b"
#define M3D_COLOR_XML_A "a"

using namespace tinyxml2;
class Xml
{
private:
    std::vector<Xml*> references;

public:
    Xml();
    virtual ~Xml();

    virtual XMLElement* save(XMLElement* root) = 0;
    virtual XMLElement* load(XMLElement* root) = 0;

    static XMLElement* saveString(XMLElement* root, const char* name, const std::string& value);
    static std::string loadString(XMLElement* root, const char* name);

    static XMLElement* saveFloat(XMLElement* root, const char* name, float value);
    static float loadFloat(XMLElement* root, const char* name);

    static XMLElement* saveInt(XMLElement* root, const char* name, int value);
    static int loadInt(XMLElement* root, const char* name);

    static XMLElement* saveBool(XMLElement* root, const char* name, bool value);
    static bool loadBool(XMLElement* root, const char* name);

    static XMLElement* saveVector3(XMLElement* root, const char* name, const Vector3& vector);
    static Vector3 loadVector3(XMLElement* root, const char* name);

    static XMLElement* saveVector4(XMLElement* root, const char* name, const Vector4& vector);
    static Vector4 loadVector4(XMLElement* root, const char* name);

    static XMLElement* saveVector4Bool(XMLElement* root, const char* name, const Vector4Bool& vector);
    static Vector4Bool loadVector4Bool(XMLElement* root, const char* name);

    static XMLElement* saveVector4Int(XMLElement* root, const char* name, const Vector4Int& vector);
    static Vector4Int loadVector4Int(XMLElement* root, const char* name);

    static XMLElement* saveColorRGBA(XMLElement* root, const char* name, const ColorRGBA& color);
    static ColorRGBA loadColorRGBA(XMLElement* root, const char* name);

    static Vector3 parseVector3(const char* vector);
    static Vector4 parseVector4(const char* vector);
    static Matrix4 parseMatrix4(const char* matrix);

    void addReference(Xml* reference);
    virtual void removeReference(Xml* reference);
    bool hasReference(Xml* reference);
    void clearReferences();
};

}

#endif // MAGIC3D_XML_H
