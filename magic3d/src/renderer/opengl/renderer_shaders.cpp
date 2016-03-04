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
#include <magic3d/magic3d.h>
#if defined(__WIN32__)
#include <windows.h>
#endif
#include <magic3d/renderer/opengl/renderer_shaders.h>
#include <math.h>
#include <magic3d/physics/physics_geometry.h>

Magic3D::GLSLShader::GLSLShader(std::string file, unsigned int id) : Shader(file, id)
{
    for (int i = 0; i < eUNIFORM_COUNT; i++)
    {
        uniforms[i] = -1;
    }

    for (int l = 0; l < M3D_SHADER_MAX_LIGHTS; l++)
    {
        ShaderLightUniform light;
        light.position    = -1;
        light.direction   = -1;
        light.ambient     = -1;
        light.diffuse     = -1;
        light.specular    = -1;
        light.attenuation = -1;
        light.cutoff      = -1;
        uniforms_light.push_back(light);
    }

    lightShadow.position    = -1;
    lightShadow.direction   = -1;
    lightShadow.ambient     = -1;
    lightShadow.diffuse     = -1;
    lightShadow.specular    = -1;
    lightShadow.attenuation = -1;
    lightShadow.cutoff      = -1;

    lights = 0;
}

Magic3D::GLSLShader::~GLSLShader()
{
    uniforms_light.clear();
}

//******************************************************************************

Magic3D::RendererOpenGL_Shaders::RendererOpenGL_Shaders() : Renderer_OpenGL()
{
    shaderDefault = NULL;
    shaderGizmo = NULL;
    shaderBlur = NULL;
    materialScreen = NULL;
    materialDefault = NULL;

    lastShader = NULL;
    lastLightsCount = 0;

    needUpdateTextureShader = true;
}

Magic3D::RendererOpenGL_Shaders::~RendererOpenGL_Shaders()
{
    shaderDefault = NULL;
    shaderGizmo = NULL;
    shaderBlur = NULL;
    materialScreen = NULL;
    materialDefault = NULL;

    lastShader = NULL;
}

void Magic3D::RendererOpenGL_Shaders::initialize()
{
    Renderer_OpenGL::initialize();

    glClearStencil(0);
    check_gl_error();

#if defined(MAGIC3D_OES2)
    glClearDepthf(1.0f);
    check_gl_error();
#else
    glClearDepth(1.0f);
    check_gl_error();
#endif
    glEnable(GL_BLEND);
    check_gl_error();
    glEnable(GL_DEPTH_TEST);
    check_gl_error();
    glDepthFunc(GL_LEQUAL);
    check_gl_error();
    glDepthMask(GL_TRUE);
    check_gl_error();

    glEnable(GL_CULL_FACE);
    check_gl_error();
    glCullFace(GL_BACK);
    check_gl_error();
    glFrontFace(GL_CCW);
    check_gl_error();

#if !defined(MAGIC3D_OES2)
#if defined(__WIN32__)
    glEnable(GL_PROGRAM_POINT_SIZE);
    check_gl_error();
#else
    glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    check_gl_error();
#endif
#endif

    setDefaultShaderVars();

    if (fbo_shadows)
    {
        glActiveTexture(GL_TEXTURE2);
        check_gl_error();
        if (isSoftShadows())
        {
            glBindTexture(GL_TEXTURE_2D, fbo_shadows->getColorTexture());
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, fbo_shadows->getDepthTexture());
        }
        check_gl_error();
    }

    if (fbo_reflections)
    {
        glActiveTexture(GL_TEXTURE3);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, fbo_reflections->getColorTexture());
        check_gl_error();
    }

    if (fbo_screen)
    {
        glActiveTexture(GL_TEXTURE4);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, fbo_screen->getDepthTexture());
        check_gl_error();
    }

    Log::log(eLOG_SUCCESS, "OpenGL renderer sucessfully started.");
}

bool Magic3D::RendererOpenGL_Shaders::start()
{
    instance = new RendererOpenGL_Shaders();

    return instance;
}

bool Magic3D::RendererOpenGL_Shaders::render(Scene* scene)
{
    this->scene = scene;

    matrix_view = Matrix4::identity();
    matrix_camera = Matrix4::identity();
    water = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    matrix_shadows = Matrix4::identity();

    rendererObjects    = 0;
    rendererTriangles  = 0;
    rendererDrawCalls  = 0;
    rendererTexChanges = 0;
    rendererReflective = 0;
    rendererShadows    = 0;

    shadowsMS     = 0.0f;
    reflectionsMS = 0.0f;
    defaultMS     = 0.0f;
    fboMS         = 0.0f;
    screenMS      = 0.0f;
    spritesMS     = 0.0f;
    gizmosMS      = 0.0f;
    
    //glLineWidth(screenEffects ? 3.0f : 1.0f);
    //check_gl_error();

#if defined(MAGIC3D_OES)
    const GLenum discards[]  = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
#endif

    renderShadows(scene);

    int vpSize = getViewPorts()->size();
    for (int i = 0; i < vpSize; i++)
    {        
        currentViewPort = i;
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        check_gl_error();

        ViewPort* view = getViewPort(i);
        Camera* camera = view->getPerspective();

        if (scene->getFBOCamera() && fbo && !view->getParent())
        {
            fboViewPort->setPerspective(scene->getFBOCamera());
            renderFBO(fbo, fboViewPort, scene);
        }

        if (!view->getParent())
        {
            scene->updateFrustumCulling(view->getOrthographic(), view->getPerspective());
        }

        if (camera)
        {
            lastShader = NULL;

            renderFBO(NULL, view, scene);

            Magic3D::getInstance()->setTimeReference();
            if (isRenderingScreenEffects())
            {
                if (materialScreen && materialScreen->getShader())
                {
                    renderGlow(scene, camera, view);
                }
            }
            screenMS = Magic3D::getInstance()->getTimeSinceReference();

            Magic3D::getInstance()->setTimeReference();
            if (isShowingGizmos())
            {
                glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
                check_gl_error();

                STEREOSCOPY eye = getEye(view);
                ViewPort* currentView = view;

                if (isRenderingScreenEffects())
                {
                    currentView = screenViewPort;
                    screenViewPort->setArea(view->getArea());
                    screenViewPort->setPerspective(camera);
                    view3D(screenViewPort, eye);
                }
                else
                {
                    view3D(view, eye);
                }

                std::vector<Object*>::const_iterator it_s = selectedObjects.begin();
                while (it_s != selectedObjects.end())
                {
                    Object* obj = *it_s++;
                    if (obj->isShowingGrid())
                    {
                        renderGrid(camera, obj, currentView);
                    }
                    renderBoundingBox(camera, obj);
                    if (obj->getType() == eOBJECT_MODEL)
                    {
                        renderSkeleton(camera, obj);
                    }
                }

                renderLightsGizmos(camera, currentView);
                renderSoundsGizmos(camera, currentView);
                if (isShowingGizmosPhysics())
                {
                    Physics::getInstance()->render();
                }
                renderDebug(camera, currentView);

                selectedObjects.clear();
                screenViewPort->setArea(0.0f, 0.0f, 1.0f, 1.0f);
            }
            gizmosMS = Magic3D::getInstance()->getTimeSinceReference();
        }

        camera = view->getOrthographic();
        if (camera)
        {
            STEREOSCOPY eye = getEye(view);
            ViewPort* currentView = view;

            lastShader = NULL;
            Magic3D::getInstance()->setTimeReference();

            glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
            check_gl_error();

            if (isRenderingScreenEffects())
            {
                currentView = screenViewPort;
                screenViewPort->setArea(view->getArea());
                screenViewPort->setOrthographic(camera);
                view2D(screenViewPort);
            }
            else
            {
                view2D(view);
            }

            matrix_view = camera->getView(eye);

            if (!view->getPerspective() /*&& !view->getParent()*/)
            {
                ColorRGBA color = scene->getColorClear();
                glClearColor(color.r, color.g, color.b, color.a);
                check_gl_error();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                check_gl_error();
            }

            std::list<RenderObject>* obj2D = scene->getVisibleObjects2D();

            glDisable(GL_DEPTH_TEST);
            check_gl_error();
            glDepthMask(GL_FALSE);
            check_gl_error();

            renderObjects(camera, obj2D, scene, true, true, true);

            glEnable(GL_DEPTH_TEST);
            check_gl_error();
            glDepthMask(GL_TRUE);
            check_gl_error();

            spritesMS = Magic3D::getInstance()->getTimeSinceReference();

            Magic3D::getInstance()->setTimeReference();
            if (isShowingGizmos())
            {
                std::vector<Object*>::const_iterator it_s = selectedObjects.begin();
                while (it_s != selectedObjects.end())
                {
                    Object* obj = *it_s++;
                    if (obj->isShowingGrid())
                    {
                        renderGrid(camera, obj, currentView);
                    }
                    renderBoundingBox(camera, obj);
                }

                renderDebug(camera, currentView);

                selectedObjects.clear();
            }
            gizmosMS += Magic3D::getInstance()->getTimeSinceReference();

            screenViewPort->setArea(0.0f, 0.0f, 1.0f, 1.0f);
        }

#if defined(MAGIC3D_OES)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        check_gl_error();
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 3, discards);
        check_gl_error();

        if (fbo)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo->getID());
            check_gl_error();
            glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo->getAttachmentsCount(), discards);
            check_gl_error();
        }
        if (fbo_blur)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_blur->getID());
            check_gl_error();
            glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo_blur->getAttachmentsCount(), discards);
            check_gl_error();
        }
#endif
    }

    if (isRenderingScreenEffects())
    {
        renderScreen(getViewPort(currentViewPort)->getPerspective());
    }

#if defined(MAGIC3D_OES)
    if (fbo_shadows)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadows->getID());
        check_gl_error();
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo_shadows->getAttachmentsCount(), discards);
        check_gl_error();
    }

    if (fbo_reflections)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflections->getID());
        check_gl_error();
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo_reflections->getAttachmentsCount(), discards);
        check_gl_error();
    }

    if (fbo_glow)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_glow->getID());
        check_gl_error();
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo_glow->getAttachmentsCount(), discards);
        check_gl_error();
    }

    if (fbo_screen)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_screen->getID());
        check_gl_error();
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, fbo_screen->getAttachmentsCount(), discards);
        check_gl_error();
    }
#endif

    selectedObjects.clear();
    debugLines[0]->clear();
    debugPoints[0]->clear();
    debugLines[1]->clear();
    debugPoints[1]->clear();

    distortions.clear();

    currentViewPort = 0;
    if (window)
    {
        window->render();
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderFBO(OpenGL_FBO* fbo, ViewPort* view, Scene* scene)
{
    Magic3D::getInstance()->setTimeReference();

    Camera* camera = view ? view->getPerspective() : NULL;

    if (camera)
    {
        STEREOSCOPY eye = getEye(view);

        toTexture = fbo != NULL || isRenderingScreenEffects();

        if (fbo)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo->getID());
            check_gl_error();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
            check_gl_error();
        }        

        if (fbo)
        {
            fboViewPort->setWidth(fbo->getWidth());
            fboViewPort->setHeight(fbo->getHeight());
            fboViewPort->setPerspective(camera);
            view3D(fboViewPort);
        }
        else
        {            
            if (isRenderingScreenEffects())
            {
                screenViewPort->setArea(view->getArea());
                screenViewPort->setPerspective(camera);
                view3D(screenViewPort, eye);
            }
            else
            {
                view3D(view, eye);
            }
        }

        ColorRGBA color = scene->getColorClear();
        glClearColor(color.r, color.g, color.b, color.a);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        matrix_view = camera->getView(eye);

        scene->getVisibleLights(camera);

        if (fbo)
        {
            scene->updateVisibleObjectsOctree(camera);
        }

        renderObjects(camera, scene->getVisibleObjects3D(), scene, true, true, true);

        if (fbo)
        {
            fboMS = Magic3D::getInstance()->getTimeSinceReference();
        }
        else
        {
            defaultMS = Magic3D::getInstance()->getTimeSinceReference();
        }

        renderReflections(scene, camera, fbo, view);

        Magic3D::getInstance()->setTimeReference();

        renderObjects(camera, scene->getZOrder(), scene, true, true, true);

        if (fbo)
        {
            fboMS = Magic3D::getInstance()->getTimeSinceReference();
        }
        else
        {
            defaultMS = Magic3D::getInstance()->getTimeSinceReference();
        }

        fboViewPort->setArea(0.0f, 0.0f, 1.0f, 1.0f);
        toTexture = false;

        /*if (fbo)
        {
            blurTexture(fbo, 3);
        }*/
    }

    return true;
}

