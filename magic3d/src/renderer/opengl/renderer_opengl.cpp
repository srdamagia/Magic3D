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
#if defined(__WIN32__)
#include <windows.h>
#endif
#include <magic3d/renderer/opengl/renderer_opengl.h>
#include <math.h>

#if defined(__WIN32__)
#if !defined(MAGIC3D_LEGACY) && !defined(MAGIC3D_OES2)
PFNGLACTIVETEXTUREPROC       glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = NULL;

PFNGLCREATEPROGRAMPROC            glCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC            glDeleteProgram = NULL;
PFNGLUSEPROGRAMPROC               glUseProgram = NULL;
PFNGLATTACHSHADERPROC             glAttachShader = NULL;
PFNGLDETACHSHADERPROC             glDetachShader = NULL;
PFNGLLINKPROGRAMPROC              glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC             glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog = NULL;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC                glUniform1i = NULL;
PFNGLUNIFORM2IPROC                glUniform2i = NULL;
PFNGLUNIFORM3IPROC                glUniform3i = NULL;
PFNGLUNIFORM4IPROC                glUniform4i = NULL;
PFNGLUNIFORM1IVPROC               glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC               glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC               glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC               glUniform4iv = NULL;
PFNGLUNIFORM1FPROC                glUniform1f = NULL;
PFNGLUNIFORM2FPROC                glUniform2f = NULL;
PFNGLUNIFORM3FPROC                glUniform3f = NULL;
PFNGLUNIFORM4FPROC                glUniform4f = NULL;
PFNGLUNIFORM1FVPROC               glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC               glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC               glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC               glUniform4fv = NULL;
PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv = NULL;
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation = NULL;
PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation = NULL;
PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform = NULL;

// Shader
PFNGLCREATESHADERPROC      glCreateShader = NULL;
PFNGLDELETESHADERPROC      glDeleteShader = NULL;
PFNGLSHADERSOURCEPROC      glShaderSource = NULL;
PFNGLCOMPILESHADERPROC     glCompileShader = NULL;
PFNGLGETSHADERIVPROC       glGetShaderiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLVALIDATEPROGRAMPROC   glValidateProgram = NULL;

// VBO
PFNGLGENBUFFERSPROC           glGenBuffers = NULL;
PFNGLBINDBUFFERPROC	          glBindBuffer = NULL;
PFNGLBUFFERDATAPROC	          glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC        glBufferSubData = NULL;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC  glVertexAttribPointer = NULL;

//VAO
PFNGLGENVERTEXARRAYSPROC      glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC      glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC   glDeleteVertexArrays = NULL;

//FBO
PFNGLDRAWBUFFERSPROC                            glDrawBuffers = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebuffer = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatus = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPEXTPROC                      glGenerateMipmap = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbuffer = NULL;

// Renderbuffer object
PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffers = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffers = NULL;
PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorage        = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          glGetRenderbufferParameteriv = NULL;
PFNGLISRENDERBUFFEREXTPROC                      glIsRenderbuffer = NULL;

PFNGLGENBUFFERSARBPROC           glGenBuffersARB = NULL;           // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC           glBindBufferARB = NULL;           // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC           glBufferDataARB = NULL;           // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB = NULL;        // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB = NULL;        // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL; // return various parameters of VBO
PFNGLMAPBUFFERARBPROC            glMapBufferARB = NULL;            // map VBO procedure
PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB = NULL;          // unmap VBO procedure

PFNGLBLENDEQUATIONEXTPROC         glBlendEquation = NULL;
PFNGLBLENDEQUATIONSEPARATEEXTPROC glBlendEquationSeparate = NULL;

#endif
bool WGLExtensionSupported(const char *extension_name)
{
    bool result = true;
    typedef const char *(WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;
    _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

    if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
    {
        result = false;
    }

    return result;
}
#endif

//******************************************************************************
Magic3D::OpenGL_FBO::OpenGL_FBO(int width, int height, bool color, bool depth, bool depthTexture)
{
    this->id     = 0;
    this->width  = width;
    this->height = height;
    this->attachments = 0;

    this->color        = color;
    this->depth        = depth;
    this->depthTexture = depthTexture;

    this->colorBuffer = 0;
    this->depthBuffer = 0;

    if (color)
    {
        colorBuffer = createTexture(false);
        Log::log(eLOG_RENDERER, "Color texture created.");
    }

    if (depth)
    {
        if (depthTexture)
        {
            depthBuffer = createTexture(true);
            Log::log(eLOG_RENDERER, "Depth texture created.");
        }
        else
        {
            glGenRenderbuffers(1, &depthBuffer);
            check_gl_error();
            Log::log(eLOG_RENDERER, "Depth buffer created.");
        }
    }

    glGenFramebuffers(1, &id);
    check_gl_error();
    Log::log(eLOG_RENDERER, "FBO created.");


    glBindFramebuffer(GL_FRAMEBUFFER, id);
    check_gl_error();
    if (color)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
#else
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
#endif
        attachments++;
        check_gl_error();
        Log::log(eLOG_RENDERER, "Color texture attached.");
    }
#if !(defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2))
    else
    {
        glDrawBuffer(GL_NONE);
        check_gl_error();
        glReadBuffer(GL_NONE);
        check_gl_error();
    }
#endif

    if (depth)
    {
        if (depthTexture)
        {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
#else
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
#endif
            attachments++;
            check_gl_error();
            Log::log(eLOG_RENDERER, "Depth texture attached.");
        }
        else
        {
            /*glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);*/

            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            check_gl_error();
            if (Renderer::getInstance()->getVersion() > 2)
            {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
#else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
#endif
                check_gl_error();
            }
            else
            {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
                check_gl_error();
            }
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
            attachments++;
            check_gl_error();
            Log::log(eLOG_RENDERER, "Depth buffer attached.");
        }
    }

    /*GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  attachments);*/
}

Magic3D::OpenGL_FBO::~OpenGL_FBO()
{
    if (color)
    {
        glDeleteTextures(1, &colorBuffer);
        check_gl_error();
    }
    if (depth)
    {
        if (depthTexture)
        {
            glDeleteTextures(1, &depthBuffer);
            check_gl_error();
        }
        else
        {
            glDeleteRenderbuffers(1, &depthBuffer);
            check_gl_error();
        }
    }
    glDeleteFramebuffers(1, &id);
    check_gl_error();
}

bool Magic3D::OpenGL_FBO::isReady()
{
    return checkFramebufferStatus(getID());
}

GLuint Magic3D::OpenGL_FBO::getID()
{
    return id;
}

GLuint Magic3D::OpenGL_FBO::getColorTexture()
{
    return colorBuffer;
}

GLuint Magic3D::OpenGL_FBO::getDepthTexture()
{
    return depthBuffer;
}

int Magic3D::OpenGL_FBO::getWidth()
{
    return width;
}

int Magic3D::OpenGL_FBO::getHeight()
{
    return height;
}

int Magic3D::OpenGL_FBO::getAttachmentsCount()
{
    return attachments;
}

