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

#include <magic3d/xml.h>
#include <magic3d/renderer/renderer.h>

Magic3D::Xml::Xml()
{

}

Magic3D::Xml::~Xml()
{
    clearReferences();
}

tinyxml2::XMLElement* Magic3D::Xml::saveString(XMLElement* root, const char* name, const std::string& value)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );

    if (value.empty())
    {
        attribute->LinkEndChild( root->GetDocument()->NewText( M3D_XML_NULL ));
    }
    else
    {
        attribute->LinkEndChild( root->GetDocument()->NewText( value.c_str() ));
    }
    if (root)
    {
        root->LinkEndChild(attribute);
    }
    return attribute;
}

std::string Magic3D::Xml::loadString(XMLElement* root, const char* name)
{
    std::string result = std::string(M3D_XML_NULL);
    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            result = std::string(xml->GetText());
        }
    }
    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveFloat(XMLElement* root, const char* name, float value)
{
    std::stringstream st;
    st << value;
    XMLElement* attribute = root->GetDocument()->NewElement(name);
    attribute->LinkEndChild( root->GetDocument()->NewText(st.str().c_str()));
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

float Magic3D::Xml::loadFloat(XMLElement* root, const char* name)
{
    float result = 0.0f;

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            result = atof(xml->GetText());
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveInt(XMLElement* root, const char* name, int value)
{
    std::stringstream st;
    st << value;
    XMLElement* attribute = root->GetDocument()->NewElement(name);
    attribute->LinkEndChild( root->GetDocument()->NewText(st.str().c_str()));
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

int Magic3D::Xml::loadInt(XMLElement* root, const char* name)
{
    int result = 0;
    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            result = atoi(xml->GetText());
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveBool(XMLElement* root, const char* name, bool value)
{
    return saveInt(root, name, value ? 1 : 0);
}

bool Magic3D::Xml::loadBool(XMLElement* root, const char* name)
{
    return loadInt(root, name) == 0 ? false : true;
}

tinyxml2::XMLElement* Magic3D::Xml::saveVector3(XMLElement* root, const char* name, const Vector3& vector)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );
    attribute->SetAttribute(M3D_VECTOR_XML_X, vector.getX());
    attribute->SetAttribute(M3D_VECTOR_XML_Y, vector.getY());
    attribute->SetAttribute(M3D_VECTOR_XML_Z, vector.getZ());
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

Magic3D::Vector3 Magic3D::Xml::loadVector3(XMLElement* root, const char* name)
{
    Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            float x = result.getX();
            float y = result.getY();
            float z = result.getZ();

            xml->QueryFloatAttribute(M3D_VECTOR_XML_X, &x);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Y, &y);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Z, &z);

            result.setX(x);
            result.setY(y);
            result.setZ(z);
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveVector4(XMLElement* root, const char* name, const Vector4& vector)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );
    attribute->SetAttribute(M3D_VECTOR_XML_X, vector.getX());
    attribute->SetAttribute(M3D_VECTOR_XML_Y, vector.getY());
    attribute->SetAttribute(M3D_VECTOR_XML_Z, vector.getZ());
    attribute->SetAttribute(M3D_VECTOR_XML_W, vector.getW());
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

Magic3D::Vector4 Magic3D::Xml::loadVector4(XMLElement* root, const char* name)
{
    Vector4 result = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            float x = result.getX();
            float y = result.getY();
            float z = result.getZ();
            float w = result.getW();

            xml->QueryFloatAttribute(M3D_VECTOR_XML_X, &x);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Y, &y);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Z, &z);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_W, &w);

            result.setX(x);
            result.setY(y);
            result.setZ(z);
            result.setW(w);
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveVector4Bool(XMLElement* root, const char* name, const Vector4Bool& vector)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );
    attribute->SetAttribute(M3D_VECTOR_XML_X, vector.x);
    attribute->SetAttribute(M3D_VECTOR_XML_Y, vector.y);
    attribute->SetAttribute(M3D_VECTOR_XML_Z, vector.z);
    attribute->SetAttribute(M3D_VECTOR_XML_W, vector.w);
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

Magic3D::Vector4Bool Magic3D::Xml::loadVector4Bool(XMLElement* root, const char* name)
{
    Vector4Bool result = Vector4Bool(false, false, false, false);

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            bool x = result.x;
            bool y = result.y;
            bool z = result.z;
            bool w = result.w;

            xml->QueryBoolAttribute(M3D_VECTOR_XML_X, &x);
            xml->QueryBoolAttribute(M3D_VECTOR_XML_Y, &y);
            xml->QueryBoolAttribute(M3D_VECTOR_XML_Z, &z);
            xml->QueryBoolAttribute(M3D_VECTOR_XML_W, &w);

            result.x = x;
            result.y = y;
            result.z = z;
            result.w = w;
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveVector4Int(XMLElement* root, const char* name, const Vector4Int& vector)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );
    attribute->SetAttribute(M3D_VECTOR_XML_X, vector.x);
    attribute->SetAttribute(M3D_VECTOR_XML_Y, vector.y);
    attribute->SetAttribute(M3D_VECTOR_XML_Z, vector.z);
    attribute->SetAttribute(M3D_VECTOR_XML_W, vector.w);
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

