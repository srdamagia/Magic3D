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

#if !defined(MAGIC3D_LEGACY)
#ifndef MAGIC3D_RENDERER_OPENGL_SHADERS_H
#define MAGIC3D_RENDERER_OPENGL_SHADERS_H

#if defined(MAGIC3D_OES2)
#define M3D_SHADER_MAX_LIGHTS 8
#else
#define M3D_SHADER_MAX_LIGHTS 16
#endif

#include <magic3d/renderer/opengl/renderer_opengl.h>

namespace Magic3D
{

class GLSLShader : public Shader
{
public:
    GLint uniforms[eUNIFORM_COUNT];
    std::vector<ShaderLightUniform> uniforms_light;
    ShaderLightUniform lightShadow;
    int lights;

    GLSLShader(std::string file, unsigned int id);
    virtual ~GLSLShader();
};

class RendererOpenGL_Shaders : public Renderer_OpenGL
{
private:
    std::vector <std::string> shaderVars;
    Shader* shaderDefault;
    Shader* shaderGizmo;
    Shader* shaderBlur;
    Material* materialScreen;
    Material* materialDefault;

    Shader* lastShader;
    int lastLightsCount;

    bool needUpdateTextureShader;

    void renderLightsGizmos(Camera* camera, ViewPort* view);
    void renderSoundsGizmos(Camera* camera, ViewPort* view);

    bool prepareLights(Camera* camera);
    bool renderLights(GLSLShader* shader, const RenderObject* object);

    bool prepareMatrix(Camera* camera, const RenderObject *object, float z);
    bool renderObject(Camera* camera, const RenderObject *object, bool textures, bool lights, bool gizmos);
    bool prepareMaterial(GLSLShader *shader, Material* material, Object* object);
    bool prepareTextures(MeshData* data, GLSLShader* shader);

    bool renderObjects(Camera* camera, std::list<RenderObject>* objects, Scene* scene, bool textures, bool lights, bool gizmos);
    bool renderMeshData(MeshData* data, GLSLShader* shader, Object* object);
    bool renderTextures(Object* object, Mesh* mesh, Material* material);

    bool prepareGizmo(Matrix4& modelView, ColorRGBA color, bool vertexColor, bool depth);
    bool renderGrid(Camera* camera, Object* object, ViewPort* view);
    bool renderAxis(Camera* camera, Object* object);
    bool renderGizmo(Camera* camera, ViewPort* view, Object* object, GIZMO_TYPE type, ColorRGBA color, Vector3 scale, bool physics, bool vertexColor, bool wire, bool depth);
    bool renderBoundingBox(Camera* camera, Object* object);
    bool renderSkeleton(Camera* camera, Object* object);
    bool renderBone(Camera* camera, Object* object, Bone* bone);

    bool renderDebug(Camera* camera, ViewPort* view);

    void setDefaultShaderVars();
    bool isShaderVarDefault(const std::string& variable);
    std::string getUniformTypeName(SHADER_VAR type);
    SHADER_VAR getUniformType(GLenum type);

protected:
    RendererOpenGL_Shaders();
    virtual ~RendererOpenGL_Shaders();
    virtual void initialize();

public:
    static bool start();
    virtual bool render(Scene* scene);
    virtual bool renderFBO(OpenGL_FBO* fbo, ViewPort* view, Scene* scene);
    virtual void renderScreen(Camera* camera);

    virtual bool renderShadows(Scene* scene);
    virtual bool renderReflections(Scene* scene, Camera* camera, FBO* fbo, ViewPort* view);
    virtual bool renderGlow(Scene* scene, Camera* camera, ViewPort* view);


    virtual void setShaderDefault(Shader* shader);
    virtual void setShaderGizmo(Shader* shader);
    virtual void setShaderBlur(Shader* shader);
    virtual void setMaterialScreen(Material* material);
    virtual void setMaterialDefault(Material* material);
    virtual Shader* createShader(std::string file);
    virtual bool updateShader(Shader* shader);
    virtual void deleteShader(unsigned int shader);
    virtual bool compileShader(unsigned int* shader, SHADER_TYPE type, std::string file);
    virtual bool linkProgram(unsigned int prog);
    virtual bool validateProgram(unsigned int prog);
    virtual bool updateShaderUserVariables(Shader* shader);

    virtual void blurTexture(FBO* fbo, int amount, float strength, bool gaussian);

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    virtual void drawTransform(const btTransform& transform, btScalar orthoLen);
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    void setUniform1i(GLint location, GLint v0);
    void setUniform1f(GLint location, GLfloat v0);
    void setUniform2i(GLint location, GLint v0, GLint v1);
    void setUniform2f(GLint location, GLfloat v0, GLfloat v1);
    void setUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    void setUniform3fv(GLint location, GLsizei count, const GLfloat *value);
    void setUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void setUniform4fv(GLint location, GLsizei count, const GLfloat *value);
    void setUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
};
}
#endif
#endif // MAGIC3D_RENDERER_OPENGL_SHADERS_H