//******************************************************************************
Magic3D::Renderer_OpenGL::Renderer_OpenGL() : Renderer()
{
    scene = NULL;
    matrix_projection = Matrix4::identity();
    matrix_model = Matrix4::identity();
    matrix_view = Matrix4::identity();
    matrix_modelView = Matrix4::identity();
    matrix_shadows = Matrix4::identity();
    matrix_camera = Matrix4::identity();

    fbo = NULL;
    fbo_blur = NULL;
    fbo_shadows = NULL;
    fbo_reflections = NULL;
    fbo_glow = NULL;
    fbo_screen = NULL;
    fboViewPort = new ViewPort(Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0);
    screenViewPort = new ViewPort(Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0);

    /*shadowBias.setCol0(Vector4(0.5f, 0.0f, 0.0f, 0.5f));
    shadowBias.setCol1(Vector4(0.0f, 0.5f, 0.0f, 0.5f));
    shadowBias.setCol2(Vector4(0.0f, 0.0f, 0.5f, 0.5f));
    shadowBias.setCol3(Vector4(0.0f, 0.0f, 0.0f, 1.0f));*/

    shadowBias.setRow(0, Vector4(0.5f,  0.f,  0.0f, 0.0f));
    shadowBias.setRow(1, Vector4(0.0f,  0.5f, 0.0f, 0.0f));
    shadowBias.setRow(2, Vector4(0.0f,  0.f,  1.0f, 0.0f));
    shadowBias.setRow(3, Vector4(0.5f,  0.5f, 0.5f, 1.0f));

    shadowBias = transpose(shadowBias);

    water = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

    toTexture = false;
    shadows = false;
    reflection = false;
    glow = false;
}

Magic3D::Renderer_OpenGL::~Renderer_OpenGL()
{
    deleteVBO(renderIDScreen);
    deleteVBO(renderIDPlane);

    if (fbo)
    {
        delete fbo;
        fbo = NULL;
    }

    if (fbo_blur)
    {
        delete fbo_blur;
        fbo_blur = NULL;
    }

    if (fbo_shadows)
    {
        delete fbo_shadows;
        fbo_shadows = NULL;
    }

    if (fbo_reflections)
    {
        delete fbo_reflections;
        fbo_reflections = NULL;
    }

    if (fbo_glow)
    {
        delete fbo_glow;
        fbo_glow = NULL;
    }

    if (fbo_screen)
    {
        delete fbo_screen;
        fbo_screen = NULL;
    }

    if (fboViewPort)
    {
        delete fboViewPort;
        fboViewPort = NULL;
    }

    if (screenViewPort)
    {
        delete screenViewPort;
        screenViewPort = NULL;
    }
}

void Magic3D::Renderer_OpenGL::initialize()
{
    char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"
    check_gl_error();
    version = ver[0] - '0';
    Log::logFormat(eLOG_RENDERER, "OpenGL: %s", ver);
    char ext[128][128]; // OpenGL extensions string
    int numExt;
    char* pt;
    char* auxStr;
    int  len;
    pt = (char*)glGetString(GL_EXTENSIONS);
    check_gl_error();
    len = strlen(pt);
    auxStr = new char[len+1];
    memcpy(auxStr, pt, len+1);
    for(numExt = 0, pt = strtok(auxStr, " "); (pt != NULL) && (numExt < 128); pt = strtok(NULL, " "), numExt++)
    {
        strcpy(ext[numExt], pt);
        extensions.push_back(ext[numExt]);
        Log::logFormat(eLOG_PLAINTEXT, "OPENGL Supported Extension: %s", ext[numExt]);
    }
    delete[] auxStr;

    std::sort(this->extensions.begin(), this->extensions.end());

#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
    this->extVBO          = true;
    this->extFBO          = true;
    this->extMapBuffer    = isExtensionSupported("GL_OES_mapbuffer");
#else
    this->extVBO          = isExtensionSupported("GL_ARB_vertex_buffer_object") || getVersion() > 2;
    this->extFBO          = isExtensionSupported("GL_EXT_framebuffer_object") || getVersion() > 2;
    this->extMapBuffer    = true;
#endif
    this->extDepthTexture = isExtensionSupported("GL_ARB_depth_texture") || isExtensionSupported("GL_OES_depth_texture");

#if defined(__WIN32__)
    if (WGLExtensionSupported("WGL_EXT_swap_control"))
    {
        typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
        PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
        wglSwapIntervalEXT(0);
    }
#endif

#if defined(__WIN32__)
#if !defined(MAGIC3D_LEGACY) && !defined(MAGIC3D_OES2)
    glActiveTexture       = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");

    glCreateProgram            = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glDeleteProgram            = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
    glUseProgram               = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glAttachShader             = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glDetachShader             = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
    glLinkProgram              = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glGetProgramiv             = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetShaderInfoLog         = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glGetUniformLocation       = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniform1i                = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glUniform2i                = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
    glUniform3i                = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
    glUniform4i                = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
    glUniform1iv               = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
    glUniform2iv               = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
    glUniform3iv               = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
    glUniform4iv               = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
    glUniform1f                = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform2f                = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
    glUniform3f                = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
    glUniform4f                = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glUniform1fv               = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
    glUniform2fv               = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
    glUniform3fv               = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
    glUniform4fv               = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
    glUniformMatrix2fv         = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
    glUniformMatrix3fv         = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
    glUniformMatrix4fv         = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glGetAttribLocation        = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
    glVertexAttrib1f           = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
    glVertexAttrib1fv          = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
    glVertexAttrib2fv          = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
    glVertexAttrib3fv          = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
    glVertexAttrib4fv          = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
    glEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
    glBindAttribLocation       = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
    glGetActiveUniform         = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

    // Shader
    glCreateShader  = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glDeleteShader  = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glShaderSource  = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderiv   = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glValidateProgram   = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");

    // VBO
    glGenBuffers           = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer           = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData           = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glBufferSubData        = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
    glDeleteBuffers        = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetBufferParameteriv");
    glVertexAttribPointer  = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

    //VAO
    glGenVertexArrays      = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray      = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glDeleteVertexArrays   = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");

    // FBO
    if (extFBO)
    {
        glDrawBuffers                         = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffersEXT");
        glGenFramebuffers                     = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
        glDeleteFramebuffers                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
        glBindFramebuffer                     = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
        glCheckFramebufferStatus              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
        glGenerateMipmap                      = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");
        glFramebufferTexture2D                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
        glFramebufferRenderbuffer             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");

        // Renderbuffer object
        glGenRenderbuffers                    = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
        glDeleteRenderbuffers                 = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
        glBindRenderbuffer                    = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
        glRenderbufferStorage                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
        glGetRenderbufferParameteriv          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
        glIsRenderbuffer                      = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
    }
    else
    {
        glDrawBuffers                         = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
        glGenFramebuffers                     = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffers");
        glDeleteFramebuffers                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffers");
        glBindFramebuffer                     = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebuffer");
        glCheckFramebufferStatus              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatus");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
        glGenerateMipmap                      = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmap");
        glFramebufferTexture2D                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2D");
        glFramebufferRenderbuffer             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbuffer");

        // Renderbuffer object
        glGenRenderbuffers                    = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffers");
        glDeleteRenderbuffers                 = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffers");
        glBindRenderbuffer                    = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbuffer");
        glRenderbufferStorage                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorage");
        glGetRenderbufferParameteriv          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
        glIsRenderbuffer                      = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbuffer");
    }

    if (extVBO)
    {
        glGenBuffersARB           = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
        glBindBufferARB           = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
        glBufferDataARB           = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
        glBufferSubDataARB        = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
        glDeleteBuffersARB        = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
        glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
        glMapBufferARB            = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
        glUnmapBufferARB          = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
    }

    glBlendEquation         = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT");
    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)wglGetProcAddress("glBlendEquationSeparateEXT");
