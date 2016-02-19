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

#ifndef MAGIC3D_RENDERER_OPENGL_H
#define MAGIC3D_RENDERER_OPENGL_H

#include <magic3d/renderer/renderer.h>
#if defined(__APPLE__)
    #if defined(MAGIC3D_OES1)
        #include <OpenGLES/ES1/gl.h>
        #include <OpenGLES/ES1/glext.h>
    #elif defined(MAGIC3D_OES2)
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #else
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
    #endif
#else
    #if defined(__linux__)
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#if defined(__WIN32__)
#if !defined(MAGIC3D_LEGACY) && !defined(MAGIC3D_OES2)
extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC      glClientActiveTexture;

extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM2IPROC                glUniform2i;
extern PFNGLUNIFORM3IPROC                glUniform3i;
extern PFNGLUNIFORM4IPROC                glUniform4i;
extern PFNGLUNIFORM1IVPROC               glUniform1iv;
extern PFNGLUNIFORM2IVPROC               glUniform2iv;
extern PFNGLUNIFORM3IVPROC               glUniform3iv;
extern PFNGLUNIFORM4IVPROC               glUniform4iv;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLUNIFORM2FPROC                glUniform2f;
extern PFNGLUNIFORM3FPROC                glUniform3f;
extern PFNGLUNIFORM4FPROC                glUniform4f;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM2FVPROC               glUniform2fv;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
extern PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;

// Shader
extern PFNGLCREATESHADERPROC      glCreateShader;
extern PFNGLDELETESHADERPROC      glDeleteShader;
extern PFNGLSHADERSOURCEPROC      glShaderSource;
extern PFNGLCOMPILESHADERPROC     glCompileShader;
extern PFNGLGETSHADERIVPROC       glGetShaderiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLVALIDATEPROGRAMPROC   glValidateProgram;

// VBO
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLBINDBUFFERPROC	         glBindBuffer;
extern PFNGLBUFFERDATAPROC	         glBufferData;
extern PFNGLBUFFERSUBDATAPROC        glBufferSubData;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLVERTEXATTRIBPOINTERPROC  glVertexAttribPointer;

// VAO
extern PFNGLGENVERTEXARRAYSPROC      glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC      glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC   glDeleteVertexArrays;

//FBO
extern PFNGLDRAWBUFFERSPROC                            glDrawBuffers;
extern PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatus;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPEXTPROC                      glGenerateMipmap;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbuffer;

// Renderbuffer object
extern PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          glGetRenderbufferParameteriv;
extern PFNGLISRENDERBUFFEREXTPROC                      glIsRenderbuffer;

extern PFNGLGENBUFFERSARBPROC           glGenBuffersARB;           // VBO Name Generation Procedure
extern PFNGLBINDBUFFERARBPROC           glBindBufferARB;           // VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC           glBufferDataARB;           // VBO Data Loading Procedure
extern PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB;        // VBO Sub Data Loading Procedure
extern PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB;        // VBO Deletion Procedure
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB; // return various parameters of VBO
extern PFNGLMAPBUFFERARBPROC            glMapBufferARB;            // map VBO procedure
extern PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB;          // unmap VBO procedure

extern PFNGLBLENDEQUATIONEXTPROC         glBlendEquation;
extern PFNGLBLENDEQUATIONSEPARATEEXTPROC glBlendEquationSeparate;
#endif
#endif

#if defined(__APPLE__)
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define GL_TEXTURE_MAX_LEVEL GL_TEXTURE_MAX_LEVEL_APPLE
#else
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
#endif

#if defined(MAGIC3D_OES1)
#define GL_UINT GL_UNSIGNED_SHORT
#elif defined(MAGIC3D_OES2)
#define GL_UINT GL_UNSIGNED_SHORT
#else
#define GL_UINT GL_UNSIGNED_INT
#endif

#if defined(DEBUG)
#define check_gl_error() Renderer_OpenGL::checkGLError(__FILE__, __LINE__)
#else
#define check_gl_error()
#endif