void Magic3D::RendererOpenGL_Shaders::renderScreen(Camera* camera)
{
    if (materialScreen && materialScreen->getShader())
    {
        GLSLShader* glsl = static_cast<GLSLShader*>(materialScreen->getShader());
        OpenGL_FBO* fboOGL = static_cast<OpenGL_FBO*>(fbo_screen);
        OpenGL_FBO* fboGlow = static_cast<OpenGL_FBO*>(fbo_glow);

        glDisable(GL_DEPTH_TEST);
        check_gl_error();

        glDisable(GL_BLEND);
        check_gl_error();

        float wSize = (getWindow()->getWidth() > getWindow()->getHeight() ? getWindow()->getWidth() : getWindow()->getHeight());
        fboViewPort->setWidth(wSize);
        fboViewPort->setHeight(wSize);
        fboViewPort->setOrthographic(NULL);
        view2D(fboViewPort);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        check_gl_error();

        glClearColor(1.0, 1.0, 1.0, 1.0);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        glUseProgram(glsl->id);
        check_gl_error();

        setUniform1i(glsl->uniforms[eUNIFORM_RENDER_MODE], getRenderMode());
        check_gl_error();

        if (distortions.size() > 0)
        {
            setUniform4fv(glsl->uniforms[eUNIFORM_DISTORTION], distortions.size(), reinterpret_cast<float*>(&distortions.front()));
            check_gl_error();
        }
        setUniform1i(glsl->uniforms[eUNIFORM_DISTORTIONS],  distortions.size() / 2);
        check_gl_error();

        if (camera)
        {
            setUniform1f(glsl->uniforms[eUNIFORM_ZNEAR], camera->getNear());
            check_gl_error();
            setUniform1f(glsl->uniforms[eUNIFORM_ZFAR], camera->getFar());
            check_gl_error();
            setUniform4f(glsl->uniforms[eUNIFORM_BARREL_DISTORTION], camera->getDistortionX(), camera->getDistortionY(), camera->getDistortion(), camera->getDistortionCube());
            check_gl_error();
        }

        setUniform4f(glsl->uniforms[eUNIFORM_WINDOW], getWindow()->getWidth(), getWindow()->getHeight(), screenViewPort->getArea().getZ(), screenViewPort->getArea().getW());
        check_gl_error();
        setUniform1f(glsl->uniforms[eUNIFORM_TIME], Magic3D::getInstance()->getTime());
        check_gl_error();
        setUniform1f(glsl->uniforms[eUNIFORM_TIME_SIN], Magic3D::getInstance()->getTimeSin());
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_RANDOM], rand() % 100);
        check_gl_error();        

        setUniform4fv(glsl->uniforms[eUNIFORM_SCREEN_ASPECT], 1, reinterpret_cast<float*>(&screenAspect));
        check_gl_error();

        glActiveTexture(GL_TEXTURE0);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, fboOGL->getColorTexture());
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_0], 0);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_0], fboOGL->getColorTexture());
        check_gl_error();
        setUniform2f(glsl->uniforms[eUNIFORM_SIZE_TEXTURE_0], fboOGL->getWidth(), fboOGL->getHeight());
        check_gl_error();

        glActiveTexture(GL_TEXTURE1);
        check_gl_error();
        if (hasGlow())
        {
            glBindTexture(GL_TEXTURE_2D, fboGlow->getColorTexture());
            check_gl_error();
            setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_1], 1);
            check_gl_error();
            setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_1], fboGlow->getColorTexture());
            check_gl_error();
            setUniform2f(glsl->uniforms[eUNIFORM_SIZE_TEXTURE_1], fboGlow->getWidth(), fboGlow->getHeight());
            check_gl_error();
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            check_gl_error();
            setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_1], 1);
            check_gl_error();
            setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_1], 0);
            check_gl_error();
            setUniform2f(glsl->uniforms[eUNIFORM_SIZE_TEXTURE_1], 0, 0);
            check_gl_error();
        }

        setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_DEPTH], 4);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_DEPTH], 1/*scene && scene->getGlows()->size() > 0 ? 1 : 0*/);
        check_gl_error();

        prepareMaterial(glsl, materialScreen, NULL);

        if (extVBO)
        {
            glBindVertexArray(renderIDScreen.id);
            check_gl_error();

            int size = renderIDScreen.indexSize / sizeof(vindex);

            if (getRenderMode() == eRENDER_MODE_SCREEN_WIREFRAME)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                check_gl_error();
                glDrawElements(GL_LINES, posteffectsLines.size() * 2, GL_UINT, &posteffectsLines.front());
                check_gl_error();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderIDScreen.index);
                check_gl_error();
            }
            else
            {
                glDrawElements(GL_TRIANGLE_STRIP, size, GL_UINT, 0);
                check_gl_error();
            }

            rendererTriangles += renderIDScreen.indexSize / sizeof(vindex) - 2;

            glBindVertexArray(0);
            check_gl_error();
        }
        else
        {
            glEnableVertexAttribArray(eVERTEX_POSITION);
            check_gl_error();
            glEnableVertexAttribArray(eVERTEX_UV0);
            check_gl_error();

            glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(PlaneVertex), &posteffectsVertices.front().vertex);
            check_gl_error();

            glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, sizeof(PlaneVertex), &posteffectsVertices.front().uv);
            check_gl_error();

            int size = posteffectsTriangles.size();

            // Draw the quad
            if (getRenderMode() == eRENDER_MODE_SCREEN_WIREFRAME)
            {
                glDrawElements(GL_LINES, posteffectsLines.size() * 2, GL_UINT, &posteffectsLines.front());
                check_gl_error();
            }
            else
            {
                glDrawElements(GL_TRIANGLE_STRIP, size, GL_UINT, &posteffectsTriangles.front());
                check_gl_error();
            }

            rendererTriangles += posteffectsTriangles.size() - 2;

            glDisableVertexAttribArray(eVERTEX_POSITION);
            check_gl_error();
            glDisableVertexAttribArray(eVERTEX_UV0);
            check_gl_error();
        }

        rendererDrawCalls++;

        glEnable(GL_DEPTH_TEST);
        check_gl_error();

        glEnable(GL_BLEND);
        check_gl_error();

        if (lastTexture0)
        {
            glActiveTexture(GL_TEXTURE0);
            check_gl_error();
            glBindTexture(GL_TEXTURE_2D, lastTexture0->id);
            check_gl_error();
        }

        if (lastTexture1)
        {
            glActiveTexture(GL_TEXTURE1);
            check_gl_error();
            glBindTexture(GL_TEXTURE_2D, lastTexture1->id);
            check_gl_error();
        }

        distortions.clear();
    }
}

bool Magic3D::RendererOpenGL_Shaders::renderShadows(Scene* scene)
{
    bool result = false;
    if (!hasShadows())
    {
        return result;
    }

    Magic3D::getInstance()->setTimeReference();
    Light* light = scene->getShadowsLight();

    if (fbo_shadows && light && light->isInFrustum())
    {
        result = true;
        lastShader = NULL;
        toTexture = true;
        shadows = true;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadows->getID());
        check_gl_error();

        fboViewPort->setWidth(fbo_shadows->getWidth());
        fboViewPort->setHeight(fbo_shadows->getHeight());
        fboViewPort->setPerspective(light);
        fboViewPort->setOrthographic(light);
        if (light->getLightType() == eLIGHT_TYPE_DIRECTIONAL)
        {
            view2D(fboViewPort);
        }
        else
        {
            view3D(fboViewPort);
        }

        glClearColor(1.0, 1.0, 1.0, 1.0);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        scene->updateVisibleObjectsOctree(light);
        std::list<RenderObject>* shadowObjects = scene->getShadows();

        matrix_view = light->getView();
        matrix_shadows = matrix_projection * light->getView();

        glDisable(GL_BLEND);
        check_gl_error();

        glCullFace(GL_FRONT);
        check_gl_error();

        if (!isSoftShadows())
        {
            glColorMask(false, false, false, false);
        }
        renderObjects(light, shadowObjects, scene, true, true, false);
        glColorMask(true, true, true, true);

        glCullFace(GL_BACK);
        check_gl_error();

        glEnable(GL_BLEND);
        check_gl_error();

        shadows = false;
        toTexture = false;
        glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
        check_gl_error();

        if (isSoftShadows())
        {
            blurTexture(fbo_shadows, 2, 0.0f, false);
        }
    }
    shadowsMS = Magic3D::getInstance()->getTimeSinceReference();

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::renderReflections(Scene* scene, Camera *camera, FBO* fbo, ViewPort* view)
{
    bool result = false;
    Magic3D::getInstance()->setTimeReference();

    std::list<RenderObject>* reflectives = scene ? scene->getReflectives() : NULL;
    if (!hasReflections())
    {
        if (reflectives)
        {
            renderObjects(camera, reflectives, scene, true, true, true);
        }
        defaultMS += Magic3D::getInstance()->getTimeSinceReference();
        return result;
    }

    if (reflectives && camera && fbo_reflections && reflectives->size() > 0)
    {        
        result = true;
        lastShader = NULL;
        ColorRGBA color = scene->getColorClear();
        float aspect = 0.0f;

        if (toTexture)
        {
            aspect = (float)fboViewPort->getHeight() / (float)fboViewPort->getWidth();
        }
        else
        {
            if (getWindow()->getHeight() < getWindow()->getWidth())
            {
                aspect = (float)getWindow()->getHeight() / (float)getWindow()->getWidth();
            }
            else
            {
                aspect = (float)getWindow()->getWidth() / (float)getWindow()->getHeight();
            }
        }

        std::list<RenderObject>::const_iterator it_o = reflectives->begin();
        while (it_o != reflectives->end())
        {
            const RenderObject* object = &(*it_o++);

            water = object->model * Vector4(0.0f, 1.0f, 0.0f, 0.0);
            water.setW(object->model.getTranslation().getY());

            reflection = true;
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflections->getID());
            check_gl_error();

            if (isRenderingScreenEffects())
            {
                fboViewPort->setWidth(fbo_reflections->getWidth() * screenAspect.getX());
                fboViewPort->setHeight(fbo_reflections->getHeight() * screenAspect.getY());
            }
            else
            {
                if (getWindow()->getHeight() < getWindow()->getWidth())
                {
                    fboViewPort->setWidth(fbo_reflections->getWidth());
                    fboViewPort->setHeight(fbo_reflections->getHeight() * aspect);
                }
                else
                {
                    fboViewPort->setWidth(fbo_reflections->getWidth() * aspect);
                    fboViewPort->setHeight(fbo_reflections->getHeight());
                }
            }
            fboViewPort->setPerspective(camera);
            view3D(fboViewPort);

            glClearColor(color.r, color.g, color.b, color.a);
            check_gl_error();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            check_gl_error();

            STEREOSCOPY eye = getEye(view);

            Matrix4 oldProjection = camera->getProjection(eye);
            camera->reflection(water, eye);
            matrix_view = camera->getView(eye);
            matrix_projection = camera->getProjection(eye);
            scene->updateVisibleObjectsOctree(camera, false, true);

            glFrontFace(GL_CW);
            check_gl_error();

            renderObjects(camera, scene->getReflections(), scene, true, true, true);
            renderObjects(camera, scene->getReflectionsZOrder(), scene, true, true, true);

            glFrontFace(GL_CCW);
            check_gl_error();
            reflection = false;

            camera->updateView();
            matrix_view = camera->getView(eye);
            matrix_projection = oldProjection;
            lastShader = NULL;

#if !defined(MAGIC3D_OES)
            blurTexture(fbo_reflections, 2, 0.0f, true);
#endif

            //water = Vector4(0.0f, 0.0f, 0.0f, 0.0);
            if (fbo)
            {
                OpenGL_FBO* fboOGL = static_cast<OpenGL_FBO*>(fbo);
                glBindFramebuffer(GL_FRAMEBUFFER, fboOGL->getID());
                check_gl_error();
                fboViewPort->setWidth(fboOGL->getWidth());
                fboViewPort->setHeight(fboOGL->getHeight());
                fboViewPort->setPerspective(camera);
                view3D(fboViewPort);
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
                check_gl_error();
                if (isRenderingScreenEffects())
                {
                    fboViewPort->setArea(view->getArea());
                    fboViewPort->setWidth(screenViewPort->getWidth());
                    fboViewPort->setHeight(screenViewPort->getHeight());
                    fboViewPort->setPerspective(camera);
                    view3D(fboViewPort);
                }
                else
                {
                    view3D(view);
                }
            }

            matrix_view = camera->getView(eye);
            matrix_projection = oldProjection;
            renderObject(camera, object, true, true, true);

            fboViewPort->setArea(0.0f, 0.0f, 1.0f, 1.0f);
        }
    }

#if defined(_MGE_)
    scene->updateVisibleObjectsOctree(camera);