#endif
#endif

#if defined(MAGIC3D_LEGACY)
    glEnableClientState(GL_VERTEX_ARRAY);
    check_gl_error();
    glEnableClientState(GL_NORMAL_ARRAY);
    check_gl_error();
    glEnableClientState(GL_COLOR_ARRAY);
    check_gl_error();

    glClientActiveTexture(GL_TEXTURE0);
    check_gl_error();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    check_gl_error();
    glClientActiveTexture(GL_TEXTURE1);
    check_gl_error();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    check_gl_error();
#endif

    createCheckTexture();

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    //Log::logFormat(eLOG_FAILURE, "Framebuffer: %d", defaultFBO);
    check_gl_error();

    Log::log(eLOG_SUCCESS, "Default OpenGL FBO.");
    //OpenGL_FBO::checkFramebufferStatus(defaultFBO);

    createFBO();
    createScreenVBO();
    createPlaneVBO();
}

unsigned int Magic3D::OpenGL_FBO::createTexture(bool depth)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    check_gl_error();

    glActiveTexture(GL_TEXTURE0);
    check_gl_error();
    glBindTexture(GL_TEXTURE_2D, textureId);
    check_gl_error();

    if (depth)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        check_gl_error();
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        check_gl_error();
#endif        
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        check_gl_error();
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check_gl_error();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    check_gl_error();

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_gl_error();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    check_gl_error();

    return textureId;
}

void Magic3D::Renderer_OpenGL::setDefaultFBO(unsigned int fbo)
{
    defaultFBO = fbo;
}

unsigned int Magic3D::Renderer_OpenGL::getDefaultFBO()
{
    unsigned int result = defaultFBO;
    if (isRenderingScreenEffects())
    {
        result = fbo_screen->getID();
    }
    return result;
}

void Magic3D::Renderer_OpenGL::createFBO()
{
    Log::log(eLOG_SUCCESS, "Creating default FBO.");
    fbo = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().FBO_MAP_SIZE, Magic3D::getInstance()->getConfiguration().FBO_MAP_SIZE, true, true, true);

    if (!fbo->isReady())
    {
        delete fbo;
        fbo = NULL;
    }

    Log::log(eLOG_SUCCESS, "Creating blur FBO.");
    fbo_blur = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().BLUR_MAP_SIZE, Magic3D::getInstance()->getConfiguration().BLUR_MAP_SIZE, true, false, false);

    if (!fbo_blur->isReady())
    {
        delete fbo_blur;
        fbo_blur = NULL;
    }

    if (hasShadows())
    {
        Log::log(eLOG_SUCCESS, "Creating shadows FBO.");
        fbo_shadows = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().SHADOW_MAP_SIZE, Magic3D::getInstance()->getConfiguration().SHADOW_MAP_SIZE, isSoftShadows(), true, true);

        if (!fbo_shadows->isReady())
        {
            delete fbo_shadows;
            fbo_shadows = NULL;
        }

    }
    else
    {
        fbo_shadows = NULL;
    }


    if (hasReflections())
    {
        Log::log(eLOG_SUCCESS, "Creating reflection FBO.");
        fbo_reflections = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().REFLECTION_MAP_SIZE, Magic3D::getInstance()->getConfiguration().REFLECTION_MAP_SIZE, true, true, false);

        if (!fbo_reflections->isReady())
        {
            delete fbo_reflections;
            fbo_reflections = NULL;
        }
    }
    else
    {
        fbo_reflections = NULL;
    }

    if (hasGlow())
    {
        Log::log(eLOG_SUCCESS, "Creating glow FBO.");
        fbo_glow = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().GLOW_MAP_SIZE, Magic3D::getInstance()->getConfiguration().GLOW_MAP_SIZE, true, true, false);

        if (!fbo_glow->isReady())
        {
            delete fbo_glow;
            fbo_glow = NULL;
        }
    }
    else
    {
        fbo_glow = NULL;
    }

    Log::log(eLOG_SUCCESS, "Creating screen FBO.");
    fbo_screen = new OpenGL_FBO(Magic3D::getInstance()->getConfiguration().SCREEN_FBO_SIZE, Magic3D::getInstance()->getConfiguration().SCREEN_FBO_SIZE, true, true, true);

    if (!fbo_screen->isReady())
    {
        delete fbo_screen;
        fbo_screen = NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    check_gl_error();
}