Magic3D::Vector4Int Magic3D::Xml::loadVector4Int(XMLElement* root, const char* name)
{
    Vector4Int result = Vector4Int(0, 0, 0, 0);

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            int x = result.x;
            int y = result.y;
            int z = result.z;
            int w = result.w;

            xml->QueryIntAttribute(M3D_VECTOR_XML_X, &x);
            xml->QueryIntAttribute(M3D_VECTOR_XML_Y, &y);
            xml->QueryIntAttribute(M3D_VECTOR_XML_Z, &z);
            xml->QueryIntAttribute(M3D_VECTOR_XML_W, &w);

            result.x = x;
            result.y = y;
            result.z = z;
            result.w = w;
        }
    }

    return result;
}

tinyxml2::XMLElement* Magic3D::Xml::saveColorRGBA(XMLElement* root, const char* name, const ColorRGBA& color)
{
    XMLElement* attribute = root->GetDocument()->NewElement( name );
    attribute->SetAttribute(M3D_COLOR_XML_R, color.r);
    attribute->SetAttribute(M3D_COLOR_XML_G, color.g);
    attribute->SetAttribute(M3D_COLOR_XML_B, color.b);
    attribute->SetAttribute(M3D_COLOR_XML_A, color.a);
    if (root)
    {
        root->LinkEndChild( attribute );
    }
    return attribute;
}

Magic3D::ColorRGBA Magic3D::Xml::loadColorRGBA(XMLElement* root, const char* name)
{
    ColorRGBA result = ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);

    if (root)
    {
        XMLElement* xml = root->FirstChildElement(name);
        if (xml)
        {
            float r = result.r;
            float g = result.g;
            float b = result.b;
            float a = result.a;

            xml->QueryFloatAttribute(M3D_COLOR_XML_R, &r);
            xml->QueryFloatAttribute(M3D_COLOR_XML_G, &g);
            xml->QueryFloatAttribute(M3D_COLOR_XML_B, &b);
            xml->QueryFloatAttribute(M3D_COLOR_XML_A, &a);

            result.r = r;
            result.g = g;
            result.b = b;
            result.a = a;
        }
    }

    return result;
}

Magic3D::Vector3 Magic3D::Xml::parseVector3(const char* vector)
{
    std::istringstream tokken (vector);
    std::string value;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (tokken.good())
    {
        tokken >> value;
        x = atof(value.c_str());
    }

    if (tokken.good())
    {
        tokken >> value;
        y = atof(value.c_str());
    }

    if (tokken.good())
    {
        tokken >> value;
        z = atof(value.c_str());
    }

    return Vector3(x, y, z);
}

Magic3D::Vector4 Magic3D::Xml::parseVector4(const char* vector)
{
    std::istringstream tokken (vector);
    std::string value;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    if (tokken.good())
    {
        tokken >> value;
        x = atof(value.c_str());
    }

    if (tokken.good())
    {
        tokken >> value;
        y = atof(value.c_str());
    }

    if (tokken.good())
    {
        tokken >> value;
        z = atof(value.c_str());
    }

    if (tokken.good())
    {
        tokken >> value;
        w = atof(value.c_str());
    }

    return Vector4(x, y, z, w);
}

Magic3D::Matrix4 Magic3D::Xml::parseMatrix4(const char* matrix)
{
    std::istringstream tokken (matrix);
    std::string value;

    int size = 16;
    float m[size];
    int index = 0;

    for (int i = 0; i < size; i++)
    {
        if (tokken.good())
        {
            tokken >> value;
            m[index++] = atof(value.c_str());
        }
    }

    Matrix4 m4;
    Renderer::getInstance()->arrayToMatrix(m, m4);

    return m4;
}

void Magic3D::Xml::addReference(Xml* reference)
{
    references.push_back(reference);
}

void Magic3D::Xml::removeReference(Xml* reference)
{
    std::vector<Xml*>::iterator it_x = references.begin();

    while (it_x != references.end())
    {
        Xml* xml = *it_x;

        if (xml == reference)
        {
            references.erase(it_x);
            xml->removeReference(this);
            break;
        }

        it_x++;
    }
}

void Magic3D::Xml::clearReferences()
{
    while (!references.empty())
    {
        Xml* xml = *references.begin();
        references.erase(references.begin());
        if (xml)
        {
            xml->removeReference(this);
        }
    }
}
