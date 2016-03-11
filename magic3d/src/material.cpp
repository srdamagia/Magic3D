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

#include <magic3d/magic3d.h>

bool SortVariableName(Magic3D::MaterialVar* lo, Magic3D::MaterialVar* ro)
{
    return strcasecmp(lo->getName().c_str(), ro->getName().c_str()) < 0;
}

Magic3D::MaterialVar::MaterialVar(ShaderVar* variable)
{
    this->variable = variable;

    if (!variable)
    {
        this->variable = new ShaderVar();
        this->variable->name = "<none>";
        this->variable->type = eSHADER_VAR_NONE;
        this->variable->size = 0;
        this->variable->location = -1;
    }
}

Magic3D::MaterialVar::~MaterialVar()
{
    if (variable->name.compare("<none>") == 0 && variable->location == -1)
    {
        delete variable;
    }
    variable = NULL;
}

const std::string& Magic3D::MaterialVar::getName()
{
    return variable->name;
}

Magic3D::SHADER_VAR Magic3D::MaterialVar::getType()
{
    return variable->type;
}

int Magic3D::MaterialVar::getLocation()
{
    return variable->location;
}

int Magic3D::MaterialVar::getSize()
{
    return variable->size;
}

Magic3D::XMLElement* Magic3D::MaterialVar::save(XMLElement* root)
{
    if (root)
    {

    }
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar::load(XMLElement* root)
{
    if (root)
    {

    }
    return root;
}

//******************************************************************************
Magic3D::MaterialVar_Boolean::MaterialVar_Boolean(ShaderVar* variable) : MaterialVar(variable)
{
    value = Vector4Bool(false, false, false, false);
}

Magic3D::MaterialVar_Boolean::~MaterialVar_Boolean()
{

}

void Magic3D::MaterialVar_Boolean::setValue(int index, bool value)
{
    if (index >= 0 && index < 4)
    {
        *(&this->value.x + index) = value;
    }
}

bool Magic3D::MaterialVar_Boolean::getValue(int index)
{
    bool result = false;
    if (index >= 0 && index < 4)
    {
        result = *(&this->value.x + index);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Boolean::save(XMLElement* root)
{
    saveVector4Bool(root, variable->name.c_str(), value);
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Boolean::load(XMLElement* root)
{
    value = loadVector4Bool(root, variable->name.c_str());
    return root;
}

//******************************************************************************
Magic3D::MaterialVar_Integer::MaterialVar_Integer(ShaderVar* variable) : MaterialVar(variable)
{
    value = Vector4Int(0, 0, 0, 0);
}

Magic3D::MaterialVar_Integer::~MaterialVar_Integer()
{

}

void Magic3D::MaterialVar_Integer::setValue(int index, int value)
{
    if (index >= 0 && index < 4)
    {
        *(&this->value.x + index) = value;
    }
}

int Magic3D::MaterialVar_Integer::getValue(int index)
{
    int result = 0;
    if (index >=0 && index < 4)
    {
        result = *(&this->value.x + index);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Integer::save(XMLElement* root)
{
    saveVector4Int(root, variable->name.c_str(), value);
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Integer::load(XMLElement* root)
{
    value = loadVector4Int(root, variable->name.c_str());
    return root;
}

//******************************************************************************
Magic3D::MaterialVar_Float::MaterialVar_Float(ShaderVar* variable) : MaterialVar(variable)
{
    value = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

Magic3D::MaterialVar_Float::~MaterialVar_Float()
{
    value = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

void Magic3D::MaterialVar_Float::setValue(int index, float value)
{
    if (index >= 0 && index < 4)
    {
        this->value.setElem(index, value);
    }
}

float Magic3D::MaterialVar_Float::getValue(int index)
{
    float result = 0.0f;
    if (index >= 0 && index < 4)
    {
        result = value.getElem(index);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Float::save(XMLElement* root)
{
    saveVector4(root, variable->name.c_str(), value);
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Float::load(XMLElement* root)
{
    value = loadVector4(root, variable->name.c_str());
    return root;
}

//******************************************************************************
Magic3D::MaterialVar_Matrix::MaterialVar_Matrix(ShaderVar* variable) : MaterialVar(variable)
{
    value = Matrix4::identity();
}

Magic3D::MaterialVar_Matrix::~MaterialVar_Matrix()
{
    value = Matrix4::identity();
}

void Magic3D::MaterialVar_Matrix::setValue(int col, int row, float value)
{
    if (col >= 0 && col < 4 && row >= 0 && row < 4)
    {
        this->value.setElem(col, row, value);
    }

    value2 = Vector4(this->value.getElem(0, 0), this->value.getElem(0, 1), this->value.getElem(1, 0), this->value.getElem(1, 1));
    value3 = this->value.getUpper3x3();
}

float Magic3D::MaterialVar_Matrix::getValue(int col, int row)
{
    float result = 0.0f;
    if (col >= 0 && col < 4 && row >= 0 && row < 4)
    {
        result = value.getElem(col, row);
    }
    return result;
}

const Magic3D::Vector4& Magic3D::MaterialVar_Matrix::getValue2()
{
    return value2;
}

const Magic3D::Matrix3& Magic3D::MaterialVar_Matrix::getValue3()
{
    return value3;
}

const Magic3D::Matrix4& Magic3D::MaterialVar_Matrix::getValue4()
{
    return value;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Matrix::save(XMLElement* root)
{
    if (root)
    {
        XMLElement* attribute = root->GetDocument()->NewElement( variable->name.c_str() );
        saveVector4(attribute, "row_0", value.getCol0());
        saveVector4(attribute, "row_1", value.getCol1());
        saveVector4(attribute, "row_2", value.getCol2());
        saveVector4(attribute, "row_3", value.getCol3());
        root->LinkEndChild(attribute);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Matrix::load(XMLElement* root)
{
    if (root)
    {
        XMLElement* xml = root->FirstChildElement( variable->name.c_str() );
        if (xml)
        {
            value.setCol0(loadVector4(xml, "row_0"));
            value.setCol1(loadVector4(xml, "row_1"));
            value.setCol2(loadVector4(xml, "row_2"));
            value.setCol3(loadVector4(xml, "row_3"));
        }
    }
    return root;
}

//******************************************************************************
Magic3D::MaterialVar_Texture::MaterialVar_Texture(ShaderVar* variable) : MaterialVar(variable)
{
    value = NULL;
}

Magic3D::MaterialVar_Texture::~MaterialVar_Texture()
{
    value = NULL;
}

void Magic3D::MaterialVar_Texture::setValue(Texture* value)
{
    this->value = value;
}

Magic3D::Texture* Magic3D::MaterialVar_Texture::getValue()
{
    return value;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Texture::save(XMLElement* root)
{
    ResourceManager::getTextures()->saveTexture(root, variable->name.c_str(), value);
    return root;
}

Magic3D::XMLElement* Magic3D::MaterialVar_Texture::load(XMLElement* root)
{
    XMLElement* tex = root->FirstChildElement(variable->name.c_str());
    if (tex)
    {
        value = ResourceManager::getTextures()->loadTexture(tex);
    }
    return root;
}

//******************************************************************************
Magic3D::Material::Material(const Material& material, std::string name)
{
    this->name = name;

    for (int i = 0; i < M3D_MULTI_TEXTURE_COUNT; i++)
    {
        this->texture[i] = material.texture[i];
    }

    this->ambient = material.ambient;
    this->diffuse = material.diffuse;
    this->emission = material.emission;
    this->specular = material.specular;

    this->shininess = material.shininess;
    this->alphaTest = material.alphaTest;

    this->shader = material.shader;

    this->blend = material.blend;
}

Magic3D::Material::Material(std::string name)
{
    this->name = name;

    for (int i = 0; i < M3D_MULTI_TEXTURE_COUNT; i++)
    {
        texture[i] = NULL;
    }

    blend = eBLEND_ALPHA;

    ambient   = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    diffuse   = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    emission  = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    specular  = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    shininess = 64.0f;
    alphaTest = 0.0f;
    shader    = NULL;
}

Magic3D::Material::~Material()
{
    for (int i = 0; i < M3D_MULTI_TEXTURE_COUNT; i++)
    {
        texture[i] = NULL;
    }

    clearVars();

    shader = NULL;
}

Magic3D::Material* Magic3D::Material::spawn() const
{
    return (new Material(*this));
}

const std::string& Magic3D::Material::getName()
{
    return name;
}

std::string Magic3D::Material::getMaterialFile()
{
    std::string fileName = ResourceManager::getMaterials()->getPath(name + M3D_MATERIAL_FILE);
    return fileName;
}

Magic3D::Texture* Magic3D::Material::getTexture(int index)
{
    Texture* result = NULL;
    if (index >= 0 && index < M3D_MULTI_TEXTURE_COUNT)
    {
        result = texture[index];
    }
    return result;
}

void Magic3D::Material::setTexture(int index, Texture* texture)
{
    if (index >= 0 && index < M3D_MULTI_TEXTURE_COUNT)
    {
        this->texture[index] = texture;
    }
}

void Magic3D::Material::setBlend(BLEND blend)
{
    this->blend = blend;
}

Magic3D::BLEND Magic3D::Material::getBlend()
{
    return blend;
}

void Magic3D::Material::setColorAmbient(ColorRGBA color)
{
    ambient = color;
}

const Magic3D::ColorRGBA& Magic3D::Material::getColorAmbient()
{
    return ambient;
}

void Magic3D::Material::setColorDiffuse(ColorRGBA color)
{
    diffuse = color;
}

const Magic3D::ColorRGBA& Magic3D::Material::getColorDiffuse()
{
    return diffuse;
}

void Magic3D::Material::setColorEmission(ColorRGBA color)
{
    emission = color;
}

const Magic3D::ColorRGBA& Magic3D::Material::getColorEmission()
{
    return emission;
}

void Magic3D::Material::setColorSpecular(ColorRGBA color)
{
    specular = color;
}

const Magic3D::ColorRGBA& Magic3D::Material::getColorSpecular()
{
    return specular;
}

void Magic3D::Material::setShininess(float factor)
{
    shininess = factor;
}

float Magic3D::Material::getShininess()
{
    return shininess;
}

void Magic3D::Material::setAlphaTest(float factor)
{
    alphaTest = factor;
}

float Magic3D::Material::getAlphaTest()
{
    return alphaTest;
}


void Magic3D::Material::setShader(Shader* shader)
{
    setShader(shader, NULL);
}

void Magic3D::Material::setShader(Shader* shader, XMLElement* root)
{
    clearVars();
    this->shader = shader;
    if (shader)
    {
        XMLElement* first = root;
        XMLDocument* doc = NULL;

        if (!first)
        {
            doc = new XMLDocument();

            bool result = false;
            if (ResourceManager::getInstance()->getPackage())
            {
                Memory mem;
                result = ResourceManager::getInstance()->unpack(getMaterialFile(), &mem);
                std::string str = mem.getBuffer()->str();
                result = doc->Parse(str.c_str(), str.size()) == XML_SUCCESS && result;
            }
            else
            {
                result = doc->LoadFile(getMaterialFile().c_str()) == XML_SUCCESS;
            }

            if (result)
            {
                XMLElement* pElem = doc->FirstChildElement();

                // should always have a valid root but handle gracefully if it does
                if (pElem)
                {
                    first = pElem->FirstChildElement(M3D_MATERIAL_XML_VARIABLES);
                }
            }
        }

        for (int i = 0; i < shader->getVarCount(); i++)
        {
            MaterialVar* variable = NULL;
            switch (shader->getVar(i)->type)
            {
                case eSHADER_VAR_FLOAT:
                case eSHADER_VAR_FLOAT_VEC2:
                case eSHADER_VAR_FLOAT_VEC3:
                case eSHADER_VAR_FLOAT_VEC4:
                {
                    variable = new MaterialVar_Float(shader->getVar(i));
                    variable->load(first);
                    break;
                }
                case eSHADER_VAR_INT:
                case eSHADER_VAR_INT_VEC2:
                case eSHADER_VAR_INT_VEC3:
                case eSHADER_VAR_INT_VEC4:
                {
                    variable = new MaterialVar_Integer(shader->getVar(i));
                    variable->load(first);
                    break;
                }
                case eSHADER_VAR_BOOL:
                case eSHADER_VAR_BOOL_VEC2:
                case eSHADER_VAR_BOOL_VEC3:
                case eSHADER_VAR_BOOL_VEC4:
                {
                    variable = new MaterialVar_Boolean(shader->getVar(i));
                    variable->load(first);
                    break;
                }
                case eSHADER_VAR_FLOAT_MAT2:
                case eSHADER_VAR_FLOAT_MAT3:
                case eSHADER_VAR_FLOAT_MAT4:
                {
                    variable = new MaterialVar_Matrix(shader->getVar(i));
                    variable->load(first);
                    break;
                }
                case eSHADER_VAR_SAMPLER_2D:
                case eSHADER_VAR_SAMPLER_CUBE:
                {
                    variable = new MaterialVar_Texture(shader->getVar(i));
                    variable->load(first);
                    break;
                }
                default: break;
            }

            if (variable != NULL)
            {
                variables.push_back(variable);
            }
        }

        std::sort(variables.begin(), variables.end(), SortVariableName);

        if (!root && doc)
        {
            delete doc;
            doc = NULL;
        }
    }
}

Magic3D::Shader* Magic3D::Material::getShader()
{
    return shader;
}

void Magic3D::Material::clearVars()
{
    while (!variables.empty())
    {
        MaterialVar* variable = *variables.begin();
        variables.erase(variables.begin());
        if (variable)
        {
            delete variable;
        }
    }
}

Magic3D::MaterialVar* Magic3D::Material::getVar(std::string name)
{
    MaterialVar* result = NULL;
    int size = variables.size();
    for (int i = 0; i < size; i++)
    {
        if (variables[i]->getName().compare(name) == 0)
        {
            result = variables[i];
        }
    }

    return result;
}

Magic3D::MaterialVar* Magic3D::Material::getVar(int index)
{
    MaterialVar* result = NULL;
    if (index >= 0 && index < (int)variables.size())
    {
        result = variables[index];
    }
    return result;
}

int Magic3D::Material::getVarCount()
{
    return variables.size();
}

Magic3D::XMLElement* Magic3D::Material::save(XMLElement* root)
{
    if (root)
    {
        saveColorRGBA(root, M3D_MATERIAL_XML_COLOR_AMBIENT, ambient);
        saveColorRGBA(root, M3D_MATERIAL_XML_COLOR_DIFFUSE, diffuse);
        saveColorRGBA(root, M3D_MATERIAL_XML_COLOR_EMISSION, emission);
        saveColorRGBA(root, M3D_MATERIAL_XML_COLOR_SPECULAR, specular);
        saveFloat(root,     M3D_MATERIAL_XML_SHININESS, shininess);
        saveFloat( root,    M3D_MATERIAL_XML_ALPHATEST, alphaTest);
        saveString(root,    M3D_MATERIAL_XML_SHADER, shader ? shader->getName() : M3D_XML_NULL);
        saveInt(root,       M3D_MATERIAL_XML_BLEND, blend);

        for (int i = 0; i < M3D_MULTI_TEXTURE_COUNT; i++)
        {
            std::stringstream ss;
            ss << M3D_MATERIAL_XML_TEXTURE << i;
            ResourceManager::getTextures()->saveTexture(root, ss.str().c_str(), texture[i]);
        }

        XMLElement* attribute = root->GetDocument()->NewElement( M3D_MATERIAL_XML_VARIABLES );

        int size = variables.size();
        for (int i = 0; i < size; i ++)
        {
            variables[i]->save(attribute);
        }
        root->LinkEndChild(attribute);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Material::load(XMLElement* root)
{
    ambient   = loadColorRGBA(root, M3D_MATERIAL_XML_COLOR_AMBIENT);
    diffuse   = loadColorRGBA(root, M3D_MATERIAL_XML_COLOR_DIFFUSE);
    emission  = loadColorRGBA(root, M3D_MATERIAL_XML_COLOR_EMISSION);
    specular  = loadColorRGBA(root, M3D_MATERIAL_XML_COLOR_SPECULAR);
    shininess = loadFloat(root,     M3D_MATERIAL_XML_SHININESS);
    alphaTest = loadFloat(root,     M3D_MATERIAL_XML_ALPHATEST);
    std::string shaderFile = loadString(root,    M3D_MATERIAL_XML_SHADER);
    blend     = BLEND(loadInt(root, M3D_MATERIAL_XML_BLEND));

    XMLElement* xml = NULL;

    for (int i = 0; i < M3D_MULTI_TEXTURE_COUNT; i++)
    {
        texture[i] = NULL;

        std::stringstream ss;
        ss << M3D_MATERIAL_XML_TEXTURE << i;

        xml = root->FirstChildElement( ss.str().c_str() );
        if (xml)
        {
            texture[i] = ResourceManager::getTextures()->loadTexture(xml);
        }
    }

    if (shaderFile.compare(M3D_XML_NULL) != 0)
    {
        xml = root->FirstChildElement( M3D_MATERIAL_XML_VARIABLES );
        bool created = false;
        setShader(ResourceManager::getInstance()->addShader(shaderFile, created), xml);
    }
    else
    {
        setShader(NULL);
    }
    return root;
}

bool Magic3D::Material::save()
{
    XMLDocument doc;
    XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
    doc.LinkEndChild( decl );

    XMLElement* root = doc.NewElement( "MGE" );
    doc.LinkEndChild( root );

    XMLComment* comment = doc.NewComment(" MGE Material ");
    root->LinkEndChild( comment );

    save(root);

    bool result = doc.SaveFile( getMaterialFile().c_str() ) == XML_SUCCESS;
    return result;
}

bool Magic3D::Material::load()
{
    XMLDocument* doc = new XMLDocument();

    bool result = false;
    if (ResourceManager::getInstance()->getPackage())
    {
        Memory mem;
        result = ResourceManager::getInstance()->unpack(getMaterialFile(), &mem);
        std::string str = mem.getBuffer()->str();
        result = doc->Parse(str.c_str(), str.size()) == XML_SUCCESS && result;
    }
    else
    {
        result = doc->LoadFile(getMaterialFile().c_str()) == XML_SUCCESS;
    }

    if (result)
    {
        XMLElement* pElem = doc->FirstChildElement();

        // should always have a valid root but handle gracefully if it does
        if (pElem)
        {
            load(pElem);
        }
    }

    if (doc)
    {
        delete doc;
        doc = NULL;
    }

    return result;
}