unsigned int Magic3D::Renderer_OpenGL::getRenderToTextureColor()
{
    return fbo ? fbo->getColorTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getRenderToTextureDepth()
{
    return fbo ? fbo->getDepthTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getShadowsTexture()
{
    return fbo_shadows ? (isSoftShadows() ? fbo_shadows->getColorTexture() : fbo_shadows->getDepthTexture()) : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getReflectionsTexture()
{
    return fbo_reflections ? fbo_reflections->getColorTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getGlowTextureColor()
{
    return fbo_glow ? fbo_glow->getColorTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getGlowTextureDepth()
{
    return fbo_glow ? fbo_glow->getDepthTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getScreenTextureColor()
{
    return fbo_screen ? fbo_screen->getColorTexture() : 0;
}

unsigned int Magic3D::Renderer_OpenGL::getScreenTextureDepth()
{
    return fbo_screen ? fbo_screen->getDepthTexture() : 0;
}

int Magic3D::Renderer_OpenGL::getRenderToTextureWidth()
{
    return fbo ? fbo->getWidth() : 0;
}

int Magic3D::Renderer_OpenGL::getRenderToTextureHeight()
{
    return fbo ? fbo->getHeight() : 0;
}

int Magic3D::Renderer_OpenGL::getShadowsTextureWidth()
{
    return fbo_shadows ? fbo_shadows->getWidth() : 0;
}

int Magic3D::Renderer_OpenGL::getShadowsTextureHeight()
{
    return fbo_shadows ? fbo_shadows->getHeight() : 0;
}

int Magic3D::Renderer_OpenGL::getReflectionsTextureWidth()
{
    return fbo_reflections ? fbo_reflections->getWidth() : 0;
}

int Magic3D::Renderer_OpenGL::getReflectionsTextureHeight()
{
    return fbo_reflections ? fbo_reflections->getHeight() : 0;
}

int Magic3D::Renderer_OpenGL::getGlowTextureWidth()
{
    return fbo_glow ? fbo_glow->getWidth() : 0;
}

int Magic3D::Renderer_OpenGL::getGlowTextureHeight()
{
    return fbo_glow ? fbo_glow->getHeight() : 0;
}

int Magic3D::Renderer_OpenGL::getScreenTextureWidth()
{
    return fbo_screen ? fbo_screen->getWidth() : 0;
}

int Magic3D::Renderer_OpenGL::getScreenTextureHeight()
{
    return fbo_screen ? fbo_screen->getHeight() : 0;
}

bool Magic3D::Renderer_OpenGL::renderShadows(Scene *scene)
{
    if (scene)
    {

    }
    return false;
}

bool Magic3D::Renderer_OpenGL::renderReflections(Scene *scene, Camera *camera, FBO* fbo, ViewPort* view)
{
    if (scene || camera || fbo || view)
    {

    }
    return false;
}

bool Magic3D::Renderer_OpenGL::renderGlow(Scene *scene, Camera *camera, ViewPort* view)
{
    if (scene || camera || view)
    {

    }
    return false;
}

bool Magic3D::Renderer_OpenGL::isExtensionSupported(const std::string& ext)
{
    std::vector<std::string>::const_iterator iter = this->extensions.begin();
    std::vector<std::string>::const_iterator endIter = this->extensions.end();

    while(iter != endIter)
    {
        if(ext == *iter)
            return true;
        else
            ++iter;
    }
    return false;
}

bool Magic3D::Renderer_OpenGL::hasShadows()
{
    return Magic3D::getInstance()->getConfiguration().SHADOWS;
}

bool Magic3D::Renderer_OpenGL::isSoftShadows()
{
    return !extDepthTexture || Magic3D::getInstance()->getConfiguration().SOFT_SHADOWS;
}

bool Magic3D::Renderer_OpenGL::hasReflections()
{
    return Magic3D::getInstance()->getConfiguration().REFLECTIONS;
}

bool Magic3D::Renderer_OpenGL::hasGlow()
{
    return Magic3D::getInstance()->getConfiguration().GLOW;
}

void Magic3D::Renderer_OpenGL::resizeWindow(int width, int height)
{
    if (getWindow())
    {
        getWindow()->resize(width, height);

        float aspectX = getScreenAspectX(false);
        float aspectY = getScreenAspectY(false);

        screenAspect = Vector4(aspectX, aspectY, getScreenAspectX(true), getScreenAspectY(true));

        if (fbo_screen)
        {
            screenViewPort->setWidth(fbo_screen->getWidth() * aspectX + 1);
            screenViewPort->setHeight(fbo_screen->getHeight() * aspectY + 1);
        }
    }
}

GLubyte* GenCheckImage( int width, int height, int checkSize )
{
   int x,
       y;
   GLubyte *pixels = (GLubyte *)malloc( width * height * 3 );

   if ( pixels == NULL )
      return NULL;

   for ( y = 0; y < height; y++ )
      for ( x = 0; x < width; x++ )
      {
         GLubyte rColor = 0;
         GLubyte bColor = 0;

         if ( ( x / checkSize ) % 2 == 0 )
         {
            rColor = 255 * ( ( y / checkSize ) % 2 );
            bColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
         }
         else
         {
            bColor = 255 * ( ( y / checkSize ) % 2 );
            rColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
         }

         pixels[(y * height + x) * 3] = rColor;
         pixels[(y * height + x) * 3 + 1] = 0;
         pixels[(y * height + x) * 3 + 2] = bColor;
      }

   return pixels;
}

void Magic3D::Renderer_OpenGL::createCheckTexture()
{
    // Texture object handle
    int width  = 8;
    int height = 8;
    GLubyte *pixels;

    pixels = GenCheckImage( width, height, 2 );

    // Generate a texture object
    glActiveTexture(GL_TEXTURE0);
    check_gl_error();

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, 0 );
    check_gl_error();

    // Load mipmap level 0
    if ( pixels != NULL )
    {
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );
        check_gl_error();

        free(pixels);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    check_gl_error();

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    check_gl_error();
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    check_gl_error();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_gl_error();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    check_gl_error();
}

void Magic3D::Renderer_OpenGL::createScreenVBO()
{
    if (extVBO)
    {
        int vcount = sizeof(PlaneVertex) * posteffectsVertices.size();
        void* vertices = &posteffectsVertices.front();

        int tcount = sizeof(vindex) * posteffectsTriangles.size();
        void* triangles = &posteffectsTriangles.front();

        createPlaneVBO(renderIDScreen, vcount, vertices, tcount, triangles);

        if (renderIDScreen.data > 0 || renderIDScreen.index > 0)
        {
            posteffectsVertices.clear();
            posteffectsTriangles.clear();
            vboCreated++;
        }
    }
}

void Magic3D::Renderer_OpenGL::createPlaneVBO()
{
    if (extVBO)
    {
        PlaneVertex tmp;
        PlaneVertex planeVertices[4];

        tmp.vertex = Vector3(-1.0f, -1.0f, 0.0f);
        tmp.uv = Texture2D(0.0f, 0.0f);
        planeVertices[0] = tmp;

        tmp.vertex = Vector3(1.0f, -1.0f, 0.0f);
        tmp.uv = Texture2D(1.0f, 0.0f);
        planeVertices[1] = tmp;

        tmp.vertex = Vector3(-1.0f, 1.0f, 0.0f);
        tmp.uv = Texture2D(0.0f, 1.0f);
        planeVertices[2] = tmp;

        tmp.vertex = Vector3(1.0f, 1.0f, 0.0f);
        tmp.uv = Texture2D(1.0f, 1.0f);
        planeVertices[3] = tmp;

        vindex planeTriangles[4] = {0, 1, 2, 3};

        int vcount = sizeof(PlaneVertex) * 4;
        void* vertices = &planeVertices;

        int tcount = sizeof(vindex) * 4;
        void* triangles = &planeTriangles;

        createPlaneVBO(renderIDPlane, vcount, vertices, tcount, triangles);

        if (renderIDPlane.data > 0 || renderIDPlane.index > 0)
        {
            vboCreated++;
        }
    }
}

void Magic3D::Renderer_OpenGL::createPlaneVBO(RENDER_ID &id, int vcount, void* vertices, int tcount, void* triangles)
{
    id.id = 0;
    id.data = 0;
    id.dataSize = 0;
    id.index = 0;
    id.indexSize = 0;

    glGenVertexArrays(1, &id.id);
    check_gl_error();
    glBindVertexArray(id.id);
    check_gl_error();

    glGenBuffers(1, &id.data);
    check_gl_error();
    glBindBuffer(GL_ARRAY_BUFFER, id.data);
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, vcount, vertices, GL_STATIC_DRAW);
    check_gl_error();
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &id.dataSize);
    check_gl_error();

    glEnableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glEnableVertexAttribArray(eVERTEX_UV0);
    check_gl_error();

    int stride = 0;

    glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(PlaneVertex), reinterpret_cast<void*>(stride));
    check_gl_error();
    stride += sizeof(Vector3);

    glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(PlaneVertex), reinterpret_cast<void*>(stride));
    check_gl_error();
    //stride += sizeof(Texture2D);

    glGenBuffers(1, &id.index);
    check_gl_error();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.index);
    check_gl_error();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tcount, triangles, GL_STATIC_DRAW);
    check_gl_error();
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &id.indexSize);
    check_gl_error();

    glBindVertexArray(0);
    check_gl_error();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    check_gl_error();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    check_gl_error();

    glDisableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glDisableVertexAttribArray(eVERTEX_UV0);
    check_gl_error();
}