namespace Magic3D
{

enum VERTEX
{
    eVERTEX_POSITION,
    eVERTEX_NORMAL,
    eVERTEX_TANGENT,
    eVERTEX_COLOR,
    eVERTEX_UV0,
    eVERTEX_UV1,
    eVERTEX_WEIGHTS,
    eVERTEX_BONES
};

enum SHADER_UNIFORMS
{
    eUNIFORM_RENDER_MODE,

    eUNIFORM_PROJECTION,
    eUNIFORM_MODELVIEW,
    eUNIFORM_MODEL,
    eUNIFORM_VIEW,
    eUNIFORM_SHADOWS,
    eUNIFORM_CAMERA,

    eUNIFORM_SKIN,
    eUNIFORM_SKELETON,

    eUNIFORM_DISTORTION,
    eUNIFORM_DISTORTIONS,

    eUNIFORM_ZNEAR,
    eUNIFORM_ZFAR,

    eUNIFORM_TEXTURE_0,
    eUNIFORM_TEXTURE_1,
    eUNIFORM_TEXTURE_SHADOWS,
    eUNIFORM_TEXTURE_REFLECTION,
    eUNIFORM_TEXTURE_DEPTH,

    eUNIFORM_WINDOW,
    eUNIFORM_WATER,
    eUNIFORM_WIND,

    eUNIFORM_USE_TEXTURE_0,
    eUNIFORM_USE_TEXTURE_1,
    eUNIFORM_USE_TEXTURE_SHADOWS,
    eUNIFORM_USE_TEXTURE_REFLECTION,
    eUNIFORM_USE_TEXTURE_DEPTH,
    eUNIFORM_SIZE_TEXTURE_0,
    eUNIFORM_SIZE_TEXTURE_1,
    eUNIFORM_RENDER_TO_TEXTURE,

    eUNIFORM_FOG_COLOR,
    eUNIFORM_FOG_ATTENUATION,

    eUNIFORM_AMBIENT_COLOR,

    eUNIFORM_MATERIAL_AMBIENT,
    eUNIFORM_MATERIAL_DIFFUSE,
    eUNIFORM_MATERIAL_EMISSION,
    eUNIFORM_MATERIAL_SPECULAR,
    eUNIFORM_MATERIAL_SHININESS,
    eUNIFORM_MATERIAL_ALPHATEST,

    eUNIFORM_LIGHTS,

    eUNIFORM_IS_VERTEX_COLOR,

    eUNIFORM_TIME,
    eUNIFORM_TIME_SIN,
    eUNIFORM_RANDOM,

    eUNIFORM_BLUR_ORIENTATION,
    eUNIFORM_BLUR_AMOUNT,
    eUNIFORM_BLUR_STRENGTH,
    eUNIFORM_BLUR_GAUSSIAN,

    eUNIFORM_BARREL_DISTORTION,
    eUNIFORM_SCREEN_ASPECT,

    eUNIFORM_COUNT
};

struct ShaderLightUniform
{
    GLint position;
    GLint direction;
    GLint ambient;
    GLint diffuse;
    GLint specular;

    GLint attenuation;
    GLint cutoff;
};

class OpenGL_FBO : public FBO
{
private:
    GLuint id;

    GLuint colorBuffer;
    GLuint depthBuffer;

    int width;
    int height;
    int attachments;
    bool color;
    bool depth;
    bool depthTexture;
protected:
    GLuint createTexture(bool depth);

public:
    OpenGL_FBO(int width, int height, bool color, bool depth, bool depthTexture);
    virtual ~OpenGL_FBO();

    bool isReady();

    GLuint getID();

    GLuint getColorTexture();
    GLuint getDepthTexture();

    int getWidth();
    int getHeight();
    
    int getAttachmentsCount();

    static std::string formatToString(GLenum format);
    static bool checkFramebufferStatus(GLuint id);
    static void printFramebufferInfo(GLenum target, GLuint fbo);

