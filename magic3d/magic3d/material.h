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

#ifndef MAGIC3D_MATERIAL_H
#define MAGIC3D_MATERIAL_H

#include <magic3d/xml.h>
#include <magic3d/resource/resources_shader.h>

namespace Magic3D
{
#define M3D_MATERIAL_XML                "Material"
#define M3D_MATERIAL_XML_TEXTURE        "Texture"
#define M3D_MATERIAL_XML_VARIABLES      "Variables"
#define M3D_MATERIAL_XML_COLOR_AMBIENT  "color_ambient"
#define M3D_MATERIAL_XML_COLOR_DIFFUSE  "color_diffuse"
#define M3D_MATERIAL_XML_COLOR_EMISSION "color_emission"
#define M3D_MATERIAL_XML_COLOR_SPECULAR "color_specular"
#define M3D_MATERIAL_XML_SHININESS      "shininess"
#define M3D_MATERIAL_XML_ALPHATEST      "alpha_test"
#define M3D_MATERIAL_XML_SHADER         "shader"
#define M3D_MATERIAL_XML_BLEND          "blend"
#define M3D_MATERIAL_XML_TEX_NAME       "name"
#define M3D_MATERIAL_XML_TEX_FILE       "file"
#define M3D_MATERIAL_XML_TEX_MIPMAP     "mipmap"
#define M3D_MATERIAL_XML_TEX_CLAMP      "clamp"
#define M3D_MATERIAL_XML_TEX_TYPE       "type"

class Texture;
class Shader;

enum BLEND
{
    eBLEND_ALPHA,
    eBLEND_ADD,
    eBLEND_MULTIPLY,
    eBLEND_DIVIDE,
    eBLEND_INVERT,
    eBLEND_COLOR,
    eBLEND_COUNT
};

class MaterialVar : public Xml
{
protected:
    ShaderVar* variable;
public:
    MaterialVar(ShaderVar* variable);
    virtual ~MaterialVar();

    const std::string& getName();
    SHADER_VAR getType();
    int getLocation();
    int getSize();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class MaterialVar_Boolean : public MaterialVar
{
private:
    Vector4Bool value;
public:
    MaterialVar_Boolean(ShaderVar* variable);
    virtual ~MaterialVar_Boolean();

    void setValue(int index, bool value);
    bool getValue(int index);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class MaterialVar_Integer : public MaterialVar
{
private:
    Vector4Int value;
public:
    MaterialVar_Integer(ShaderVar* variable);
    virtual ~MaterialVar_Integer();

    void setValue(int index, int value);
    int getValue(int index);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class MaterialVar_Float : public MaterialVar
{
private:
    Vector4 value;
public:
    MaterialVar_Float(ShaderVar* variable);
    virtual ~MaterialVar_Float();

    void setValue(int index, float value);
    float getValue(int index);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class MaterialVar_Matrix : public MaterialVar
{
private:
    Vector4 value2;
    Matrix3 value3;
    Matrix4 value;
public:
    MaterialVar_Matrix(ShaderVar* variable);
    virtual ~MaterialVar_Matrix();

    const Vector4& getValue2();
    const Matrix3& getValue3();
    const Matrix4& getValue4();
    void setValue(int col, int row, float value);
    float getValue(int col, int row);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class MaterialVar_Texture : public MaterialVar
{
private:
    Texture* value;
public:
    MaterialVar_Texture(ShaderVar* variable);
    virtual ~MaterialVar_Texture();

    void setValue(Texture* value);
    Texture* getValue();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class Material : public Xml
{
private:
    std::string name;
    ColorRGBA ambient;
    ColorRGBA diffuse;
    ColorRGBA emission;
    ColorRGBA specular;

    float shininess;
    float alphaTest;

    BLEND blend;

    Texture* texture[M3D_MULTI_TEXTURE_COUNT];
    Shader* shader;

    std::vector<MaterialVar*> variables;

    void setShader(Shader* shader, XMLElement* root);

protected:
    Material(const Material& material, std::string name);

public:
    Material(std::string name);
    virtual ~Material();
    virtual Material* spawn() const;

    const std::string& getName();
    std::string getMaterialFile();

    Texture* getTexture(int index);
    void setTexture(int index, Texture* texture);

    void setBlend(BLEND blend);
    BLEND getBlend();

    void setColorAmbient(ColorRGBA color);
    const ColorRGBA& getColorAmbient();

    void setColorDiffuse(ColorRGBA color);
    const ColorRGBA& getColorDiffuse();

    void setColorEmission(ColorRGBA color);
    const ColorRGBA& getColorEmission();

    void setColorSpecular(ColorRGBA color);
    const ColorRGBA& getColorSpecular();

    void setShininess(float factor);
    float getShininess();

    void setAlphaTest(float factor);
    float getAlphaTest();

    void setShader(Shader* shader);
    Shader* getShader();

    void clearVars();
    MaterialVar* getVar(std::string name);
    MaterialVar* getVar(int index);
    int getVarCount();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);

    virtual bool save();
    virtual bool load();
};

}

#endif // MATERIAL_H