unsigned int Magic3D::Renderer_OpenGL::createTexture(Image* image, bool mipmap, bool clamp)
{
    GLuint texID = 0;

    if (image)
    {
        glGenTextures(1, &texID);
        check_gl_error();

        glActiveTexture(GL_TEXTURE0);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, texID);
        check_gl_error();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        check_gl_error();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        check_gl_error();

        switch (image->getBPP())
        {
            case 32: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,             image->getWidth(), image->getHeight(), 0, GL_RGBA,            GL_UNSIGNED_BYTE,  image->getPixels()); break;
#if defined(MAGIC3D_OES1)
            case 24: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,              image->getWidth(), image->getHeight(), 0, GL_LUMINANCE,       GL_UNSIGNED_BYTE,  image->getPixels()); break;
#else
            case 24: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,              image->getWidth(), image->getHeight(), 0, GL_RGB,             GL_UNSIGNED_BYTE,  image->getPixels()); break;
#endif
            case 16: glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA,  image->getWidth(), image->getHeight(), 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image->getPixels()); break;
            case 8 : glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,            image->getWidth(), image->getHeight(), 0, GL_ALPHA,           GL_UNSIGNED_BYTE,  image->getPixels()); break;
            default: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,             image->getWidth(), image->getHeight(), 0, GL_RGBA,            GL_UNSIGNED_BYTE,  image->getPixels()); break;
        }
        check_gl_error();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        check_gl_error();

        if (mipmap)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            check_gl_error();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            check_gl_error();
        }
        else
        {
#if !defined(MAGIC3D_OES)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
#endif
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            check_gl_error();
        }
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        GLfloat fLargest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

        if (clamp)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            check_gl_error();
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            check_gl_error();
        }
    }

    return texID;
}

void Magic3D::Renderer_OpenGL::deleteTexture(unsigned int texture)
{
    glDeleteTextures(1, &texture);
    check_gl_error();
}

void Magic3D::Renderer_OpenGL::setTextureWrap(Texture* texture, bool clamp)
{
    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        check_gl_error();
        if (clamp)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            check_gl_error();
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            check_gl_error();
        }
        if (lastTexture0)
        {
            glBindTexture(GL_TEXTURE_2D, lastTexture0->id);
        }
        check_gl_error();
    }
}

void Magic3D::Renderer_OpenGL::blurTexture(FBO* fbo, int amount, float strength, bool gaussian)
{
    if (fbo || amount || strength || gaussian)
    {

    }
}

Magic3D::RENDER_ID Magic3D::Renderer_OpenGL::createVBO(void* vertices, int vcount, void* indexes, int icount)
{
    RENDER_ID result;
    result.id = 0;
    result.data = 0;
    result.dataSize = 0;
    result.index = 0;
    result.indexSize = 0;

    if (extVBO)
    {
        glGenVertexArrays(1, &result.id);
        check_gl_error();
        glBindVertexArray(result.id);
        check_gl_error();

        glGenBuffers(1, &result.data);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, result.data);
        check_gl_error();
        glBufferData(GL_ARRAY_BUFFER, vcount, vertices, GL_DYNAMIC_DRAW);
        check_gl_error();
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &result.dataSize);
        check_gl_error();

        glEnableVertexAttribArray(eVERTEX_POSITION);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_NORMAL);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_TANGENT);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_COLOR);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_UV0);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_UV1);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_WEIGHTS);
        check_gl_error();
        glEnableVertexAttribArray(eVERTEX_BONES);
        check_gl_error();

        int inc = sizeof(Vertex3D);
        int incVec3 = sizeof(Vector3);
        int incVec4 = sizeof(Vector4);
        int incTex = sizeof(Texture2D);
        int stride = 0;

        glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incVec3;

        glVertexAttribPointer (eVERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incVec3;

        glVertexAttribPointer (eVERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incVec3;

        glVertexAttribPointer(eVERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incVec4;

        glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incTex;

        glVertexAttribPointer(eVERTEX_UV1, 2, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incTex;

        glVertexAttribPointer(eVERTEX_WEIGHTS, 4, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        stride += incVec4;

        glVertexAttribPointer(eVERTEX_BONES, 4, GL_FLOAT, GL_FALSE, inc, reinterpret_cast<void*>(stride));
        check_gl_error();
        //stride += incVec4;

        glGenBuffers(1, &result.index);
        check_gl_error();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.index);
        check_gl_error();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount, indexes, GL_STATIC_DRAW);
        check_gl_error();
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &result.indexSize);
        check_gl_error();

        glBindVertexArray(0);
        check_gl_error();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        check_gl_error();

        glDisableVertexAttribArray(eVERTEX_POSITION);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_NORMAL);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_TANGENT);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_COLOR);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_UV0);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_UV1);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_WEIGHTS);
        check_gl_error();
        glDisableVertexAttribArray(eVERTEX_BONES);
        check_gl_error();
    }

    if (result.data > 0 || result.index > 0)
    {
        vboCreated++;
    }

    return result;
}

bool Magic3D::Renderer_OpenGL::createVBO(MeshData* data)
{
    bool result = false;
    if (extVBO)
    {
        int vcount = sizeof(Vertex3D) * data->getVertices()->size();
        void* vertices = &data->getVertices()->front();

        int icount = sizeof(vindex) * data->getIndexesCount();
        void* indexes = &data->getIndexes()->front();

        data->setRenderID(createVBO(vertices, vcount, indexes, icount));

        result = true;
    }
    return result;
}

bool Magic3D::Renderer_OpenGL::updateVBO(MeshData* data)
{
    bool result = false;
    if (data && data->isVBO())
    {
        int vcount = sizeof(Vertex3D) * data->getVertices()->size();
        void* vertices = &data->getVertices()->front();

        int icount = sizeof(vindex) * data->getIndexesCount();
        void* indexes = &data->getIndexes()->front();

        glBindBuffer(GL_ARRAY_BUFFER, data->getRenderID().data);
        check_gl_error();
        glBufferSubData(GL_ARRAY_BUFFER, 0, vcount, vertices);
        check_gl_error();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->getRenderID().index);
        check_gl_error();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, icount, indexes);
        check_gl_error();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        check_gl_error();

        result = true;
    }
    return result;
}

bool Magic3D::Renderer_OpenGL::updateVBOVertices(uint id, float* data, int size)
{
    bool result = false;
    if (data && id > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        check_gl_error();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        check_gl_error();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();

        result = true;
    }
    return result;
}

