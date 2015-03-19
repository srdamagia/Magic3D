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

#ifndef MAGIC3D_RESOURCES_SHADER_H
#define MAGIC3D_RESOURCES_SHADER_H

#include <magic3d/resource/resources.h>

namespace Magic3D
{
#define M3D_PATH_SHADER "shader/"
#define M3D_VERTEX_SHADER_FILE ".vsh"
#define M3D_FRAGMENT_SHADER_FILE ".fsh"

#define M3D_DEFAULT_SHADER_VERTEX "vertex.glsl"
#define M3D_DEFAULT_SHADER_FRAGMENT "fragment.glsl"
#define M3D_DEFAULT_SHADER_DEFINES "defines.glsl"

#define M3D_DEFAULT_SHADER "default"
#define M3D_DEFAULT_SHADER_GIZMO "gizmo"
#define M3D_DEFAULT_SHADER_BLUR "blur"
#define M3D_DEFAULT_SHADER_GUI "gui"
#define M3D_DEFAULT_SHADER_PARTICLES "particles"
#define M3D_DEFAULT_SHADER_BUMP "bump"
#define M3D_DEFAULT_SHADER_TOON "toon"

enum SHADER_VAR
{
    eSHADER_VAR_FLOAT,
    eSHADER_VAR_FLOAT_VEC2,
    eSHADER_VAR_FLOAT_VEC3,
    eSHADER_VAR_FLOAT_VEC4,
    eSHADER_VAR_INT,
    eSHADER_VAR_INT_VEC2,
    eSHADER_VAR_INT_VEC3,
    eSHADER_VAR_INT_VEC4,
    eSHADER_VAR_BOOL,
    eSHADER_VAR_BOOL_VEC2,
    eSHADER_VAR_BOOL_VEC3,
    eSHADER_VAR_BOOL_VEC4,
    eSHADER_VAR_FLOAT_MAT2,
    eSHADER_VAR_FLOAT_MAT3,
    eSHADER_VAR_FLOAT_MAT4,
    eSHADER_VAR_SAMPLER_2D,
    eSHADER_VAR_SAMPLER_CUBE,
    eSHADER_VAR_NONE
};

struct ShaderVar
{
    std::string name;
    SHADER_VAR type;
    int location;
    int size;
};

class Shader {
private:
    std::string name;
    std::vector<ShaderVar> userVars;

    std::string getShaderFile();
public:
    unsigned int id;
    int defines;

    Shader(std::string name, unsigned int id);
    virtual ~Shader();

    const std::string& getName();

    static Shader* load(std::string name);
    static bool compile(Shader* shader);

    void addVar(std::string name, SHADER_VAR type, int location, int size);
    void clearVars();
    ShaderVar* getVar(std::string name);
    ShaderVar* getVar(int index);
    int getVarCount();
};

//template <class M3D_Class>
class ResourcesShader : public Resources<Shader>
{
public:
    ResourcesShader();
    virtual ~ResourcesShader();

    virtual bool remove(std::string name);

    Shader* load(std::string name, bool& created);
    Shader* compile(std::string file, bool& created);

    std::string getPath(std::string shader);
    virtual void addDefault();
};

}

#endif // MAGIC3D_RESOURCES_SHADER_H
