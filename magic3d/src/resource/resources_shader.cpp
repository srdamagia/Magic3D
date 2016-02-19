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

#include <magic3d/magic3d.h>
#include <magic3d/resource/resources_shader.h>

Magic3D::Shader::Shader(std::string name, unsigned int id)
{
    this->name = name;
    this->id = id;
    this->defines = 0;
}

Magic3D::Shader::~Shader()
{
    Renderer::getInstance()->deleteShader(id);
}

const std::string& Magic3D::Shader::getName()
{
    return name;
}

std::string Magic3D::Shader::getShaderFile()
{
    std::string fileName = ResourceManager::getShaders()->getPath(name);
    return fileName;
}

Magic3D::Shader* Magic3D::Shader::load(std::string name)
{
    Shader* shader = Renderer::getInstance()->createShader(name);

    if (shader)
    {
        Log::logFormat(eLOG_RENDERER, "Shader Program \"%s\" created.", name.c_str());
    }

    return shader;
}

bool Magic3D::Shader::compile(Shader* shader)
{
    bool result = Renderer::getInstance()->updateShader(shader);
    if (result)
    {
        Log::logFormat(eLOG_RENDERER, "Shader Program \"%s\" updated.", shader->getName().c_str());
    }

    return result;
}

void Magic3D::Shader::addVar(std::string name, SHADER_VAR type, int location, int size)
{
    ShaderVar variable;
    variable.name = name;
    variable.type = type;
    variable.location = location;
    variable.size = size;

    userVars.push_back(variable);
}

void Magic3D::Shader::clearVars()
{
    userVars.clear();
}

Magic3D::ShaderVar* Magic3D::Shader::getVar(std::string name)
{
    ShaderVar* result = NULL;
    unsigned int size = userVars.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (userVars[i].name.compare(name) == 0)
        {
            result = &userVars[i];
        }
    }

    return result;
}

Magic3D::ShaderVar* Magic3D::Shader::getVar(int index)
{
    ShaderVar* result = NULL;
    if (index >= 0 && index < (int)userVars.size())
    {
        result = &userVars[index];
    }
    return result;
}

int Magic3D::Shader::getVarCount()
{
    return userVars.size();
}

//******************************************************************************
Magic3D::ResourcesShader::ResourcesShader() : Resources<Shader>()
{
    resourceName = M3D_RESOURCE_SHADER;
}

Magic3D::ResourcesShader::~ResourcesShader()
{

}

bool Magic3D::ResourcesShader::remove(std::string name)
{
    bool result = false;
    Shader* shader = Resources<Shader>::get(name);
    Resources<Shader>::resources.erase(name);
    if (shader)
    {
        typename std::map<std::string, Material*>::const_iterator it_o = ResourceManager::getMaterials()->begin();

        while (it_o != ResourceManager::getMaterials()->end())
        {
            Material* child = (*it_o++).second;

            if (child->getShader() == shader)
            {
                child->setShader(NULL);
            }
        }

        delete shader;
        Log::logFormat(eLOG_RENDERER, "Shader Program \"%s\" deleted.", name.c_str());

        result = true;
    }

    return result;
}

Magic3D::Shader* Magic3D::ResourcesShader::load(std::string name, bool& created)
{
    Shader* result = Resources<Shader>::get(name);
    created = !result;

    if (!result)
    {
        result = Shader::load(name);
        add(result);
    }

    return result;
}

Magic3D::Shader* Magic3D::ResourcesShader::compile(std::string file, bool& created)
{
    Shader* shader = Resources<Shader>::get(file);
    created = !shader;

    if (shader)
    {
        typename std::map<std::string, Material*>::const_iterator it_o = ResourceManager::getMaterials()->begin();
        while (it_o != ResourceManager::getMaterials()->end())
        {
            Material* child = (*it_o++).second;

            if (child->getShader() == shader)
            {
                child->clearVars();
            }
        }

        Shader::compile(shader);

        it_o = ResourceManager::getMaterials()->begin();
        while (it_o != ResourceManager::getMaterials()->end())
        {
            Material* child = (*it_o++).second;

            if (child->getShader() == shader)
            {
                child->setShader(shader);
            }
        }
    }

    return shader;
}

std::string Magic3D::ResourcesShader::getPath(std::string shader)
{
    std::string fileName = Resources<Shader>::getPath() + M3D_PATH_SHADER + shader;

    return fileName;
}

void Magic3D::ResourcesShader::addDefault()
{
    bool created = false;

    Shader* shader = NULL;

    shader = load(M3D_DEFAULT_SHADER, created);
    Renderer::getInstance()->setShaderDefault(shader);

    shader = load(M3D_DEFAULT_SHADER_GIZMO, created);
    Renderer::getInstance()->setShaderGizmo(shader);

    shader = load(M3D_DEFAULT_SHADER_BLUR, created);
    Renderer::getInstance()->setShaderBlur(shader);

    load(M3D_DEFAULT_SHADER_GUI, created);
    load(M3D_DEFAULT_SHADER_PARTICLES, created);
    load(M3D_DEFAULT_SHADER_BUMP, created);
    load(M3D_DEFAULT_SHADER_TOON, created);
    load(M3D_DEFAULT_SHADER_VEGETATION, created);
    load(M3D_DEFAULT_SHADER_WATER, created);
    load(M3D_DEFAULT_SHADER_TERRAIN, created);
}