Magic3D::RENDER_ID Magic3D::Renderer_OpenGL::copyVBO(const RENDER_ID& id)
{
    RENDER_ID result;
    result.id = 0;
    result.data = 0;
    result.dataSize = 0;
    result.index = 0;
    result.indexSize = 0;

    if (extVBO && id.data > 0 && id.index > 0)
    {
        float* vertices = NULL;
        float* tmpVertices = mapVBO(id.data);

        vertices = new float[id.dataSize];
        memcpy(vertices, tmpVertices, id.dataSize);

        unmapVBO();

        vindex* indexes = NULL;
        vindex* tmpIndexes = mapIndexes(id.index);

        indexes = new vindex[id.indexSize];
        memcpy(indexes, tmpIndexes, id.indexSize);

        unmapIndexes();

        result = createVBO(vertices, id.dataSize, indexes, id.indexSize);

        delete[] vertices;
        delete[] indexes;
    }

    return result;
}

float* Magic3D::Renderer_OpenGL::mapVBO(uint id)
{
    float* result = NULL;
    if (extVBO)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        glBindBuffer(GL_ARRAY_BUFFER, id);
        check_gl_error();
        result = (float*)glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
        check_gl_error();
#else
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, id);
        check_gl_error();
        result = (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
        check_gl_error();
#endif
    }
    return result;
}

bool Magic3D::Renderer_OpenGL::unmapVBO()
{
    bool result = false;
    if (extVBO)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        result = glUnmapBufferOES(GL_ARRAY_BUFFER);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
#else
        result = glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        check_gl_error();
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        check_gl_error();
#endif
    }
    return result;
}

vindex* Magic3D::Renderer_OpenGL::mapIndexes(uint id)
{
    vindex* result = NULL;
    if (extVBO)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        check_gl_error();
        result = (vindex*)glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
        check_gl_error();
#else
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, id);
        check_gl_error();
        result = (vindex*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
        check_gl_error();
#endif
    }
    return result;
}

bool Magic3D::Renderer_OpenGL::unmapIndexes()
{
    bool result = false;
    if (extVBO)
    {
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
        result = glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
        check_gl_error();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        check_gl_error();
#else
        result = glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
        check_gl_error();
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        check_gl_error();
#endif
    }
    return result;
}

void Magic3D::Renderer_OpenGL::deleteVBO(RENDER_ID id)
{
    if (extVBO && (id.data > 0 || id.indexSize > 0))
    {
        glDeleteVertexArrays(1, &id.id);
        check_gl_error();

#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2) || !defined(MAGIC3D_LEGACY)
        glDeleteBuffers(1, &id.data);
        check_gl_error();
        glDeleteBuffers(1, &id.index);
        check_gl_error();
#else
        glDeleteBuffersARB(1, &id.data);
        check_gl_error();
        glDeleteBuffersARB(1, &id.index);
        check_gl_error();
#endif
        vboDestroyed++;
    }
}

void Magic3D::Renderer_OpenGL::view2D(ViewPort* view)
{
    Window* window = getWindow();

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int w = view->getWidth();
    int h = view->getHeight();
    //int ws = w;
    //int hs = h;

    bool landscape = false;
    if (window)
    {
        switch (window->getOrientation()) {
            case eWINDOW_ORIENTATION_NONE:
            case eWINDOW_ORIENTATION_PORTRAIT_UP:
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                w = w > 0 ? w : window->getWidth();
                h = h > 0 ? h : window->getHeight();
                //ws = window->getWidth();
                //hs = window->getHeight();
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                landscape = true;
                w = w > 0 ? w : window->getHeight();
                h = h > 0 ? h : window->getWidth();
                //ws = window->getHeight();
                //hs = window->getWidth();
                break;
            }
        }
    }

    const Vector4& area = view->getArea();
    x = area.getX() * w;
    y = area.getY() * h;
    width = area.getZ() * w;
    height = area.getW() * h;

    glViewport(x, y, width, height);
    check_gl_error();
    glScissor(x, y, width, height);
    check_gl_error();
    glEnable(GL_SCISSOR_TEST);
    check_gl_error();

    Camera* camera = view->getOrthographic();

    if (camera)
    {
        if (camera->getType() == eOBJECT_LIGHT)
        {
            float zoom = (1.0f / (w < h ? w : h)) * camera->getFar() * 0.5f;

            float x = (float)w * zoom;
            float y = (float)h * zoom;
            
            if (landscape)
            {
                matrix_projection = camera->view2D(-x, -y, x, y);
            }
            else
            {
                matrix_projection = camera->view2D(-x, x, -y, y);
            }
        }
        else
        {
            Vector3 aspect = Renderer::getInstance()->getWindow()->getWindowScreenAspect();
            matrix_projection = camera->view2D(0.0f, aspect.getX(), aspect.getY(), 0.0f);
        }
    }
    else
    {
        matrix_projection = Matrix4::identity();
    }

    /*GLfloat zNear = 0.01f;
    GLfloat zFar = 10000.0f;

    glOrtho(0.0f, w, h, 0.0f, zNear, zFar);
    check_gl_error();*/
}

void Magic3D::Renderer_OpenGL::view3D(ViewPort* view, STEREOSCOPY stereoscopy)
{
    Window* window = getWindow();

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int w = view->getWidth();
    int h = view->getHeight();

    w = w > 0 ? w : window->getWidth();
    h = h > 0 ? h : window->getHeight();

    if (window)
    {
        const Vector4& area = view->getArea();
        switch (window->getOrientation()) {
            case eWINDOW_ORIENTATION_NONE:
            case eWINDOW_ORIENTATION_PORTRAIT_UP:
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                x = area.getX() * w;
                y = area.getY() * h;
                width = area.getZ() * w;
                height = area.getW() * h;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                x = area.getX() * h;
                y = area.getY() * w;
                width = area.getZ() * h;
                height = area.getW() * w;
                break;
            }
        }
    }

    glViewport(x, y, width, height);
    check_gl_error();
    glScissor(x, y, width, height);
    check_gl_error();
    glEnable(GL_SCISSOR_TEST);
    check_gl_error();

    Camera* camera = view->getPerspective();

    if (camera)
    {
        float aspect = (float)w / (float)h;
        camera->view3D(aspect);
        matrix_projection = camera->getProjection(stereoscopy);
    }
    else
    {
        matrix_projection = Matrix4::identity();
    }

    /*GLfloat aspect = (GLfloat)w/(GLfloat)h;
    GLfloat zNear = 0.01f;
    GLfloat zFar = 10000.0f;
    GLfloat fovy = 45.0f;

    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    check_gl_error();*/
}

void Magic3D::Renderer_OpenGL::matrixToArray(float array[16], const Matrix4& matrix)
{
    array[0]  = matrix.getCol0().getX();
    array[1]  = matrix.getCol0().getY();
    array[2]  = matrix.getCol0().getZ();
    array[3]  = matrix.getCol0().getW();

    array[4]  = matrix.getCol1().getX();
    array[5]  = matrix.getCol1().getY();
    array[6]  = matrix.getCol1().getZ();
    array[7]  = matrix.getCol1().getW();

    array[8]  = matrix.getCol2().getX();
    array[9]  = matrix.getCol2().getY();
    array[10] = matrix.getCol2().getZ();
    array[11] = matrix.getCol2().getW();

    array[12] = matrix.getCol3().getX();
    array[13] = matrix.getCol3().getY();
    array[14] = matrix.getCol3().getZ();
    array[15] = matrix.getCol3().getW();
}