    static std::string getTextureParameters(GLuint id);
    static std::string getRenderbufferParameters(GLuint id);
};

class Renderer_OpenGL : public Renderer
{
private:
    std::vector <std::string> extensions;

    void createCheckTexture();
    void createScreenVBO();
    void createPlaneVBO();
    void createPlaneVBO(RENDER_ID &id, int vcount, void* vertices, int tcount, void* triangles);

protected:
    GLint defaultFBO;
    OpenGL_FBO* fbo;
    OpenGL_FBO* fbo_blur;
    OpenGL_FBO* fbo_shadows;
    OpenGL_FBO* fbo_reflections;
    OpenGL_FBO* fbo_glow;
    OpenGL_FBO* fbo_screen;

    ViewPort* fboViewPort;
    ViewPort* screenViewPort;
    RENDER_ID renderIDScreen;
    RENDER_ID renderIDPlane;

    Matrix4 shadowBias;

    Vector4 water;
    Vector4 screenAspect;

    bool toTexture;
    bool shadows;
    bool reflection;
    bool glow;

    Renderer_OpenGL();
    virtual ~Renderer_OpenGL();
    virtual void initialize();

public:
    static bool checkGLError(const char* file, int line);
    virtual void setDefaultFBO(unsigned int fbo);
    virtual unsigned int getDefaultFBO();
    void createFBO();

    virtual unsigned int getRenderToTextureColor();
    virtual unsigned int getRenderToTextureDepth();

    virtual unsigned int getShadowsTexture();
    virtual unsigned int getReflectionsTexture();

    virtual unsigned int getGlowTextureColor();
    virtual unsigned int getGlowTextureDepth();

    virtual unsigned int getScreenTextureColor();
    virtual unsigned int getScreenTextureDepth();

    virtual int getRenderToTextureWidth();
    virtual int getRenderToTextureHeight();

    virtual int getShadowsTextureWidth();
    virtual int getShadowsTextureHeight();

    virtual int getReflectionsTextureWidth();
    virtual int getReflectionsTextureHeight();

    virtual int getGlowTextureWidth();
    virtual int getGlowTextureHeight();

    virtual int getScreenTextureWidth();
    virtual int getScreenTextureHeight();

    virtual bool renderShadows(Scene* scene);
    virtual bool renderReflections(Scene* scene, Camera* camera, FBO* fbo, ViewPort* view);
    virtual bool renderGlow(Scene* scene, Camera* camera, ViewPort* view);

    bool isExtensionSupported(const std::string& ext);
    bool hasShadows();
    bool isSoftShadows();
    bool hasReflections();
    bool hasGlow();

    virtual void resizeWindow(int width, int height);

    virtual unsigned int createTexture(Image* image, bool mipmap, bool clamp);
    virtual void deleteTexture(unsigned int texture);
    virtual void setTextureWrap(Texture* texture, bool clamp);
    virtual void blurTexture(FBO* fbo, int amount, float strength, bool gaussian);

    virtual RENDER_ID createVBO(void* vertices, int vcount, void* indexes, int icount);
    virtual bool createVBO(MeshData* data);
    virtual bool updateVBO(MeshData* data);
    virtual bool updateVBOVertices(uint id, float* data, int size);
    virtual RENDER_ID copyVBO(const RENDER_ID& id);
    virtual float* mapVBO(uint id);
    virtual bool unmapVBO();
    virtual vindex* mapIndexes(uint id);
    virtual bool unmapIndexes();
    virtual void deleteVBO(RENDER_ID id);

    virtual void view2D(ViewPort* view);
    virtual void view3D(ViewPort* view, STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);

    virtual void matrixToArray(float array[16], const Matrix4& matrix);
    virtual void arrayToMatrix(const float array[16], Matrix4& matrix);
};

}
#endif // MAGIC3D_RENDERER_OPENGL_H