#endif

    reflectionsMS += Magic3D::getInstance()->getTimeSinceReference();

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::renderGlow(Scene* scene, Camera *camera, ViewPort* view)
{
    bool result = false;
    if (!hasGlow())
    {
        return result;
    }

    std::list<RenderObject>* glows = scene ? scene->getGlows() : NULL;
    if (glows && camera && fbo_glow /*&& glows->size() > 0*/)
    {
        result = true;
        lastShader = NULL;
        toTexture = true;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_glow->getID());
        check_gl_error();        

        /*glDisable(GL_BLEND);
        check_gl_error();*/

        fboViewPort->setArea(view->getArea());
        fboViewPort->setWidth(fbo_glow->getWidth());
        fboViewPort->setHeight(fbo_glow->getHeight());
        fboViewPort->setPerspective(camera);
        view3D(fboViewPort);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        STEREOSCOPY eye = getEye(view);

        fboViewPort->setWidth(fbo_glow->getWidth() * screenAspect.getX());
        fboViewPort->setHeight(fbo_glow->getHeight() * screenAspect.getY());
        view3D(fboViewPort, eye);
        matrix_view = camera->getView(eye);

        glow = true;
        std::list<RenderObject>::const_iterator it_o = glows->begin();

        glColorMask(true, true, true, false);
        while (it_o != glows->end())
        {
            const RenderObject* object = &(*it_o++);

            renderObject(camera, object, true, true, false);
        }       

        /*glEnable(GL_BLEND);
        check_gl_error();*/

        glDisable(GL_DEPTH_TEST);
        check_gl_error();
        glDepthMask(GL_FALSE);
        check_gl_error();

        if (view->getOrthographic())
        {
            fboViewPort->setArea(view->getArea());
            fboViewPort->setOrthographic(view->getOrthographic());
            fboViewPort->setWidth(fbo_glow->getWidth() * screenAspect.getX());
            fboViewPort->setHeight(fbo_glow->getHeight() * screenAspect.getY());
            view2D(fboViewPort);
            matrix_view = view->getOrthographic()->getView(eye);

            std::list<RenderObject>* obj2D = scene->getVisibleObjects2D();
            it_o = obj2D->begin();
            while (it_o != obj2D->end())
            {
                const RenderObject* object = &(*it_o++);

                renderObject(view->getOrthographic(), object, true, false, false);
            }
        }

        glEnable(GL_DEPTH_TEST);
        check_gl_error();
        glDepthMask(GL_TRUE);
        check_gl_error();

        glColorMask(true, true, true, true);

        fboViewPort->setArea(0.0f, 0.0f, 1.0f, 1.0f);
        glow = false;

        toTexture = false;
        
        blurTexture(fbo_glow, 5.0, 0.5f, true);
    }

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::prepareLights(Camera* camera)
{
    bool hasLights = false;

    std::list<Object*>* lights = scene->getLights();

    if (lights && lights->size() > 0)
    {
        std::list<Object*>::const_iterator it_l = lights->begin();

        int i = 0;
        while (it_l != lights->end())
        {
            if (i < M3D_SHADER_MAX_LIGHTS)
            {
                Light* light = static_cast<Light*>(*it_l++);

                if (light->isEnabled())
                {
                    hasLights = true;

                    if (camera)
                    {
                        light->updateRenderMatrix(camera);
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    return hasLights;
}

void Magic3D::RendererOpenGL_Shaders::renderLightsGizmos(Camera* camera, ViewPort* view)
{
    std::list<Object*>* lights = scene->getLights();

    if (camera && lights && lights->size() > 0)
    {
        std::list<Object*>::const_iterator it_l = lights->begin();

        int i = 0;
        while (it_l != lights->end())
        {
            if (i < M3D_SHADER_MAX_LIGHTS)
            {
                Light* light = static_cast<Light*>(*it_l++);

                if (light->isEnabled())
                {
                    Box box = light->getBoundingBox();
                    Vector3 scale = Vector3(box.getWidth(), box.getHeight(), box.getDepth());
                    renderGizmo(camera, view, light, eGIZMO_TYPE_SPHERE, light->getColorDiffuse(), scale, false, false, false, false);

                    if (light->isSelected())
                    {
                        if (light->getLightType() == eLIGHT_TYPE_SPOT)
                        {
                            renderGizmo(camera, view, light, eGIZMO_TYPE_CONE, light->getColorDiffuse(), Vector3(box.getWidth(), box.getHeight() * -3.0f, box.getDepth()), false, false, false, false);
                        }
                        ColorRGBA c = light->getColorDiffuse();
                        c.a = 0.025f;
                        renderGizmo(camera, view, light, eGIZMO_TYPE_SPHERE, c, Vector3(light->getFar(), light->getFar(), light->getFar()) * 2.0f, false, false, false, false);
                        light->render();
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
}

void Magic3D::RendererOpenGL_Shaders::renderSoundsGizmos(Camera* camera, ViewPort* view)
{
    std::list<Object*>* sounds = scene->getSounds();

    if (camera && sounds && sounds->size() > 0)
    {
        std::list<Object*>::const_iterator it_s = sounds->begin();

        while (it_s != sounds->end())
        {
            Sound* sound = static_cast<Sound*>(*it_s++);

            Box box = sound->getBoundingBox();
            Vector3 scale = Vector3(box.getWidth(), box.getHeight(), box.getDepth());

            if (sound->isPlaying())
            {
                renderGizmo(camera, view, sound, eGIZMO_TYPE_BOX, ColorRGBA(0.0f, 1.0f, 0.0f, 0.25f), scale, false, false, false, false);
            }
            else
            {
                renderGizmo(camera, view, sound, eGIZMO_TYPE_BOX, ColorRGBA(1.0f, 0.0f, 0.0f, 0.25f), scale, false, false, false, false);
            }
        }
    }
}

bool Magic3D::RendererOpenGL_Shaders::renderLights(GLSLShader* shader, const RenderObject* object)
{
    if (object)
    {

    }
    lastLightsCount = 0;
    bool hasLights = false;
    std::list<Object*>* lights = scene->getLights();

    if (shader->lights > 0 && lights && lights->size() > 0)
    {
        std::list<Object*>::const_iterator it_l = lights->begin();
        std::list<Object*>::const_iterator it_end = lights->end();

        while (it_l != it_end)
        {
            if (lastLightsCount < M3D_SHADER_MAX_LIGHTS)
            {
                Light* light = static_cast<Light*>(*it_l++);

                if (light && light->isEnabled())
                {
                    hasLights = true;

                    Vector4 pos = Vector4(light->getRenderMatrix().getTranslation(), (float)light->getLightType());
                    Vector4 dir = Vector4(light->getRenderMatrix().getCol2().getXYZ(), 0.0f);

                    if (reflection)
                    {
                        dir = dir * -1.0f;
                    }

                    lastLightsCount++;
                    int index = lastLightsCount - 1;
                    setUniform4fv(shader->uniforms_light[index].position,  1, reinterpret_cast<float*>(&pos));
                    check_gl_error();
                    setUniform4fv(shader->uniforms_light[index].direction, 1, reinterpret_cast<float*>(&dir));
                    check_gl_error();

                    setUniform4fv(shader->uniforms_light[index].ambient,  1, (float*)&light->getColorAmbient());
                    check_gl_error();
                    setUniform4fv(shader->uniforms_light[index].diffuse,  1, (float*)&light->getColorDiffuse());
                    check_gl_error();
                    setUniform4fv(shader->uniforms_light[index].specular, 1, (float*)&light->getColorSpecular());
                    check_gl_error();

                    setUniform4f(shader->uniforms_light[index].attenuation, light->getAttenuationConstant(), light->getAttenuationLinear(), light->getAttenuationQuadratic(), 0.0f);
                    check_gl_error();
                    setUniform4f(shader->uniforms_light[index].cutoff, light->getExponent(), Math::radians(light->getCutOff()), light->getFar(), light->getNear());
                    check_gl_error();
                }
            }
            else
            {
                lastLightsCount = M3D_SHADER_MAX_LIGHTS;
                break;
            }
        }
    }

    if (scene->getShadowsLight())
    {
        Light* light = scene->getShadowsLight();
        Vector4 pos = Vector4(light->getRenderMatrix().getTranslation(), (float)light->getLightType());
        Vector4 dir = Vector4(light->getRenderMatrix().getCol2().getXYZ(), 0.0f);

        setUniform4fv(shader->lightShadow.position,  1, reinterpret_cast<float*>(&pos));
        check_gl_error();
        setUniform4fv(shader->lightShadow.direction, 1, reinterpret_cast<float*>(&dir));
        check_gl_error();

        setUniform4fv(shader->lightShadow.ambient,  1, (float*)&light->getColorAmbient());
        check_gl_error();
        setUniform4fv(shader->lightShadow.diffuse,  1, (float*)&light->getColorDiffuse());
        check_gl_error();
        setUniform4fv(shader->lightShadow.specular, 1, (float*)&light->getColorSpecular());
        check_gl_error();

        setUniform4f(shader->lightShadow.attenuation, light->getAttenuationConstant(), light->getAttenuationLinear(), light->getAttenuationQuadratic(), 0.0f);
        check_gl_error();
        setUniform4f(shader->lightShadow.cutoff, light->getExponent(), Math::radians(light->getCutOff()), light->getFar(), light->getNear());
        check_gl_error();
    }

    return hasLights;
}

bool Magic3D::RendererOpenGL_Shaders::prepareMatrix(Camera* camera, const RenderObject* object)
{
    matrix_model = object->object->getType() == eOBJECT_LIGHT ? camera->getView() : object->model;
    matrix_modelView = object->object->getType() == eOBJECT_LIGHT ? camera->getView() : object->modelView;

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderObject(Camera* camera, const RenderObject *object, bool textures, bool lights, bool gizmos)
{
    if (gizmos)
    {

    }
    Object* finalObject = object->object->getType() == eOBJECT_INSTANCE ? static_cast<ObjectInstance*>(object->object)->getInstance() : object->object;
    if (object->object->getType() == eOBJECT_INSTANCE && !finalObject)
    {
        finalObject = object->object;
    }
    finalObject->setShadowed(false);
    finalObject->setReflective(false);
    finalObject->setGlowed(false);

    prepareMatrix(camera, object);

    bool isParticles = finalObject->getType() == eOBJECT_PARTICLES || finalObject->getType() == eOBJECT_LIGHT;
    bool depthMaskChange = false;

    if (isParticles && !textures)
    {
        return false;
    }

    if (!shadows && !toTexture && camera->getProjectionType() == ePROJECTION_ORTHOGRAPHIC && finalObject->getType() == eOBJECT_LIGHT)
    {
        Light* light = static_cast<Light*>(finalObject);

        if (light->isShowingLens() || light->isShowingFlare())
        {
            Vector3 pos = getCurrentViewPort()->getPerspective()->getPosition2D(object->model.getTranslation(), getCurrentViewPort());
            GLfloat bufferZ;

            /*int x = 0;
            int y = 0;*/

            if (isRenderingScreenEffects())
            {
                /*x = pos.getX() * fbo_screen->getWidth() * screenAspect.getX();
                float aspectY = screenAspect.getY();
                y = fbo_screen->getHeight() * aspectY - pos.getY() * fbo_screen->getHeight() * aspectY;*/

                bufferZ = pos.getZ();
            }
            else
            {
                int x = pos.getX() * getWindow()->getWidth();
                int y = getWindow()->getHeight() - pos.getY() * getWindow()->getHeight();

                glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &bufferZ);
                check_gl_error();
            }

            if (bufferZ < pos.getZ())
            {
                return false;
            }
        }
    }

    std::vector<Mesh*>* meshes = finalObject->getMeshes();
    std::vector<Mesh*>::const_iterator it_m = meshes->begin();
    while (it_m != meshes->end())
    {
        Mesh* mesh = *it_m++;

        if (mesh->isCastingShadows())
        {
            finalObject->setShadowed(true);
        }

        if (mesh->isReflective())
        {
            finalObject->setReflective(true);
        }

        if (mesh->isGlow())
        {
            finalObject->setGlowed(true);
        }

        if ((shadows && mesh->isCastingShadows()) || (reflection && !mesh->isReflective()))
        {
            finalObject->setInEffectFrustum(true);
        }

        if ((shadows && !mesh->isCastingShadows()) || (reflection && mesh->isReflective()) || (glow && !mesh->isGlow()/* && finalObject->getRender() == eRENDER_3D*/))
        {
            continue;
        }
        
        if (mesh->isVisible() && (getRenderMode() != eRENDER_MODE_WIREFRAME || (mesh->getData() != finalObject->getCollisionMesh()  || !isShowingGizmosPhysics())))
        {
            if (shadows && object->object->getType() == eOBJECT_TERRAIN)
            {
                glCullFace(GL_BACK);
            }
            if (mesh->isDoubleSide())
            {
                glDisable(GL_CULL_FACE);
                check_gl_error();
            }

            if (finalObject->getType() == eOBJECT_LIGHT || finalObject->getType() == eOBJECT_PARTICLES)
            {
                if ((!shadows /*&& !glow*/) || finalObject->getType() == eOBJECT_LIGHT)
                {
                    depthMaskChange = true;
                    glDepthMask(GL_FALSE);
                    check_gl_error();
                }
            }

            Material* material = NULL;
            if (mesh->getMaterials()->size() > 0)
            {
                material = mesh->getMaterials()->at(0);
            }

            if (!material)
            {
                material = materialDefault;
            }

            if (textures)
            {
                renderTextures(finalObject, mesh, material);
            }

            Shader* shader = material->getShader();

            GLSLShader* glsl = static_cast<GLSLShader*>(shader ? shader : shaderDefault);
            if ((/*getRenderMode() == eRENDER_MODE_WIREFRAME ||*/ getRenderMode() == eRENDER_MODE_NORMALS || getRenderMode() == eRENDER_MODE_UVS) && shaderGizmo && !isProfileObject(finalObject))
            {
                glsl = static_cast<GLSLShader*>(shaderGizmo);
            }
            
            if (glsl)
            {
                Skeleton* skeleton = NULL;
                int boneCount  = 0;

                if (finalObject->getType() == eOBJECT_MODEL)
                {
                    skeleton = (static_cast<Model*>(finalObject))->getSkeleton();
                    boneCount = skeleton ? skeleton->getBoneCount() : 0;
                }

                if (glsl != lastShader)
                {
                    glUseProgram(glsl->id);
                    check_gl_error();

                    setUniform1i(glsl->uniforms[eUNIFORM_RENDER_MODE], getRenderMode());
                    check_gl_error();

                    matrix_camera = camera->getMatrixFromParent();
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_PROJECTION], 1, GL_FALSE, reinterpret_cast<float*>(&matrix_projection));
                    check_gl_error();
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_VIEW],       1, GL_FALSE, reinterpret_cast<float*>(&matrix_view));
                    check_gl_error();
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_SHADOWS],    1, GL_FALSE, reinterpret_cast<float*>(&matrix_shadows));
                    check_gl_error();
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_CAMERA],     1, GL_FALSE, reinterpret_cast<float*>(&matrix_camera));
                    check_gl_error();

                    setUniform4fv(glsl->uniforms[eUNIFORM_FOG_COLOR], 1, (float*)&scene->getFogColor());
                    check_gl_error();

                    renderLights(glsl, object);

                    setUniform4fv(glsl->uniforms[eUNIFORM_AMBIENT_COLOR], 1, (float*)&scene->getColorAmbient());
                    check_gl_error();

                    setUniform1f(glsl->uniforms[eUNIFORM_TIME], Magic3D::getInstance()->getTime());
                    check_gl_error();
                    setUniform1f(glsl->uniforms[eUNIFORM_TIME_SIN], Magic3D::getInstance()->getTimeSin());
                    check_gl_error();
                    setUniform1i(glsl->uniforms[eUNIFORM_RANDOM], rand() % 100);
                    check_gl_error();

                    setUniform1f(glsl->uniforms[eUNIFORM_ZNEAR], camera->getNear());
                    check_gl_error();
                    setUniform1f(glsl->uniforms[eUNIFORM_ZFAR], camera->getFar());
                    check_gl_error();

                    setUniform3fv(glsl->uniforms[eUNIFORM_WIND], 1, (float*)&scene->getWind());
                    check_gl_error();

                    setUniform4fv(glsl->uniforms[eUNIFORM_WATER], 1, reinterpret_cast<float*>(&water));
                    check_gl_error();
                }

                if (skeleton)
                {
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_SKIN],  boneCount, GL_FALSE, reinterpret_cast<float*>(&skeleton->skin.front()));
                    check_gl_error();
                }
                setUniform1i(glsl->uniforms[eUNIFORM_SKELETON],  skeleton ? 1 : 0);
                check_gl_error();

                setUniformMatrix4fv(glsl->uniforms[eUNIFORM_MODELVIEW],  1, GL_FALSE, reinterpret_cast<float*>(&matrix_modelView));
                check_gl_error();
                setUniformMatrix4fv(glsl->uniforms[eUNIFORM_MODEL],      1, GL_FALSE, reinterpret_cast<float*>(&matrix_model));
                check_gl_error();

                setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_SHADOWS],  shadows ? -1 : rendererShadows == 0 || finalObject->getRender() == eRENDER_2D || !mesh->isReceivingShadows() ? 0 : 1);
                check_gl_error();
                setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_REFLECTION],  reflection ? -1 : rendererReflective == 0 || finalObject->getRender() == eRENDER_2D ? 0 : 1);
                check_gl_error();
                if (glow)
                {
                    setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_DEPTH], finalObject->getRender() == eRENDER_2D && !mesh->isGlow() ? -2 : -1);
                }
                else
                {
                    setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_DEPTH], finalObject->getRender() == eRENDER_2D ? 0 : 1);
                }
                check_gl_error();

                setUniform1i(glsl->uniforms[eUNIFORM_RENDER_TO_TEXTURE], toTexture ? 1 : 0);
                check_gl_error();

                prepareMaterial(glsl, material, finalObject);

                if (toTexture)
                {
                    setUniform4f(glsl->uniforms[eUNIFORM_WINDOW], fboViewPort->getWidth(), fboViewPort->getHeight(), fboViewPort->getArea().getZ(), fboViewPort->getArea().getW());
                    check_gl_error();
                }
                else
                {
                    ViewPort* view = getCurrentViewPort();
                    setUniform4f(glsl->uniforms[eUNIFORM_WINDOW], getWindow()->getWidth(), getWindow()->getHeight(), view->getArea().getZ(), view->getArea().getW());
                    check_gl_error();
                }

                if (mesh->isFogged() && !shadows)
                {
                    setUniform3f(glsl->uniforms[eUNIFORM_FOG_ATTENUATION],  scene->getFogStart(), scene->getFogFinish(), 0.0f);
                    check_gl_error();
                }
                else
                {
                    setUniform3f(glsl->uniforms[eUNIFORM_FOG_ATTENUATION],  0.0f, 0.0f, 0.0f);
                    check_gl_error();
                }

                setUniform4f(glsl->uniforms[eUNIFORM_BARREL_DISTORTION], camera->getDistortionX(), camera->getDistortionY(), camera->getDistortion(), camera->getDistortionCube());
                check_gl_error();

                setUniform4fv(glsl->uniforms[eUNIFORM_SCREEN_ASPECT], 1, reinterpret_cast<float*>(&screenAspect));
                check_gl_error();

                if (!lights || !mesh->isIlluminated())
                {
                    setUniform1i(glsl->uniforms[eUNIFORM_LIGHTS], 0);
                    check_gl_error();
                }
                else
                {
                    setUniform1i(glsl->uniforms[eUNIFORM_LIGHTS], lastLightsCount);
                    check_gl_error();
                }

                renderMeshData(mesh->getData(), glsl, finalObject);

                lastShader = glsl;
            }

            if (depthMaskChange)
            {                
                glDepthMask(GL_TRUE);
                check_gl_error();
            }

            if (mesh->isDoubleSide())
            {
                glEnable(GL_CULL_FACE);
                check_gl_error();
            }
            if (shadows && object->object->getType() == eOBJECT_TERRAIN)
            {
                glCullFace(GL_FRONT);
            }

            /*glUseProgram(0);
            check_gl_error();*/
        }
    }