void Magic3D::Renderer_OpenGL::arrayToMatrix(const float array[16], Matrix4& matrix)
{
    matrix.setCol(0, Vector4(array[0], array[1], array[2], array[3]));
    matrix.setCol(1, Vector4(array[4], array[5], array[6], array[7]));
    matrix.setCol(2, Vector4(array[8], array[9], array[10], array[11]));
    matrix.setCol(3, Vector4(array[12], array[13], array[14], array[15]));
}

bool Magic3D::Renderer_OpenGL::checkGLError(const char* file, int line)
{
    GLenum error;
    error = glGetError();

    bool result = true;
    while (error != GL_NO_ERROR)
    {
        bool hasError = false;

        switch (error)
        {
            case GL_NO_ERROR:                      {                                                           hasError = false;  break;}
            case GL_INVALID_ENUM:                  {Log::log(eLOG_RENDERER, "Invalid Enum.");                  hasError = true; break;}
            case GL_INVALID_VALUE:                 {Log::log(eLOG_RENDERER, "Invalid Value.");                 hasError = true; break;}
            case GL_INVALID_OPERATION:             {Log::log(eLOG_RENDERER, "Invalid Operation.");             hasError = true; break;}
            case GL_INVALID_FRAMEBUFFER_OPERATION: {Log::log(eLOG_RENDERER, "Invalid Framebuffer Operation."); hasError = true; break;}
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
            case GL_STACK_OVERFLOW:                {Log::log(eLOG_RENDERER, "Stack Overflow.");                hasError = true; break;}
            case GL_STACK_UNDERFLOW:               {Log::log(eLOG_RENDERER, "Stack Underflow.");               hasError = true; break;}
#endif
            case GL_OUT_OF_MEMORY:                 {Log::log(eLOG_RENDERER, "Out of Memory.");                 hasError = true; break;}
            default:                               {Log::logFormat(eLOG_RENDERER, "Unknow Error: %d", error);  hasError = true; break;}
        }

        if (hasError)
        {
            result = false;
            Log::debug(file, line);
        }

        error = glGetError();
    }

    return result;
}

std::string Magic3D::OpenGL_FBO::formatToString(GLenum format)
{
    std::string formatName;

    switch(format)
    {
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
    case GL_STENCIL_INDEX:      // 0x1901
        formatName = "GL_STENCIL_INDEX";
        break;
#endif
    case GL_DEPTH_COMPONENT:    // 0x1902
        formatName = "GL_DEPTH_COMPONENT";
        break;
    case GL_ALPHA:              // 0x1906
        formatName = "GL_ALPHA";
        break;
    case GL_RGB:                // 0x1907
        formatName = "GL_RGB";
        break;
    case GL_RGBA:               // 0x1908
        formatName = "GL_RGBA";
        break;
    case GL_LUMINANCE:          // 0x1909
        formatName = "GL_LUMINANCE";
        break;
    case GL_LUMINANCE_ALPHA:    // 0x190A
        formatName = "GL_LUMINANCE_ALPHA";
        break;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
    case GL_R3_G3_B2:           // 0x2A10
        formatName = "GL_R3_G3_B2";
        break;
    case GL_ALPHA4:             // 0x803B
        formatName = "GL_ALPHA4";
        break;
    case GL_ALPHA8:             // 0x803C
        formatName = "GL_ALPHA8";
        break;
    case GL_ALPHA12:            // 0x803D
        formatName = "GL_ALPHA12";
        break;
    case GL_ALPHA16:            // 0x803E
        formatName = "GL_ALPHA16";
        break;
    case GL_LUMINANCE4:         // 0x803F
        formatName = "GL_LUMINANCE4";
        break;
    case GL_LUMINANCE8:         // 0x8040
        formatName = "GL_LUMINANCE8";
        break;
    case GL_LUMINANCE12:        // 0x8041
        formatName = "GL_LUMINANCE12";
        break;
    case GL_LUMINANCE16:        // 0x8042
        formatName = "GL_LUMINANCE16";
        break;
    case GL_LUMINANCE4_ALPHA4:  // 0x8043
        formatName = "GL_LUMINANCE4_ALPHA4";
        break;
    case GL_LUMINANCE6_ALPHA2:  // 0x8044
        formatName = "GL_LUMINANCE6_ALPHA2";
        break;
    case GL_LUMINANCE8_ALPHA8:  // 0x8045
        formatName = "GL_LUMINANCE8_ALPHA8";
        break;
    case GL_LUMINANCE12_ALPHA4: // 0x8046
        formatName = "GL_LUMINANCE12_ALPHA4";
        break;
    case GL_LUMINANCE12_ALPHA12:// 0x8047
        formatName = "GL_LUMINANCE12_ALPHA12";
        break;
    case GL_LUMINANCE16_ALPHA16:// 0x8048
        formatName = "GL_LUMINANCE16_ALPHA16";
        break;
    case GL_INTENSITY:          // 0x8049
        formatName = "GL_INTENSITY";
        break;
    case GL_INTENSITY4:         // 0x804A
        formatName = "GL_INTENSITY4";
        break;
    case GL_INTENSITY8:         // 0x804B
        formatName = "GL_INTENSITY8";
        break;
    case GL_INTENSITY12:        // 0x804C
        formatName = "GL_INTENSITY12";
        break;
    case GL_INTENSITY16:        // 0x804D
        formatName = "GL_INTENSITY16";
        break;
    case GL_RGB4:               // 0x804F
        formatName = "GL_RGB4";
        break;
    case GL_RGB5:               // 0x8050
        formatName = "GL_RGB5";
        break;
    case GL_RGB10:              // 0x8052
        formatName = "GL_RGB10";
        break;
    case GL_RGB12:              // 0x8053
        formatName = "GL_RGB12";
        break;
    case GL_RGB16:              // 0x8054
        formatName = "GL_RGB16";
        break;
    case GL_RGBA2:              // 0x8055
        formatName = "GL_RGBA2";
        break;
#endif
    case GL_RGBA4:              // 0x8056
        formatName = "GL_RGBA4";
        break;
    case GL_RGB5_A1:            // 0x8057
        formatName = "GL_RGB5_A1";
        break;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
        case GL_RGB8:               // 0x8051
        formatName = "GL_RGB8";
        break;
    case GL_RGBA8:              // 0x8058
        formatName = "GL_RGBA8";
        break;
    case GL_RGB10_A2:           // 0x8059
        formatName = "GL_RGB10_A2";
        break;
    case GL_RGBA12:             // 0x805A
        formatName = "GL_RGBA12";
        break;
    case GL_RGBA16:             // 0x805B
        formatName = "GL_RGBA16";
        break;
#else
        case GL_RGB8_OES:       // 0x8051
        formatName = "GL_RGB8_OES";
        break;
        case GL_RGBA8_OES:      // 0x8058
        formatName = "GL_RGBA8_OES";
        break;
#endif
    case GL_DEPTH_COMPONENT16:  // 0x81A5
        formatName = "GL_DEPTH_COMPONENT16";
        break;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
    case GL_DEPTH_COMPONENT24:  // 0x81A6
        formatName = "GL_DEPTH_COMPONENT24";
        break;
    case GL_DEPTH_COMPONENT32:  // 0x81A7
        formatName = "GL_DEPTH_COMPONENT32";
        break;
    case GL_DEPTH_STENCIL:      // 0x84F9
        formatName = "GL_DEPTH_STENCIL";
        break;
#endif
#if GL_VERSION_3_0
    case GL_RGBA32F:            // 0x8814
        formatName = "GL_RGBA32F";
        break;
    case GL_RGB32F:             // 0x8815
        formatName = "GL_RGB32F";
        break;
    case GL_RGBA16F:            // 0x881A
        formatName = "GL_RGBA16F";
        break;
    case GL_RGB16F:             // 0x881B
        formatName = "GL_RGB16F";
        break;
#endif
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
    case GL_DEPTH24_STENCIL8:   // 0x88F0
        formatName = "GL_DEPTH24_STENCIL8";
        break;
    case GL_DEPTH_COMPONENT32F: // 0x8CAC
        formatName = "GL_DEPTH_COMPONENT32F";
        break;
    case GL_DEPTH32F_STENCIL8:  // 0x8CAD
        formatName = "GL_DEPTH32F_STENCIL8";
        break;
#else
        case GL_DEPTH_STENCIL_OES:     // 0x84F9
        formatName = "GL_DEPTH_STENCIL_OES";
        break;
        case GL_UNSIGNED_INT_24_8_OES: // 0x84FA
        formatName = "GL_UNSIGNED_INT_24_8_OES";
        break;
        case GL_DEPTH24_STENCIL8_OES:  // 0x88F0
        formatName = "GL_DEPTH24_STENCIL8_OES";
        break;
#endif
    default:
        formatName = "Unknown Format";
    }

    return formatName;
}