#if defined(_MGE_) || defined(DEBUG)
    if (object->object->getType() == eOBJECT_INSTANCE && finalObject)
    {
        finalObject = object->object;
    }
    if (gizmos && (finalObject->isSelected() || getRenderMode() == eRENDER_MODE_WIREFRAME) && !isProfileObject(finalObject))
    {
        selectedObjects.push_back(finalObject);
    }
#endif

    rendererObjects++;

    if (shadows)
    {
        rendererShadows++;
    }

    if (reflection)
    {
        rendererReflective++;
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::prepareMaterial(GLSLShader *shader, Material* material, Object* object)
{
    if (shader && material)
    {
        setUniform4fv(shader->uniforms[eUNIFORM_MATERIAL_AMBIENT],  1, (float*)&material->getColorAmbient());
        check_gl_error();
        if (object && (object->getType() == eOBJECT_GUI_LABEL || object->getType() == eOBJECT_TEXT))
        {            
            TextData* textData = NULL;
            if (object->getType() == eOBJECT_GUI_LABEL)
            {
                textData = static_cast<GUILabel*>(object)->getText();
            }
            else if (object->getType() == eOBJECT_TEXT)
            {
                textData = static_cast<Text*>(object)->getText();
            }
            setUniform4fv(shader->uniforms[eUNIFORM_MATERIAL_DIFFUSE],  1, (float*)&textData->getTextColor());
            check_gl_error();
        }
        else
        {
            setUniform4fv(shader->uniforms[eUNIFORM_MATERIAL_DIFFUSE],  1, (float*)&material->getColorDiffuse());
            check_gl_error();
        }
        setUniform4fv(shader->uniforms[eUNIFORM_MATERIAL_EMISSION], 1, (float*)&material->getColorEmission());
        check_gl_error();
        setUniform4fv(shader->uniforms[eUNIFORM_MATERIAL_SPECULAR], 1, (float*)&material->getColorSpecular());
        check_gl_error();
        setUniform1f(shader->uniforms[eUNIFORM_MATERIAL_SHININESS], material->getShininess());
        check_gl_error();
        setUniform1f(shader->uniforms[eUNIFORM_MATERIAL_ALPHATEST], material->getAlphaTest());
        check_gl_error();

        if (shader != shaderGizmo)
        {
            int texture = 5;
            for (int i = 0; i < material->getVarCount(); i++)
            {
                MaterialVar* variable = material->getVar(i);
                if (variable->getLocation() == -1)
                {
                    continue;
                }

                switch (variable->getType())
                {
                    case eSHADER_VAR_FLOAT:      glUniform1f(variable->getLocation(), static_cast<MaterialVar_Float*>(variable)->getValue(0)); break;
                    case eSHADER_VAR_FLOAT_VEC2: glUniform2f(variable->getLocation(), static_cast<MaterialVar_Float*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(1)); break;
                    case eSHADER_VAR_FLOAT_VEC3: glUniform3f(variable->getLocation(), static_cast<MaterialVar_Float*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(2)); break;
                    case eSHADER_VAR_FLOAT_VEC4: glUniform4f(variable->getLocation(), static_cast<MaterialVar_Float*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(2),
                                                                                      static_cast<MaterialVar_Float*>(variable)->getValue(3)); break;
                    case eSHADER_VAR_INT:        glUniform1i(variable->getLocation(), static_cast<MaterialVar_Integer*>(variable)->getValue(0)); break;
                    case eSHADER_VAR_INT_VEC2:   glUniform2i(variable->getLocation(), static_cast<MaterialVar_Integer*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(1)); break;
                    case eSHADER_VAR_INT_VEC3:   glUniform3i(variable->getLocation(), static_cast<MaterialVar_Integer*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(2)); break;
                    case eSHADER_VAR_INT_VEC4:   glUniform4i(variable->getLocation(), static_cast<MaterialVar_Integer*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(2),
                                                                                      static_cast<MaterialVar_Integer*>(variable)->getValue(3)); break;
                    case eSHADER_VAR_BOOL:       glUniform1i(variable->getLocation(), static_cast<MaterialVar_Boolean*>(variable)->getValue(0)); break;
                    case eSHADER_VAR_BOOL_VEC2:  glUniform2i(variable->getLocation(), static_cast<MaterialVar_Boolean*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(1)); break;
                    case eSHADER_VAR_BOOL_VEC3:  glUniform3i(variable->getLocation(), static_cast<MaterialVar_Boolean*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(2)); break;
                    case eSHADER_VAR_BOOL_VEC4:  glUniform4i(variable->getLocation(), static_cast<MaterialVar_Boolean*>(variable)->getValue(0),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(1),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(2),
                                                                                      static_cast<MaterialVar_Boolean*>(variable)->getValue(3)); break;
                    case eSHADER_VAR_FLOAT_MAT2: glUniformMatrix2fv(variable->getLocation(), 1, GL_FALSE, (float*)&static_cast<MaterialVar_Matrix*>(variable)->getValue2()); break;
                    case eSHADER_VAR_FLOAT_MAT3: glUniformMatrix3fv(variable->getLocation(), 1, GL_FALSE, (float*)&static_cast<MaterialVar_Matrix*>(variable)->getValue3()); break;
                    case eSHADER_VAR_FLOAT_MAT4: glUniformMatrix4fv(variable->getLocation(), 1, GL_FALSE, (float*)&static_cast<MaterialVar_Matrix*>(variable)->getValue4()); break;
                    case eSHADER_VAR_SAMPLER_2D:
                    case eSHADER_VAR_SAMPLER_CUBE:
                    {
                        if (static_cast<MaterialVar_Texture*>(variable)->getValue())
                        {
                            if (texture < 8)
                            {
                                switch (texture)
                                {
                                    case 5: glActiveTexture(GL_TEXTURE5); break;
                                    case 6: glActiveTexture(GL_TEXTURE6); break;
                                    case 7: glActiveTexture(GL_TEXTURE7); break;
                                    default: break;
                                }

                                check_gl_error();
                                glBindTexture(GL_TEXTURE_2D, static_cast<MaterialVar_Texture*>(variable)->getValue()->id);
                                check_gl_error();
                                glUniform1i(variable->getLocation(), texture++);
                                break;
                            }
                        }
                        break;
                    }
                    default: break;
                }
#if defined(DEBUG)
                if (!check_gl_error())
                {
                    Log::log(eLOG_FAILURE, variable->getName().c_str());
                }
#endif
            }
        }
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::prepareTextures(MeshData* data, GLSLShader *shader)
{
    if (data && shader)
    {
        needUpdateTextureShader = false;
        setUniform1i(shader->uniforms[eUNIFORM_TEXTURE_0], 0);
        check_gl_error();
        setUniform1i(shader->uniforms[eUNIFORM_USE_TEXTURE_0], lastTexture0 ? lastTexture0->id : 0);
        check_gl_error();
        setUniform2f(shader->uniforms[eUNIFORM_SIZE_TEXTURE_0], lastTexture0 ? (float)lastTexture0->width : 0.0f, lastTexture0 ? (float)lastTexture0->height : 0.0f);
        check_gl_error();

        setUniform1i(shader->uniforms[eUNIFORM_TEXTURE_1], 1);
        check_gl_error();
        setUniform1i(shader->uniforms[eUNIFORM_USE_TEXTURE_1], lastTexture1 ? lastTexture1->id : 0);
        check_gl_error();
        setUniform2f(shader->uniforms[eUNIFORM_SIZE_TEXTURE_1], lastTexture1 ? (float)lastTexture1->width : 0.0f, lastTexture1 ? (float)lastTexture1->height : 0.0f);
        check_gl_error();

        setUniform1i(shader->uniforms[eUNIFORM_TEXTURE_SHADOWS], 2);
        check_gl_error();

        setUniform1i(shader->uniforms[eUNIFORM_TEXTURE_REFLECTION], 3);
        check_gl_error();

        setUniform1i(shader->uniforms[eUNIFORM_TEXTURE_DEPTH], 4);
        check_gl_error();
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderObjects(Camera* camera, std::list<RenderObject>* objects, Scene *scene, bool textures, bool lights, bool gizmos)
{
    bool result = true;

    if (scene && camera && objects)
    {
        lastTexture0 = NULL;
        lastTexture1 = NULL;

        std::list<RenderObject>::const_iterator it_o = objects->begin();
        while (it_o != objects->end())
        {
            const RenderObject* object = &(*it_o++);

            if (object->object->getType() != eOBJECT_CAMERA)
            {
                renderObject(camera, object, textures, lights, gizmos);
            }
            else
            {
                Camera* cam = static_cast<Camera*>(object->object);
                if (cam->getProjectionType() == ePROJECTION_PERSPECTIVE && cam != camera)
                {
                    cam->render();
                }
            }
        }
    }
    else
    {
        result = false;
    }

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::renderTextures(Object* object, Mesh* mesh, Material* material)
{
    bool noTexture0 = true;
    bool noTexture1 = true;

    if (object && mesh && material)
    {
        Font* font = NULL;

        if (object && (object->getType() == eOBJECT_GUI_LABEL))
        {
            font = static_cast<GUILabel*>(object)->getText()->getFont();
        }
        if (object && (object->getType() == eOBJECT_TEXT))
        {
            font = static_cast<Text*>(object)->getText()->getFont();
        }

        Texture* texture0 = material->getTexture(0);
        Texture* texture1 = material->getTexture(1);
        if (font)
        {
            texture0 = font->getTexture(0);
            texture1 = font->getTexture(1);
        }

        if (material->getBlend() != lastBlend)
        {
            lastBlend = material->getBlend();
            switch (lastBlend)
            {
                //case eBLEND_ALPHA:    glBlendFunc( GL_ZERO, GL_SRC_COLOR ); break;
                //case eBLEND_ADD:      glBlendFunc( GL_ONE, GL_ONE ); break;
                case eBLEND_ALPHA:    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); break;
                case eBLEND_ADD:      glBlendFunc( GL_SRC_ALPHA, GL_ONE ); break;
                case eBLEND_MULTIPLY: glBlendFunc( GL_DST_COLOR, GL_ZERO ); break;
                case eBLEND_DIVIDE:   glBlendFunc( GL_ONE, GL_ONE ); break;
                case eBLEND_INVERT:   glBlendFunc( GL_ONE, GL_ZERO ); break;
                case eBLEND_COLOR:    glBlendFunc( GL_SRC_COLOR, GL_DST_COLOR ); break;
                default: break;
            }
            check_gl_error();
        }

        if (texture0)
        {
            noTexture0 = false;            
            if (!lastTexture0 || texture0->id != lastTexture0->id)
            {
                glActiveTexture(GL_TEXTURE0);
                check_gl_error();
                glBindTexture(GL_TEXTURE_2D, texture0->id);
                check_gl_error();

                //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
                //check_gl_error();
                //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                //check_gl_error();

                rendererTexChanges++;
                lastTexture0 = texture0;
                needUpdateTextureShader = true;
            }
        }

        if (texture1)
        {
            noTexture1 = false;            
            if (!lastTexture1 || texture1->id != lastTexture1->id)
            {
                glActiveTexture(GL_TEXTURE1);
                check_gl_error();
                glBindTexture(GL_TEXTURE_2D, texture1->id);
                check_gl_error();

                //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
                //check_gl_error();
                //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                //check_gl_error();

                rendererTexChanges++;
                lastTexture1 = texture1;
                needUpdateTextureShader = true;
            }
        }
    }

    if (noTexture0)
    {
        lastTexture0 = NULL;
    }

    if (noTexture1)
    {
        lastTexture1 = NULL;
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderMeshData(MeshData* data, GLSLShader* shader, Object* object)
{
    if (!data)
    {
        return false;
    }

    bool skin = data->isSkinned();

    if (needUpdateTextureShader)
    {
        prepareTextures(data, shader);
    }

    if (data->isVBO())
    {
        glBindVertexArray(data->getRenderID().id);
        check_gl_error();

        int size = data->getIndexesCount();

        if (getRenderMode() == eRENDER_MODE_WIREFRAME && !isProfileObject(object))
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            check_gl_error();
            glDrawElements(GL_LINES,   data->getLinesCount() * 2, GL_UINT, &data->getLines()->front());
            check_gl_error();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->getRenderID().index);
            check_gl_error();
        }
        else
        {
            switch (data->getType())
            {
                case eMESH_TRIANGLES:
                {
                    glDrawElements(GL_TRIANGLES,      size, GL_UINT, 0);
                    break;
                }
                case eMESH_TRIANGLES_STRIP:
                {
                    glDrawElements(GL_TRIANGLE_STRIP, size, GL_UINT, 0);
                    break;
                }
            }
#if defined(DEBUG)
            if (!check_gl_error())
            {
                Log::logFormat(eLOG_FAILURE, "object: %s - mesh: %s - size: %d", object->getName().c_str(), data->getName().c_str(), size);
            }
#endif
        }

        glBindVertexArray(0);
        check_gl_error();
    }
    else if (data->getVertices()->size() > 0)
    {
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

        if (skin)
        {
            glEnableVertexAttribArray(eVERTEX_WEIGHTS);
            check_gl_error();
            glEnableVertexAttribArray(eVERTEX_BONES);
            check_gl_error();
        }

        int inc = sizeof(Vertex3D);
        glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().position);
        check_gl_error();
        glVertexAttribPointer(eVERTEX_NORMAL, 3, GL_FLOAT, GL_TRUE, inc, &data->getVertices()->front().normal);
        check_gl_error();
        glVertexAttribPointer(eVERTEX_TANGENT, 3, GL_FLOAT, GL_TRUE, inc, &data->getVertices()->front().tangent);
        check_gl_error();
        glVertexAttribPointer(eVERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().color);
        check_gl_error();
        glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().uv[0]);
        check_gl_error();
        glVertexAttribPointer(eVERTEX_UV1, 2, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().uv[1]);
        check_gl_error();

        if (skin)
        {
            glVertexAttribPointer(eVERTEX_WEIGHTS, 4, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().weights);
            check_gl_error();
            glVertexAttribPointer(eVERTEX_BONES, 4, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().bones);
            check_gl_error();
        }

        int size = data->getIndexesCount();
        if (getRenderMode() == eRENDER_MODE_WIREFRAME && !isProfileObject(object))
        {
            glDrawElements(GL_LINES,   data->getLinesCount() * 2, GL_UINT, &data->getLines()->front());
            check_gl_error();
        }
        else
        {
            switch (data->getType())
            {
                case eMESH_TRIANGLES:
                {
                    glDrawElements(GL_TRIANGLES,      size, GL_UINT, &data->getIndexes()->front());
                    break;
                }
                case eMESH_TRIANGLES_STRIP:
                {
                    glDrawElements(GL_TRIANGLE_STRIP, size, GL_UINT, &data->getIndexes()->front());
                    break;
                }
            }
            check_gl_error();
        }

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

        if (skin)
        {
            glDisableVertexAttribArray(eVERTEX_WEIGHTS);
            check_gl_error();
            glDisableVertexAttribArray(eVERTEX_BONES);
            check_gl_error();
        }
    }

    rendererTriangles += data->getTrianglesCount();
    rendererDrawCalls++;

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::prepareGizmo(Matrix4& modelView, ColorRGBA color, bool vertexColor, bool depth)
{
    if (shaderGizmo)
    {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        check_gl_error();
        lastBlend = eBLEND_ALPHA;

        glDisable(GL_CULL_FACE);
        check_gl_error();

        if (depth)
        {
            glEnable(GL_DEPTH_TEST);
            check_gl_error();
            glDepthMask(GL_TRUE);
            check_gl_error();
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            check_gl_error();
            glDepthMask(GL_FALSE);
            check_gl_error();
        }

        GLSLShader* glsl = static_cast<GLSLShader*>(shaderGizmo);
        glUseProgram(glsl->id);
        check_gl_error();

        setUniform1i(glsl->uniforms[eUNIFORM_RENDER_MODE], 0);
        check_gl_error();

        setUniformMatrix4fv(glsl->uniforms[eUNIFORM_PROJECTION], 1, GL_FALSE, reinterpret_cast<float*>(&matrix_projection));
        check_gl_error();
        setUniformMatrix4fv(glsl->uniforms[eUNIFORM_MODELVIEW], 1, GL_FALSE, reinterpret_cast<float*>(&modelView));
        check_gl_error();

        setUniform4fv(glsl->uniforms[eUNIFORM_MATERIAL_DIFFUSE], 1, reinterpret_cast<float*>(&color));
        check_gl_error();

        setUniform1i(glsl->uniforms[eUNIFORM_IS_VERTEX_COLOR], vertexColor ? 1 : 0);
        check_gl_error();

        setUniform1i(glsl->uniforms[eUNIFORM_SKELETON],  0);
        check_gl_error();
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderGrid(Camera* camera, Object* object, ViewPort* view)
{
    if (!isShowingGizmos())
    {
        return false;
    }

    STEREOSCOPY eye = getEye(view);

    Matrix4 m = camera->getView(eye) * Matrix4(object->getRender() == eRENDER_2D ? Quaternion::rotationX(Math::radians(90.0f)) : Quaternion::identity(), object->getMatrixFromParent().getTranslation());

    Vector3 v = object->getRender() == eRENDER_2D ? getGrid2D() : getGrid3D();
    m = appendScale(m, v);

    /*int count = 11;

     float dimension = count / 2.0f;
     Vector3 v[(count + 1) * (count + 1)];
     Vector4 c[(count + 1) * (count + 1)];
     unsigned int q[count * count * 4];
     int size = sizeof(q) / sizeof(unsigned int);
     int idx = 0;
     for (int z = 0; z < count + 1; z++)
     {
     for (int x = 0; x < count + 1; x++)
     {
     int index = x * count + x + z;
     v[index] = Vector3(x - dimension, 0.0, z - dimension);
     Point3 p(v[index]);
     float dx = fabs((float)x - count / 2.0f) / (count / 2.0f);
     float dy = fabs((float)z - count / 2.0f) / (count / 2.0f);
     float alpha = 1.0f - Math::max(dx, dy);
     if (alpha < 0.9f)
     {
     alpha *= 0.5f;
     }
     c[index] = Vector4(0.0f, 1.0f, 0.0f, alpha);
     if (x < count && z < count)
     {
     q[idx * 4]     = index;
     q[idx * 4 + 1] = index + 1;
     q[idx * 4 + 2] = index + (count + 1) + 1;
     q[idx * 4 + 3] = index + (count + 1);
     idx++;
     }
     }
     }
     glVertexPointer(3, GL_FLOAT, sizeof(Vector3), v);
     check_gl_error();
     glColorPointer(4, GL_FLOAT, sizeof(Vector4), c);
     check_gl_error();
     glDrawElements(GL_QUADS, size, GL_UINT, q);
     check_gl_error();

     for (int i = 0; i < sizeof(v) / sizeof(Vector3); i++)
     {
     Log::log(eLOG_SUCCESS, "%8.6f, %8.6f, %8.6f,\n", v[i].getX(), v[i].getY(), v[i].getZ());
     }

     for (int i = 0; i < sizeof(v) / sizeof(Vector4); i++)
     {
     Log::log(eLOG_SUCCESS,"%8.6f, %8.6f, %8.6f, %8.6f,\n", c[i].getX(), c[i].getY(), c[i].getZ(), c[i].getW());
     }

     int a = 0;
     while (a < sizeof(q) / sizeof(unsigned int))
     {
     Log::log(eLOG_SUCCESS,"%d, %d, %d, %d,\n", q[a], q[a+1], q[a+2], q[a+3]);

     a += 4;
     }*/

    int size = sizeof(gridLines) / sizeof(vindex);

    prepareGizmo(m, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f), true, true);

    glEnableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, gridPositions);
    check_gl_error();

    glEnableVertexAttribArray(eVERTEX_COLOR);
    check_gl_error();
    glVertexAttribPointer(eVERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, 0, gridColors);
    check_gl_error();

    glDrawElements(GL_LINES, size, GL_UINT, gridLines);
    check_gl_error();

    glDisableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glDisableVertexAttribArray(eVERTEX_COLOR);
    check_gl_error();

    if (lastShader)
    {
        glUseProgram(lastShader->id);
        check_gl_error();
    }

    rendererTriangles += size / 4 / 2;
    rendererDrawCalls++;

    glEnable(GL_CULL_FACE);
    check_gl_error();

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderAxis(Camera* camera, Object* object)
{
    if (!isShowingGizmos() || !object->isInFrustum())
    {
        return false;
    }

    Matrix4 m = object->getMatrixFromParent();

    float size = object->getRender() == eRENDER_2D ? 0.1f : 1.0f;
    float alpha = 1.0f;
    bool ortho = camera->getProjectionType() == ePROJECTION_ORTHOGRAPHIC;
    Renderer::addLine(m.getTranslation(), m.getTranslation() + normalize(m.getCol0().getXYZ()) * size, ortho, ColorRGBA(1.0f, 0.0f, 0.0f, alpha));
    Renderer::addLine(m.getTranslation(), m.getTranslation() + normalize(m.getCol1().getXYZ()) * size, ortho, ColorRGBA(0.0f, 1.0f, 0.0f, alpha));
    Renderer::addLine(m.getTranslation(), m.getTranslation() + normalize(m.getCol2().getXYZ()) * size, ortho, ColorRGBA(0.0f, 0.0f, 1.0f, alpha));

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderGizmo(Camera* camera, ViewPort* view, Object* object, GIZMO_TYPE type, ColorRGBA color, Vector3 scale, bool physics, bool vertexColor, bool wire, bool depth)
{
    if (!isShowingGizmos() || !object->isInFrustum())
    {
        return false;
    }

    int size = 0;
    int inc = sizeof(Vertex3D);

    STEREOSCOPY eye = getEye(view);

    Matrix4 r = object->getMatrixFromParent();
    Matrix4 m = camera->getView(eye) * r;

    glEnableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();

    switch (type)
    {
        case eGIZMO_TYPE_MESH:
        {
            MeshData* data = object->getCollisionMesh();
            if (data && data->getVertices()->size() > 0)
            {
                size = data->getIndexesCount() * sizeof(vindex);

                Vector3 diff = (r * Vector4(object->getShapePosition())).getXYZ();
                Matrix4 matrix = camera->getView() * Matrix4(Quaternion::identity(), object->getPositionFromParent() - diff) * r;

                matrix = appendScale(matrix, scale);

                prepareGizmo(matrix, color, vertexColor, depth);

                Skeleton* skeleton = NULL;
                int boneCount  = 0;

                if (object->getType() == eOBJECT_MODEL)
                {
                    skeleton = static_cast<Model*>(object)->getSkeleton();
                    boneCount = skeleton ? skeleton->getBoneCount() : 0;
                }

                GLSLShader* glsl = static_cast<GLSLShader*>(shaderGizmo);
                if (skeleton)
                {
                    setUniformMatrix4fv(glsl->uniforms[eUNIFORM_SKIN],  boneCount, GL_FALSE, reinterpret_cast<float*>(&skeleton->skin.front()));
                    check_gl_error();
                }
                setUniform1i(glsl->uniforms[eUNIFORM_SKELETON],  skeleton ? 1 : 0);
                check_gl_error();

                glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().position);
                check_gl_error();

                bool skin = data->isSkinned();

                if (skin)
                {
                    glEnableVertexAttribArray(eVERTEX_WEIGHTS);
                    check_gl_error();
                    glEnableVertexAttribArray(eVERTEX_BONES);
                    check_gl_error();
                    glVertexAttribPointer(eVERTEX_WEIGHTS, 4, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().weights);
                    check_gl_error();
                    glVertexAttribPointer(eVERTEX_BONES, 4, GL_FLOAT, GL_FALSE, inc, &data->getVertices()->front().bones);
                    check_gl_error();
                }

                if (wire)
                {
                    glDrawElements(GL_LINES, size * 6, GL_UINT, &data->getLines()->front());
                    check_gl_error();
                }
                else
                {
                    glDrawElements(GL_TRIANGLES, size, GL_UINT, &data->getIndexes()->front());
                    check_gl_error();
                }

                if (skin)
                {
                    glDisableVertexAttribArray(eVERTEX_WEIGHTS);
                    check_gl_error();
                    glDisableVertexAttribArray(eVERTEX_BONES);
                    check_gl_error();
                }
            }

            break;
        }
        case eGIZMO_TYPE_CAPSULE:
        {
            Matrix4 matrix;

            if (physics)
            {
                matrix = m * Matrix4(Quaternion::identity(), Vector3(0.0f, object->getShapeSize().getY() * 0.5f, 0.0f));
                matrix = appendScale(matrix, Vector3(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f));
            }
            else
            {
                matrix = m * Matrix4(Quaternion::identity(), Vector3(0.0f, scale.getY() * 0.5f, 0.0f));
                matrix = appendScale(matrix, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, spherePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(sphereLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, sphereLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(sphereTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
                check_gl_error();
            }

            if (physics)
            {
                matrix = appendScale(m, Vector3(object->getRadius() * 2.0f, object->getShapeSize().getY(), object->getRadius() * 2.0f));
            }
            else
            {
                matrix = appendScale(m, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, cylinderPositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(cylinderLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, cylinderLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(cylinderTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, cylinderTriangles);
                check_gl_error();
            }

            if (physics)
            {
                matrix = m * Matrix4(Quaternion::rotationX(Math::radians(180.0f)), Vector3(0.0f, object->getShapeSize().getY() * -0.5f, 0.0f));
                matrix = appendScale(matrix, Vector3(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f));
            }
            else
            {
                matrix = m * Matrix4(Quaternion::rotationX(Math::radians(180.0f)), Vector3(0.0f, scale.getY() * -0.5f, 0.0f));
                matrix = appendScale(matrix, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, spherePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(sphereLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, sphereLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(sphereTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
                check_gl_error();
            }
            break;
        }
        case eGIZMO_TYPE_SPHERE:
        {
            Matrix4 matrix;

            if (physics)
            {
                matrix = appendScale(m, Vector3(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f));
            }
            else
            {
                matrix = appendScale(m, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, spherePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(sphereLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, sphereLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(sphereTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
                check_gl_error();
            }

            matrix = m * Matrix4(Quaternion::rotationX(Math::radians(180.0f)), Vector3(0.0f, 0.0f, 0.0f));
            if (physics)
            {
                matrix = appendScale(matrix, Vector3(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f));
            }
            else
            {
                matrix = appendScale(matrix, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, spherePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(sphereLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, sphereLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(sphereTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
                check_gl_error();
            }
            break;
        }
        case eGIZMO_TYPE_CYLINDER:
        {
            Matrix4 matrix;

            if (physics)
            {
                matrix = appendScale(m, Vector3(object->getShapeSize().getX(), object->getShapeSize().getY(), object->getShapeSize().getZ()));
            }
            else
            {
                matrix = appendScale(m, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, cylinderPositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(cylinderLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, cylinderLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(cylinderTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, cylinderTriangles);
                check_gl_error();
            }
            break;
        }
        case eGIZMO_TYPE_CONE:
        {
            Matrix4 matrix = m;

            if (object->getRender() == eRENDER_2D)
            {
                matrix = m * Matrix4(Quaternion::rotationX(Math::radians(180.0f)), Vector3(0.0f, 0.0f, 0.0f));
            }

            if (object->getType() == eOBJECT_LIGHT)
            {
                matrix = m * Matrix4(Quaternion::rotationX(Math::radians(90.0f)), Vector3(0.0f, 0.0f, -scale.getY() * 0.5f));
            }

            if (physics)
            {
                matrix = appendScale(matrix, Vector3(object->getRadius() * 2.0f, object->getShapeSize().getY(), object->getRadius() * 2.0f));
            }
            else
            {
                matrix = appendScale(matrix, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, conePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(coneLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, coneLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(coneTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, coneTriangles);
                check_gl_error();
            }
            break;
        }
        case eGIZMO_TYPE_BOUNDING_BOX:
        {
            renderBoundingBox(camera, object);
            break;
        }
        default:
        {
            Matrix4 matrix;

            if (physics)
            {
                matrix = appendScale(m, object->getShapeSize());
            }
            else
            {
                matrix = appendScale(m, scale);
            }

            prepareGizmo(matrix, color, vertexColor, depth);

            glVertexAttribPointer  (eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, cubePositions);
            check_gl_error();

            if (wire)
            {
                size = sizeof(cubeLines) / sizeof(vindex);
                glDrawElements(GL_LINES, size, GL_UINT, cubeLines);
                check_gl_error();
            }
            else
            {
                size = sizeof(cubeTriangles) / sizeof(vindex);
                glDrawElements(GL_TRIANGLES, size, GL_UINT, cubeTriangles);
                check_gl_error();
            }
            break;
        }
    }

    if (wire)
    {
        rendererTriangles += size / 6;
    }
    else
    {
        rendererTriangles += size / 3;
    }
    rendererDrawCalls++;

    glDisableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();

    if (lastShader)
    {
        glUseProgram(lastShader->id);
        check_gl_error();
    }

    glEnable(GL_DEPTH_TEST);
    check_gl_error();
    glDepthMask(GL_TRUE);
    check_gl_error();
    glEnable(GL_CULL_FACE);
    check_gl_error();

    if (object && object->isSelected() && type != eGIZMO_TYPE_BOUNDING_BOX)
    {
        renderBoundingBox(camera, object);
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderBoundingBox(Camera* camera, Object* object)
{
    if (!isShowingGizmos() || !object->isInFrustum())
    {
        return false;
    }

    Box box = object->getBoundingBox();

    Matrix4 matrix = object->getMatrixFromParent();

    Vector3 min = box.corners[0];
    Vector3 max = box.corners[1];
    ColorRGBA color = ColorRGBA(0.0f, 0.75f, 0.0f, 1.0f);
    bool ortho = camera->getProjectionType() == ePROJECTION_ORTHOGRAPHIC;

    Vector3 topFrontLeft  = (matrix * Vector4(min.getX(), max.getY(), max.getZ(), 1.0f)).getXYZ();
    Vector3 topFrontRight = (matrix * Vector4(max.getX(), max.getY(), max.getZ(), 1.0f)).getXYZ();
    Vector3 topBackLeft   = (matrix * Vector4(min.getX(), max.getY(), min.getZ(), 1.0f)).getXYZ();
    Vector3 topBackRight  = (matrix * Vector4(max.getX(), max.getY(), min.getZ(), 1.0f)).getXYZ();

    Vector3 bottomFrontLeft  = (matrix * Vector4(min.getX(), min.getY(), max.getZ(), 1.0f)).getXYZ();
    Vector3 bottomFrontRight = (matrix * Vector4(max.getX(), min.getY(), max.getZ(), 1.0f)).getXYZ();
    Vector3 bottomBackLeft   = (matrix * Vector4(min.getX(), min.getY(), min.getZ(), 1.0f)).getXYZ();
    Vector3 bottomBackRight  = (matrix * Vector4(max.getX(), min.getY(), min.getZ(), 1.0f)).getXYZ();

    //horizontal
    Renderer::addLine(topFrontLeft,    topFrontRight,    ortho, color);
    Renderer::addLine(topBackLeft,     topBackRight,     ortho, color);
    Renderer::addLine(bottomFrontLeft, bottomFrontRight, ortho, color);
    Renderer::addLine(bottomBackLeft,  bottomBackRight,  ortho, color);

    //Vertical
    Renderer::addLine(topFrontLeft,  bottomFrontLeft,  ortho, color);
    Renderer::addLine(topFrontRight, bottomFrontRight, ortho, color);
    Renderer::addLine(topBackLeft,   bottomBackLeft,   ortho, color);
    Renderer::addLine(topBackRight,  bottomBackRight,  ortho, color);

    //Z
    Renderer::addLine(topFrontLeft,     topBackLeft,     ortho, color);
    Renderer::addLine(topFrontRight,    topBackRight,    ortho, color);
    Renderer::addLine(bottomFrontLeft,  bottomBackLeft,  ortho, color);
    Renderer::addLine(bottomFrontRight, bottomBackRight, ortho, color);

    renderAxis(camera, object);

    return true;
}


bool Magic3D::RendererOpenGL_Shaders::renderSkeleton(Camera* camera, Object* object)
{
    if (!isShowingGizmos() || !object->isInFrustum())
    {
        return false;
    }

    Skeleton* skeleton = static_cast<Model*>(object)->getSkeleton();
    if (object->getType() != eOBJECT_MODEL || !skeleton)
    {
        return false;
    }

    std::vector<Bone*>* bones = skeleton->getBones();

    std::vector<Bone*>::const_iterator it_b = bones->begin();
    while (it_b != bones->end())
    {
        Bone* b = *it_b++;

        renderBone(camera, object, b);
    }

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderBone(Camera* camera, Object* object, Bone* bone)
{
    if (camera)
    {

    }
    Matrix4 p = object->getMatrixFromParent() * (bone->getParent() ? bone->getParent()->getMatrixFromParent() : bone->getMatrixFromParent());
    Matrix4 m = object->getMatrixFromParent() * bone->getMatrixFromParent();

    ColorRGBA color;
    if (bone->isSelected())
    {
        color = ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if (bone->getParent() && !bone->getParent()->getParent())
    {
        color = ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        color = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
    }

    if (bone->getChilds()->size() == 0)
    {
        Renderer::addPoint(m.getTranslation(), 5.0f, false, color);
    }

    if (bone->getParent() && bone->getParent()->isSelected())
    {
        color = ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if (bone->getParent() && !bone->getParent()->getParent())
    {
        color = ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        color = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
    }

    Renderer::addPoint(p.getTranslation(), 5.0f, false, color);
    Renderer::addLine(p.getTranslation(), m.getTranslation(), false, color);

    return true;
}

bool Magic3D::RendererOpenGL_Shaders::renderDebug(Camera* camera, ViewPort* view)
{
    if (!isShowingGizmos() || !camera)
    {
        return false;
    }

    STEREOSCOPY eye = getEye(view);

    Matrix4 identity = camera->getView(eye);
    prepareGizmo(identity, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f), true, false);

    MeshData* lines;
    MeshData* points;

    if (camera->getProjectionType() == ePROJECTION_ORTHOGRAPHIC)
    {
        lines = debugLines[1];
        points = debugPoints[1];
    }
    else
    {
        lines = debugLines[0];
        points = debugPoints[0];
    }

    int inc = sizeof(Vertex3D);

    glEnableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glEnableVertexAttribArray(eVERTEX_COLOR);
    check_gl_error();
    glEnableVertexAttribArray(eVERTEX_UV0);
    check_gl_error();

    glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, inc, &lines->getVertices()->front().position);
    check_gl_error();
    glVertexAttribPointer(eVERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, inc, &lines->getVertices()->front().color);
    check_gl_error();
    glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, inc, &lines->getVertices()->front().uv[0]);
    check_gl_error();

    glDrawElements(GL_LINES, lines->getLinesCount() * 2, GL_UINT, &lines->getLines()->front());
    check_gl_error();
    rendererDrawCalls++;

    glVertexAttribPointer(eVERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, inc, &points->getVertices()->front().position);
    check_gl_error();
    glVertexAttribPointer(eVERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, inc, &points->getVertices()->front().color);
    check_gl_error();
    glVertexAttribPointer(eVERTEX_UV0, 2, GL_FLOAT, GL_FALSE, inc, &points->getVertices()->front().uv[0]);
    check_gl_error();

    glDrawElements(GL_POINTS, points->getPointsCount(), GL_UINT, &points->getPoints()->front());
    check_gl_error();
    rendererDrawCalls++;

    glDisableVertexAttribArray(eVERTEX_POSITION);
    check_gl_error();
    glDisableVertexAttribArray(eVERTEX_COLOR);
    check_gl_error();
    glDisableVertexAttribArray(eVERTEX_UV0);
    check_gl_error();

    if (lastShader)
    {
        glUseProgram(lastShader->id);
        check_gl_error();
    }

    glEnable(GL_DEPTH_TEST);
    check_gl_error();
    glDepthMask(GL_TRUE);
    check_gl_error();
    glEnable(GL_CULL_FACE);
    check_gl_error();

    lines->clear();
    points->clear();

    return true;
}

void Magic3D::RendererOpenGL_Shaders::setShaderDefault(Shader* shader)
{
    shaderDefault = shader;
}

void Magic3D::RendererOpenGL_Shaders::setShaderGizmo(Shader* shader)
{
    shaderGizmo = shader;
}

void Magic3D::RendererOpenGL_Shaders::setShaderBlur(Shader* shader)
{
    shaderBlur = shader;
}

void Magic3D::RendererOpenGL_Shaders::setMaterialScreen(Material* material)
{
    materialScreen = material;
}

void Magic3D::RendererOpenGL_Shaders::setMaterialDefault(Material* material)
{
    materialDefault = material;
}

Magic3D::Shader* Magic3D::RendererOpenGL_Shaders::createShader(std::string file)
{
    Shader* shader = new GLSLShader(file, 0);
    updateShader(shader);
    return shader;
}

bool Magic3D::RendererOpenGL_Shaders::updateShader(Shader* shader)
{
    if (!shader)
    {
        return false;
    }

    shader->defines = 0;

    bool result = false;
    bool compiled = true;
    GLuint vertShader;
    GLuint fragShader;
    std::string vertDefaultShader;
    std::string fragDefaultShader;
    std::string definesDefaultShader;
    std::string vertShaderPathname;
    std::string fragShaderPathname;

    vertDefaultShader = ResourceManager::getShaders()->getPath(M3D_DEFAULT_SHADER_VERTEX);
    fragDefaultShader = ResourceManager::getShaders()->getPath(M3D_DEFAULT_SHADER_FRAGMENT);
    definesDefaultShader = ResourceManager::getShaders()->getPath(M3D_DEFAULT_SHADER_DEFINES);

    // Create and compile vertex shader.
    vertShaderPathname = ResourceManager::getShaders()->getPath(shader->getName() + M3D_VERTEX_SHADER_FILE);

    std::stringstream te;

    if (Magic3D::getInstance()->getConfiguration().STEREOSCOPY)
    {
        te << "#define STEREOSCOPY\n";
        shader->defines++;
    }

    te << "#define SKINNING\n";
    shader->defines++;

    if (hasShadows())
    {
        te << "#define SHADOWS\n";
        shader->defines++;
    }

    if (isSoftShadows())
    {
        te << "#define SOFT_SHADOWS\n";
        shader->defines++;
    }

    if (hasReflections())
    {
        te << "#define REFLECTIONS\n";
        shader->defines++;
    }

    if (hasGlow())
    {
        te << "#define GLOW\n";
        shader->defines++;
    }

    if (fbo_shadows)
    {
        te << "#define SHADOWS_SIZE vec2(";
        te << fbo_shadows->getWidth();
        te << ", ";
        te << fbo_shadows->getHeight();
        te << ")\n";
        shader->defines++;
    }

    if (fbo_reflections)
    {
        te << "#define REFLECTIONS_SIZE vec2(";
        te << fbo_reflections->getWidth();
        te << ", ";
        te << fbo_reflections->getHeight();
        te << ")\n";
        shader->defines++;
    }

    if (fbo_glow)
    {
        te << "#define GLOW_SIZE vec2(";
        te << fbo_glow->getWidth();
        te << ", ";
        te << fbo_glow->getHeight();
        te << ")\n";
        shader->defines++;
    }

    if (fbo_screen)
    {
        te << "#define DEPTH_SIZE vec2(";
        te << fbo_screen->getWidth();
        te << ", ";
        te << fbo_screen->getHeight();
        te << ")\n";
        shader->defines++;
    }

    std::stringstream shaderVertex;
    if (ResourceManager::getInstance()->getPackage())
    {
        Memory tdd;
        Memory tdv;
        Memory tv;

        ResourceManager::getInstance()->unpack(definesDefaultShader, &tdd);
        ResourceManager::getInstance()->unpack(vertDefaultShader, &tdv);
        ResourceManager::getInstance()->unpack(vertShaderPathname, &tv);

        shaderVertex << te.rdbuf();
        shaderVertex << tdd.getBuffer();
        shaderVertex << tdv.getBuffer();
        shaderVertex << tv.getBuffer();
    }
    else
    {
        std::ifstream tdd(definesDefaultShader.c_str());
        std::ifstream tdv(vertDefaultShader.c_str());
        std::ifstream tv(vertShaderPathname.c_str());

        shaderVertex << te.rdbuf();
        shaderVertex << tdd.rdbuf();
        shaderVertex << tdv.rdbuf();
        shaderVertex << tv.rdbuf();
    }

    if (!compileShader(vertShader, eSHADER_TYPE_VERTEX, shaderVertex.str().c_str())) {
        compiled = false;
        Log::log(eLOG_FAILURE, "Failed to compile vertex shader");
    }

    // Create and compile fragment shader.
    fragShaderPathname = ResourceManager::getShaders()->getPath(shader->getName() + M3D_FRAGMENT_SHADER_FILE);

    te.rdbuf()->pubseekpos(0, te.in);

    std::stringstream shaderFragment;
    if (ResourceManager::getInstance()->getPackage())
    {
        Memory tdd;
        Memory tdv;
        Memory tv;

        ResourceManager::getInstance()->unpack(definesDefaultShader, &tdd);
        ResourceManager::getInstance()->unpack(fragDefaultShader, &tdv);
        ResourceManager::getInstance()->unpack(fragShaderPathname, &tv);

        shaderFragment << te.rdbuf();
        shaderFragment << tdd.getBuffer();
        shaderFragment << tdv.getBuffer();
        shaderFragment << tv.getBuffer();
    }
    else
    {
        std::ifstream tdd(definesDefaultShader.c_str());
        std::ifstream tdf(fragDefaultShader.c_str());
        std::ifstream tf(fragShaderPathname.c_str());

        shaderFragment << te.rdbuf();
        shaderFragment << tdd.rdbuf();
        shaderFragment << tdf.rdbuf();
        shaderFragment << tf.rdbuf();
    }

    if (!compileShader(fragShader, eSHADER_TYPE_FRAGMENT, shaderFragment.str().c_str())) {
        compiled = false;
        Log::log(eLOG_FAILURE, "Failed to compile fragment shader");
    }

    if (compiled)
    {
        // Create shader program.
        shader->id = glCreateProgram();
        check_gl_error();

        // Attach vertex shader to program.
        glAttachShader(shader->id, vertShader);
        check_gl_error();

        // Attach fragment shader to program.
        glAttachShader(shader->id, fragShader);
        check_gl_error();

        // Bind attribute locations.
        // This needs to be done prior to linking.
        glBindAttribLocation(shader->id, eVERTEX_POSITION, "position");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_NORMAL, "normal");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_TANGENT, "tangent");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_COLOR, "color");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_UV0, "uv_0");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_UV1, "uv_1");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_WEIGHTS, "weights");
        check_gl_error();
        glBindAttribLocation(shader->id, eVERTEX_BONES, "bones");
        check_gl_error();


        // Link program.
        if (!linkProgram(shader->id)) {
            Log::logFormat(eLOG_FAILURE, "Failed to link program: %d", shader->id);

            if (vertShader) {
                glDeleteShader(vertShader);
                check_gl_error();
                vertShader = 0;
            }
            if (fragShader) {
                glDeleteShader(fragShader);
                check_gl_error();
                fragShader = 0;
            }
            if (shader->id) {
                glDeleteProgram(shader->id);
                check_gl_error();
                shader->id = 0;
            }
        }
        else
        {
            result = true;
        }
    }

    if (result)
    {
        GLSLShader* glsl = static_cast<GLSLShader*>(shader);
        updateShaderUserVariables(glsl);
        // Get uniform locations.
        glsl->uniforms[eUNIFORM_RENDER_MODE]            = glGetUniformLocation(glsl->id, "renderMode");
        check_gl_error();

        glsl->uniforms[eUNIFORM_PROJECTION]             = glGetUniformLocation(glsl->id, "matrixProjection");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MODELVIEW]              = glGetUniformLocation(glsl->id, "matrixModelView");
        check_gl_error();
        glsl->uniforms[eUNIFORM_VIEW]                   = glGetUniformLocation(glsl->id, "matrixView");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MODEL]                  = glGetUniformLocation(glsl->id, "matrixModel");
        check_gl_error();
        glsl->uniforms[eUNIFORM_SHADOWS]                = glGetUniformLocation(glsl->id, "matrixShadows");
        check_gl_error();
        glsl->uniforms[eUNIFORM_CAMERA]                 = glGetUniformLocation(glsl->id, "matrixCamera");
        check_gl_error();

        glsl->uniforms[eUNIFORM_SKIN]                   = glGetUniformLocation(glsl->id, "skin");
        check_gl_error();
        glsl->uniforms[eUNIFORM_SKELETON]               = glGetUniformLocation(glsl->id, "skeleton");
        check_gl_error();

        glsl->uniforms[eUNIFORM_DISTORTION]             = glGetUniformLocation(glsl->id, "distortion");
        check_gl_error();
        glsl->uniforms[eUNIFORM_DISTORTIONS]            = glGetUniformLocation(glsl->id, "distortions");
        check_gl_error();

        glsl->uniforms[eUNIFORM_TEXTURE_0]              = glGetUniformLocation(glsl->id, "texture_0");
        check_gl_error();
        glsl->uniforms[eUNIFORM_TEXTURE_1]              = glGetUniformLocation(glsl->id, "texture_1");
        check_gl_error();
        glsl->uniforms[eUNIFORM_TEXTURE_SHADOWS]        = glGetUniformLocation(glsl->id, "texture_Shadows");
        check_gl_error();
        glsl->uniforms[eUNIFORM_TEXTURE_REFLECTION]     = glGetUniformLocation(glsl->id, "texture_Reflection");
        check_gl_error();
        glsl->uniforms[eUNIFORM_TEXTURE_DEPTH]          = glGetUniformLocation(glsl->id, "texture_Depth");
        check_gl_error();

        glsl->uniforms[eUNIFORM_ZNEAR]                  = glGetUniformLocation(glsl->id, "zNear");
        check_gl_error();
        glsl->uniforms[eUNIFORM_ZFAR]                   = glGetUniformLocation(glsl->id, "zFar");
        check_gl_error();

        glsl->uniforms[eUNIFORM_WINDOW]                 = glGetUniformLocation(glsl->id, "window");
        check_gl_error();
        glsl->uniforms[eUNIFORM_WATER]                  = glGetUniformLocation(glsl->id, "water");
        check_gl_error();
        glsl->uniforms[eUNIFORM_WIND]                   = glGetUniformLocation(glsl->id, "wind");
        check_gl_error();

        glsl->uniforms[eUNIFORM_USE_TEXTURE_0]          = glGetUniformLocation(glsl->id, "useTexture_0");
        check_gl_error();
        glsl->uniforms[eUNIFORM_SIZE_TEXTURE_0]         = glGetUniformLocation(glsl->id, "sizeTexture_0");
        check_gl_error();
        glsl->uniforms[eUNIFORM_USE_TEXTURE_1]          = glGetUniformLocation(glsl->id, "useTexture_1");
        check_gl_error();
        glsl->uniforms[eUNIFORM_SIZE_TEXTURE_1]         = glGetUniformLocation(glsl->id, "sizeTexture_1");
        check_gl_error();
        glsl->uniforms[eUNIFORM_USE_TEXTURE_SHADOWS]    = glGetUniformLocation(glsl->id, "useTexture_Shadows");
        check_gl_error();
        glsl->uniforms[eUNIFORM_USE_TEXTURE_REFLECTION] = glGetUniformLocation(glsl->id, "useTexture_Reflection");
        check_gl_error();
        glsl->uniforms[eUNIFORM_USE_TEXTURE_DEPTH]      = glGetUniformLocation(glsl->id, "useTexture_Depth");
        check_gl_error();
        glsl->uniforms[eUNIFORM_RENDER_TO_TEXTURE]      = glGetUniformLocation(glsl->id, "renderToTexture");
        check_gl_error();

        glsl->uniforms[eUNIFORM_FOG_COLOR]              = glGetUniformLocation(glsl->id, "fog.color");
        check_gl_error();
        glsl->uniforms[eUNIFORM_FOG_ATTENUATION]        = glGetUniformLocation(glsl->id, "fog.attenuation");
        check_gl_error();

        glsl->uniforms[eUNIFORM_AMBIENT_COLOR]          = glGetUniformLocation(glsl->id, "ambientColor");
        check_gl_error();

        glsl->uniforms[eUNIFORM_MATERIAL_AMBIENT]       = glGetUniformLocation(glsl->id, "material.ambient");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MATERIAL_DIFFUSE]       = glGetUniformLocation(glsl->id, "material.diffuse");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MATERIAL_EMISSION]      = glGetUniformLocation(glsl->id, "material.emission");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MATERIAL_SPECULAR]      = glGetUniformLocation(glsl->id, "material.specular");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MATERIAL_SHININESS]     = glGetUniformLocation(glsl->id, "material.shininess");
        check_gl_error();
        glsl->uniforms[eUNIFORM_MATERIAL_ALPHATEST]     = glGetUniformLocation(glsl->id, "material.alphaTest");
        check_gl_error();

        glsl->uniforms[eUNIFORM_LIGHTS]                 = glGetUniformLocation(glsl->id, "lightCount");
        check_gl_error();

        glsl->uniforms[eUNIFORM_IS_VERTEX_COLOR]        = glGetUniformLocation(glsl->id, "isVertexColor");
        check_gl_error();

        glsl->uniforms[eUNIFORM_TIME]                   = glGetUniformLocation(glsl->id, "time");
        check_gl_error();
        glsl->uniforms[eUNIFORM_TIME_SIN]               = glGetUniformLocation(glsl->id, "timeSin");
        check_gl_error();
        glsl->uniforms[eUNIFORM_RANDOM]                 = glGetUniformLocation(glsl->id, "random");
        check_gl_error();

        glsl->uniforms[eUNIFORM_BLUR_ORIENTATION]       = glGetUniformLocation(glsl->id, "blurOrientation");
        check_gl_error();
        glsl->uniforms[eUNIFORM_BLUR_AMOUNT]            = glGetUniformLocation(glsl->id, "blurAmount");
        check_gl_error();
        glsl->uniforms[eUNIFORM_BLUR_STRENGTH]          = glGetUniformLocation(glsl->id, "blurStrength");
        check_gl_error();
        glsl->uniforms[eUNIFORM_BLUR_GAUSSIAN]          = glGetUniformLocation(glsl->id, "blurGaussian");
        check_gl_error();

        glsl->uniforms[eUNIFORM_BARREL_DISTORTION]      = glGetUniformLocation(glsl->id, "barrelDistortion");
        check_gl_error();
        glsl->uniforms[eUNIFORM_SCREEN_ASPECT]          = glGetUniformLocation(glsl->id, "screenAspect");
        check_gl_error();

        glsl->lights = 0;
        for (int i = 0; i < M3D_SHADER_MAX_LIGHTS; i++)
        {
            std::stringstream uniform;
            uniform << "lights[" << i << "].";

            glsl->uniforms_light[i].position  = glGetUniformLocation(glsl->id, std::string(uniform.str() + "position").c_str());
            check_gl_error();
            glsl->uniforms_light[i].direction = glGetUniformLocation(glsl->id, std::string(uniform.str() + "direction").c_str());
            check_gl_error();
            glsl->uniforms_light[i].ambient   = glGetUniformLocation(glsl->id, std::string(uniform.str() + "ambient").c_str());
            check_gl_error();
            glsl->uniforms_light[i].diffuse   = glGetUniformLocation(glsl->id, std::string(uniform.str() + "diffuse").c_str());
            check_gl_error();
            glsl->uniforms_light[i].specular  = glGetUniformLocation(glsl->id, std::string(uniform.str() + "specular").c_str());
            check_gl_error();

            glsl->uniforms_light[i].attenuation = glGetUniformLocation(glsl->id, std::string(uniform.str() + "attenuation").c_str());
            check_gl_error();
            glsl->uniforms_light[i].cutoff      = glGetUniformLocation(glsl->id, std::string(uniform.str() + "cutoff").c_str());
            check_gl_error();

            if (glsl->uniforms_light[i].position >= 0 || glsl->uniforms_light[i].direction >= 0)
            {
                glsl->lights++;
            }
        }

        glsl->lightShadow.position    = glGetUniformLocation(glsl->id, std::string("lightShadow.position").c_str());
        check_gl_error();
        glsl->lightShadow.direction   = glGetUniformLocation(glsl->id, std::string("lightShadow.direction").c_str());
        check_gl_error();
        glsl->lightShadow.ambient     = glGetUniformLocation(glsl->id, std::string("lightShadow.ambient").c_str());
        check_gl_error();
        glsl->lightShadow.diffuse     = glGetUniformLocation(glsl->id, std::string("lightShadow.diffuse").c_str());
        check_gl_error();
        glsl->lightShadow.specular    = glGetUniformLocation(glsl->id, std::string("lightShadow.specular").c_str());
        check_gl_error();
        glsl->lightShadow.attenuation = glGetUniformLocation(glsl->id, std::string("lightShadow.attenuation").c_str());
        check_gl_error();
        glsl->lightShadow.cutoff      = glGetUniformLocation(glsl->id, std::string("lightShadow.cutoff").c_str());
        check_gl_error();

        // Release vertex and fragment shaders.
        if (vertShader)
        {
            glDetachShader(glsl->id, vertShader);
            check_gl_error();
            glDeleteShader(vertShader);
            check_gl_error();
        }
        if (fragShader)
        {
            glDetachShader(glsl->id, fragShader);
            check_gl_error();
            glDeleteShader(fragShader);
            check_gl_error();
        }
    }

    return result;
}

void Magic3D::RendererOpenGL_Shaders::deleteShader(unsigned int shader)
{
    glDeleteProgram(shader);
    check_gl_error();
}

bool Magic3D::RendererOpenGL_Shaders::compileShader(unsigned int& shader, SHADER_TYPE type, std::string file)
{
    bool result = true;
    GLint status;
    const GLchar *source;

    source = (GLchar*)file.c_str();
    if (!source)
    {
        Log::log(eLOG_FAILURE, "Failed to load vertex shader");
        result = false;
    }

    if (result)
    {
        GLenum shaderType;
        switch (type)
        {
            case eSHADER_TYPE_VERTEX: shaderType = GL_VERTEX_SHADER; break;
            case eSHADER_TYPE_FRAGMENT: shaderType = GL_FRAGMENT_SHADER; break;
        }

        shader = glCreateShader(shaderType);

        check_gl_error();
        glShaderSource(shader, 1, &source, NULL);
        check_gl_error();
        glCompileShader(shader);
        check_gl_error();

        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        check_gl_error();
        if (logLength > 1)
        {
            GLchar *log = static_cast<GLchar*>(malloc(logLength));
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            check_gl_error();
            Log::log(eLOG_FAILURE, "Shader compile log:");
            Log::log(eLOG_FAILURE, log);
            free(log);
        }

        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        check_gl_error();
        if (status == 0)
        {
            glDeleteShader(shader);
            check_gl_error();
            result = false;
        }
    }

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::linkProgram(GLuint prog)
{
    bool result = true;
    GLint status;
    glLinkProgram(prog);
    check_gl_error();

    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    check_gl_error();
    if (logLength > 1)
    {
        GLchar *log = static_cast<GLchar*>(malloc(logLength));
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        check_gl_error();
        Log::log(eLOG_FAILURE, "Program link log:");
        Log::log(eLOG_FAILURE, log);
        free(log);
    }

    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    check_gl_error();
    if (status == 0)
    {
        result = false;
    }

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::validateProgram(GLuint prog)
{
    bool result = true;
    GLint logLength, status;

    glValidateProgram(prog);
    check_gl_error();
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    check_gl_error();
    if (logLength > 0)
    {
        GLchar *log = static_cast<GLchar*>(malloc(logLength));
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        check_gl_error();
        Log::log(eLOG_FAILURE, "Program validate log:");
        Log::log(eLOG_FAILURE, log);
        free(log);
    }

    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    check_gl_error();
    if (status == 0)
    {
        result = false;
    }

    return result;
}

void Magic3D::RendererOpenGL_Shaders::setDefaultShaderVars()
{
    shaderVars.push_back("renderMode");

    shaderVars.push_back("matrixProjection");
    shaderVars.push_back("matrixModelView");
    shaderVars.push_back("matrixView");
    shaderVars.push_back("matrixModel");
    shaderVars.push_back("matrixShadows");
    shaderVars.push_back("matrixCamera");

    shaderVars.push_back("skin[0]");
    shaderVars.push_back("skeleton");

    shaderVars.push_back("distortion[0]");
    shaderVars.push_back("distortions");

    shaderVars.push_back("texture_0");
    shaderVars.push_back("texture_1");
    shaderVars.push_back("texture_Shadows");
    shaderVars.push_back("texture_Reflection");
    shaderVars.push_back("texture_Depth");

    shaderVars.push_back("zNear");
    shaderVars.push_back("zFar");

    shaderVars.push_back("window");
    shaderVars.push_back("water");
    shaderVars.push_back("wind");

    shaderVars.push_back("useTexture_0");
    shaderVars.push_back("useTexture_1");
    shaderVars.push_back("useTexture_Shadows");
    shaderVars.push_back("useTexture_Reflection");
    shaderVars.push_back("useTexture_Depth");
    shaderVars.push_back("sizeTexture_0");
    shaderVars.push_back("sizeTexture_1");
    shaderVars.push_back("renderToTexture");

    shaderVars.push_back("fog.color");
    shaderVars.push_back("fog.attenuation");

    shaderVars.push_back("ambientColor");

    shaderVars.push_back("material.ambient");
    shaderVars.push_back("material.diffuse");
    shaderVars.push_back("material.emission");
    shaderVars.push_back("material.specular");
    shaderVars.push_back("material.shininess");
    shaderVars.push_back("material.alphaTest");

    shaderVars.push_back("lightCount");

    shaderVars.push_back("isVertexColor");
    shaderVars.push_back("pointSize");

    shaderVars.push_back("time");
    shaderVars.push_back("timeSin");
    shaderVars.push_back("random");

    shaderVars.push_back("blurOrientation");
    shaderVars.push_back("blurAmount");
    shaderVars.push_back("blurStrength");
    shaderVars.push_back("blurGaussian");

    shaderVars.push_back("barrelDistortion");
    shaderVars.push_back("screenAspect");

    shaderVars.push_back("lightShadow.type");
    shaderVars.push_back("lightShadow.position");
    shaderVars.push_back("lightShadow.direction");
    shaderVars.push_back("lightShadow.ambient");
    shaderVars.push_back("lightShadow.diffuse");
    shaderVars.push_back("lightShadow.specular");
    shaderVars.push_back("lightShadow.attenuation");
    shaderVars.push_back("lightShadow.cutoff");
}

bool Magic3D::RendererOpenGL_Shaders::isShaderVarDefault(const std::string& variable)
{
    if (variable.compare(0, 7, "lights[") == 0)
    {
        return true;
    }
    std::vector<std::string>::const_iterator iter = this->shaderVars.begin();
    std::vector<std::string>::const_iterator endIter = this->shaderVars.end();

    while (iter != endIter)
    {
        if (variable == *iter)
        {
            return true;
        }
        else
        {
            ++iter;
        }
    }
    return false;
}

std::string Magic3D::RendererOpenGL_Shaders::getUniformTypeName(SHADER_VAR type)
{
    std::string result;
    switch (type)
    {
        case eSHADER_VAR_FLOAT:        result = "float"; break;
        case eSHADER_VAR_FLOAT_VEC2:   result = "vec2"; break;
        case eSHADER_VAR_FLOAT_VEC3:   result = "vec3"; break;
        case eSHADER_VAR_FLOAT_VEC4:   result = "vec4"; break;
        case eSHADER_VAR_INT:          result = "int"; break;
        case eSHADER_VAR_INT_VEC2:     result = "ivec2"; break;
        case eSHADER_VAR_INT_VEC3:     result = "ivec3"; break;
        case eSHADER_VAR_INT_VEC4:     result = "ivec4"; break;
        case eSHADER_VAR_BOOL:         result = "bool"; break;
        case eSHADER_VAR_BOOL_VEC2:    result = "bvec2"; break;
        case eSHADER_VAR_BOOL_VEC3:    result = "bvec3"; break;
        case eSHADER_VAR_BOOL_VEC4:    result = "bvec4"; break;
        case eSHADER_VAR_FLOAT_MAT2:   result = "mat2"; break;
        case eSHADER_VAR_FLOAT_MAT3:   result = "mat3"; break;
        case eSHADER_VAR_FLOAT_MAT4:   result = "mat4"; break;
        case eSHADER_VAR_SAMPLER_2D:   result = "sampler2D"; break;
        case eSHADER_VAR_SAMPLER_CUBE: result = "samplerCube"; break;
        default:                       result = "<none>"; break;
    }

    return result;
}

Magic3D::SHADER_VAR Magic3D::RendererOpenGL_Shaders::getUniformType(GLenum type)
{
    SHADER_VAR result;
    switch (type)
    {
        case GL_FLOAT:        result = eSHADER_VAR_FLOAT;        break;
        case GL_FLOAT_VEC2:   result = eSHADER_VAR_FLOAT_VEC2;   break;
        case GL_FLOAT_VEC3:   result = eSHADER_VAR_FLOAT_VEC3;   break;
        case GL_FLOAT_VEC4:   result = eSHADER_VAR_FLOAT_VEC4;   break;
        case GL_INT:          result = eSHADER_VAR_INT;          break;
        case GL_INT_VEC2:     result = eSHADER_VAR_INT_VEC2;     break;
        case GL_INT_VEC3:     result = eSHADER_VAR_INT_VEC3;     break;
        case GL_INT_VEC4:     result = eSHADER_VAR_INT_VEC4;     break;
        case GL_BOOL:         result = eSHADER_VAR_BOOL;         break;
        case GL_BOOL_VEC2:    result = eSHADER_VAR_BOOL_VEC2;    break;
        case GL_BOOL_VEC3:    result = eSHADER_VAR_BOOL_VEC3;    break;
        case GL_BOOL_VEC4:    result = eSHADER_VAR_BOOL_VEC4;    break;
        case GL_FLOAT_MAT2:   result = eSHADER_VAR_FLOAT_MAT2;   break;
        case GL_FLOAT_MAT3:   result = eSHADER_VAR_FLOAT_MAT3;   break;
        case GL_FLOAT_MAT4:   result = eSHADER_VAR_FLOAT_MAT4;   break;
        case GL_SAMPLER_2D:   result = eSHADER_VAR_SAMPLER_2D;   break;
        case GL_SAMPLER_CUBE: result = eSHADER_VAR_SAMPLER_CUBE; break;
        default:              result = eSHADER_VAR_NONE;         break;
    }

    return result;
}

bool Magic3D::RendererOpenGL_Shaders::updateShaderUserVariables(Shader* shader)
{
    bool result = false;
    if (shader)
    {
        result = true;
        GLint count = 0;
        GLint maxSize = 0;
        GLint maxFrag = 0;
        glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTES, &count);
        check_gl_error();
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxSize);
        check_gl_error();
        Log::logFormat(eLOG_PLAINTEXT, "attributes: max: %d - count: %d", maxSize, count);

#if defined(MAGIC3D_OES2)
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxSize);
        check_gl_error();
#else
        glGetIntegerv(GL_MAX_VARYING_FLOATS, &maxSize);
        check_gl_error();
#endif
        Log::logFormat(eLOG_PLAINTEXT, "varying: max: %d", maxSize);

        glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &count);
        check_gl_error();

#if defined(MAGIC3D_OES2)
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxSize);
        check_gl_error();
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFrag);
        check_gl_error();
#else
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxSize);
        check_gl_error();
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFrag);
        check_gl_error();
#endif
        Log::logFormat(eLOG_PLAINTEXT, "uniforms: vert: %d - frag: %d - count: %d", maxSize, maxFrag, count);

        glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxSize);
        check_gl_error();

        shader->clearVars();

        for (int i = 0; i < count; i++)
        {
            char uniform_name[maxSize + 1];
            GLint length = 0;
            GLint size = 0;
            GLenum type = 0;

            glGetActiveUniform(shader->id, i, maxSize, &length, &size, &type, uniform_name);
            check_gl_error();

            if (!isShaderVarDefault(uniform_name))
            {
                GLint location = glGetUniformLocation(shader->id, uniform_name);
                check_gl_error();                

                std::string uname = uniform_name;
                std::string newName = uname;

                if (uname.find('[') != std::string::npos)
                {
                    newName = uname.substr(0, uname.find('['));
                }

                for (int v = 0; v < size; v++)
                {
                    if (size > 1)
                    {
                        sprintf(uniform_name, "%s[%d]", newName.c_str(), v);
                        location = glGetUniformLocation(shader->id, uniform_name);
                        sprintf(uniform_name, "%s_%d", newName.c_str(), v);
                        shader->addVar(uniform_name, getUniformType(type), location, size);
                        Log::logFormat(eLOG_PLAINTEXT, "uniform: %s - length: %d - size: %d - type: %s - location: %d", uniform_name, length, size, getUniformTypeName(getUniformType(type)).c_str(), location);
                    }
                    else
                    {
                        shader->addVar(uniform_name, getUniformType(type), location, size);
                        Log::logFormat(eLOG_PLAINTEXT, "uniform: %s - length: %d - size: %d - type: %s - location: %d", uniform_name, length, size, getUniformTypeName(getUniformType(type)).c_str(), location);
                    }
                }
            }
        }
    }
    return result;
}

void Magic3D::RendererOpenGL_Shaders::blurTexture(FBO* fbo, int amount, float strength, bool gaussian)
{
    if (shaderBlur)
    {
        GLSLShader* glsl = static_cast<GLSLShader*>(shaderBlur);
        OpenGL_FBO* oglFBO = static_cast<OpenGL_FBO*>(fbo);

        glDisable(GL_DEPTH_TEST);
        check_gl_error();

        glDepthMask(false);
        check_gl_error();

        glDisable(GL_BLEND);
        check_gl_error();

        fboViewPort->setWidth(fbo_blur->getWidth());
        fboViewPort->setHeight(fbo_blur->getHeight());
        fboViewPort->setPerspective(NULL);
        view3D(fboViewPort);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_blur->getID());
        check_gl_error();

        glClearColor(1.0, 1.0, 1.0, 1.0);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        glUseProgram(glsl->id);
        check_gl_error();
        setUniform2f(glsl->uniforms[eUNIFORM_BLUR_ORIENTATION], 1.0f, 0.0f);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_BLUR_AMOUNT], amount);
        check_gl_error();
        setUniform1f(glsl->uniforms[eUNIFORM_BLUR_STRENGTH], strength);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_BLUR_GAUSSIAN], gaussian);
        check_gl_error();

        glActiveTexture(GL_TEXTURE0);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, oglFBO->getColorTexture());
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_0], 0);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_0], oglFBO->getColorTexture());
        check_gl_error();
        setUniform2f(glsl->uniforms[eUNIFORM_SIZE_TEXTURE_0], oglFBO->getWidth(), oglFBO->getHeight());
        check_gl_error();

        glBindVertexArray(renderIDPlane.id);
        check_gl_error();

        // Draw the quad
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UINT, 0);
        check_gl_error();


        fboViewPort->setWidth(oglFBO->getWidth());
        fboViewPort->setHeight(oglFBO->getHeight());
        fboViewPort->setPerspective(NULL);
        view3D(fboViewPort);
        glBindFramebuffer(GL_FRAMEBUFFER, oglFBO->getID());
        check_gl_error();

        glClearColor(1.0, 1.0, 1.0, 1.0);
        check_gl_error();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        check_gl_error();

        setUniform2f(glsl->uniforms[eUNIFORM_BLUR_ORIENTATION], 0.0f, 1.0f);
        check_gl_error();

        glBindTexture(GL_TEXTURE_2D, fbo_blur->getColorTexture());
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_TEXTURE_0], 0);
        check_gl_error();
        setUniform1i(glsl->uniforms[eUNIFORM_USE_TEXTURE_0], fbo_blur->getColorTexture());
        check_gl_error();
        setUniform2f(glsl->uniforms[eUNIFORM_SIZE_TEXTURE_0], fbo_blur->getWidth(), fbo_blur->getHeight());
        check_gl_error();

        // Draw the quad
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UINT, 0);
        check_gl_error();

        glBindVertexArray(0);
        check_gl_error();

        if (lastShader)
        {
            glUseProgram(lastShader->id);
            check_gl_error();
        }

        glEnable(GL_DEPTH_TEST);
        check_gl_error();

        glDepthMask(true);
        check_gl_error();

        glEnable(GL_BLEND);
        check_gl_error();

        glBindFramebuffer(GL_FRAMEBUFFER, getDefaultFBO());
        check_gl_error();

        if (lastTexture0)
        {
            glBindTexture(GL_TEXTURE_2D, lastTexture0->id);
            check_gl_error();
        }
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform1i(GLint location, GLint v0)
{
    if (location >= 0)
    {
        glUniform1i(location, v0);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform1f(GLint location, GLfloat v0)
{
    if (location >= 0)
    {
        glUniform1f(location, v0);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform2i(GLint location, GLint v0, GLint v1)
{
    if (location >= 0)
    {
        glUniform2i(location, v0, v1);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    if (location >= 0)
    {
        glUniform2f(location, v0, v1);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    if (location >= 0)
    {
        glUniform3f(location, v0, v1, v2);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform3fv (GLint location, GLsizei count, const GLfloat *value)
{
    if (location >= 0)
    {
        glUniform3fv(location, count, value);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    if (location >= 0)
    {
        glUniform4f(location, v0, v1, v2, v3);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniform4fv (GLint location, GLsizei count, const GLfloat *value)
{
    if (location >= 0)
    {
        glUniform4fv(location, count, value);
    }
}

void Magic3D::RendererOpenGL_Shaders::setUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    if (location >= 0)
    {
        glUniformMatrix4fv(location, count, transpose, value);
    }
}

#endif