void Magic3D::OpenGL_FBO::printFramebufferInfo(GLenum target, GLuint fbo)
{
    int res = 0;

    glBindFramebuffer(target, fbo);
    check_gl_error();

    int colorBufferCount = 0;
#if defined(MAGIC3D_OES1) || defined(MAGIC3D_OES2)
    colorBufferCount = 1;
#else
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &colorBufferCount);
    check_gl_error();
#endif

    for (int i = 0; i < colorBufferCount; i++) {
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
        check_gl_error();
        if (res != GL_NONE)
        {
            bool texture = res == GL_TEXTURE ? true : false;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
            GLint buffer;
            glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);
            check_gl_error();
            Log::logFormat(eLOG_PLAINTEXT, "Shader Output Location %d - color attachment %d", i, buffer - GL_COLOR_ATTACHMENT0);
#endif
            glGetFramebufferAttachmentParameteriv(target, GL_COLOR_ATTACHMENT0+i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
            check_gl_error();
            Log::logFormat(eLOG_PLAINTEXT, "Attachment Type: %s", texture ? getTextureParameters(res).c_str() : getRenderbufferParameters(res).c_str());
            Log::logFormat(eLOG_PLAINTEXT, "Attachment object name: %d", res);
        }
    }

    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
    check_gl_error();
    if (res != GL_NONE)
    {
        bool texture = res == GL_TEXTURE ? true : false;
        glGetFramebufferAttachmentParameteriv(target, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
        check_gl_error();
        Log::logFormat(eLOG_PLAINTEXT, "Attachment Type: %s", texture ? getTextureParameters(res).c_str() : getRenderbufferParameters(res).c_str());
        Log::logFormat(eLOG_PLAINTEXT, "Attachment object name: %d", res);
    }

    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
    check_gl_error();
    if (res != GL_NONE)
    {
        bool texture = res == GL_TEXTURE ? true : false;
        glGetFramebufferAttachmentParameteriv(target, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
        check_gl_error();
        Log::logFormat(eLOG_PLAINTEXT, "Attachment Type: %s", texture ? getTextureParameters(res).c_str() : getRenderbufferParameters(res).c_str());
        Log::logFormat(eLOG_PLAINTEXT, "Attachment object name: %d", res);
    }
}

bool Magic3D::OpenGL_FBO::checkFramebufferStatus(GLuint id)
{
    printFramebufferInfo(GL_FRAMEBUFFER, id);

   // check FBO status
   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   check_gl_error();
   switch(status)
   {
   case GL_FRAMEBUFFER_COMPLETE:
       Log::log(eLOG_RENDERER, "Framebuffer complete.");
       return true;

   case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Attachment is NOT complete.");
       return false;

   case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: No image is attached to FBO.");
       return false;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
   case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Attached images have different dimensions.");
       return false;

   case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Color attached images have different internal formats.");
       return false;

   case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Draw buffer.");
       return false;

   case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Read buffer.");
       return false;

   case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
       Log::log(eLOG_FAILURE, "Unsupported by FBO implementation.");
       return false;
           
   case GL_FRAMEBUFFER_UNDEFINED:
       Log::log(eLOG_FAILURE, "Framebuffer Undefined.");
       return false;
#else
   case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
       Log::log(eLOG_FAILURE, "Framebuffer incomplete: Attached images have different dimensions.");
       return false;
   case GL_FRAMEBUFFER_UNSUPPORTED:
       Log::log(eLOG_FAILURE, "Unsupported by FBO implementation.");
       return false;
#endif
   default:
      Log::logFormat(eLOG_FAILURE, "Unknow error: %d", status);
       return false;
   }
}

std::string Magic3D::OpenGL_FBO::getTextureParameters(GLuint id)
{
    if(glIsTexture(id) == GL_FALSE)
    {
        check_gl_error();
        return "Not texture object";
    }
    else
    {
        check_gl_error();
    }

    int width, height;
    std::string formatName;
#if !defined(MAGIC3D_OES1) && !defined(MAGIC3D_OES2)
    int format;
    glActiveTexture(GL_TEXTURE0);
    check_gl_error();
    glBindTexture(GL_TEXTURE_2D, id);
    check_gl_error();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);            // get texture width
    check_gl_error();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);          // get texture height
    check_gl_error();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format); // get texture internal format
    check_gl_error();

    formatName = formatToString(format);
#else
    width = 0;
    height = 0;
    formatName = "Unknown Format";
#endif

    std::stringstream ss;
    ss << "Texture - " << width << "x" << height << ", " << formatName;
    return ss.str();
}

std::string Magic3D::OpenGL_FBO::getRenderbufferParameters(GLuint id)
{
    if(glIsRenderbuffer(id) == GL_FALSE)
    {
        check_gl_error();
        return "Not Renderbuffer object";
    }
    else
    {
        check_gl_error();
    }

    int width, height, format;
    std::string formatName;
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    check_gl_error();
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);    // get renderbuffer width
    check_gl_error();
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);  // get renderbuffer height
    check_gl_error();
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format); // get renderbuffer internal format
    check_gl_error();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    check_gl_error();

    formatName = formatToString(format);

    std::stringstream ss;
    ss << "Renderbuffer - " << width << "x" << height << ", " << formatName;
    return ss.str();
}
