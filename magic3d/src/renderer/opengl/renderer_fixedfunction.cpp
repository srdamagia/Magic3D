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

#if defined(MAGIC3D_LEGACY)
#if defined(__WIN32__)
#include <windows.h>
#endif
#include <magic3d/magic3d.h>
#include <magic3d/renderer/opengl/renderer_fixedfunction.h>
#include <math.h>
#include <magic3d/physics_geometry.h>

Magic3D::RendererOpenGL_FixedFunction::RendererOpenGL_FixedFunction() : Renderer_OpenGL()
{

}

Magic3D::RendererOpenGL_FixedFunction::~RendererOpenGL_FixedFunction()
{

}

bool Magic3D::RendererOpenGL_FixedFunction::start()
{
    instance = new RendererOpenGL_FixedFunction();

    return instance;
}

void Magic3D::RendererOpenGL_FixedFunction::initialize()
{
    Renderer_OpenGL::initialize();

    glShadeModel(GL_SMOOTH);

    glClearStencil(0);
#if !defined(MAGIC3D_OES1)
    glClearDepth(1.0f);
#endif

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_ALPHA_TEST);

    Log::log(eLOG_SUCCESS, "%s", "OpenGL renderer sucessfully started.");
}

bool Magic3D::RendererOpenGL_FixedFunction::render(Scene* scene)
{
    rendererObjects    = 0;
    rendererTriangles  = 0;
    rendererDrawCalls  = 0;
    rendererTexChanges = 0;

    ColorRGBA color = scene->getColorClear();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    std::vector<ViewPort*>::const_iterator it_v = viewports.begin();
    while (it_v != viewports.end())
    {
        ViewPort* view = *it_v++;

        if (scene->getFogStart() > 0.0f && scene->getFogFinish() > 0.0f)
        {
            glEnable(GL_FOG);
            glFogfv(GL_FOG_COLOR, (const GLfloat*)&scene->getFogColor());
            glFogf(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, scene->getFogStart());
            glFogf(GL_FOG_END, scene->getFogFinish());
        }

        view3D(view);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glLoadMatrixf((float*)&matrix_projection);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        Camera* camera = view->perspective;

        std::list<RenderObject>* objects = scene->getVisibleObjects3D(camera);

        if (scene->getShadows()->size() > 0)
        {
            drawShadows(camera, objects, scene);
        }
        else
        {
            renderObjects(camera, objects, scene, true, true, true, true);
            renderObjects(camera, scene->getZOrder(), scene, true, true, true, true);
        }

        glDisable(GL_FOG);

        view2D(view);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glLoadMatrixf((float*)&matrix_projection);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        camera = view->orthographic;

        renderObjects(camera, scene->getVisibleObjects2D(camera), scene, true, true, true, true);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

    if (window)
    {
        window->render();
    }

    return true;
}

void Magic3D::RendererOpenGL_FixedFunction::drawShadows(Camera* camera, std::list<RenderObject>* objects, Scene* scene)
{
    drawDepthBuffer(camera, objects, scene);

    drawLighting(camera, objects, scene);

    drawTexture(camera, objects, scene);
}

void Magic3D::RendererOpenGL_FixedFunction::drawDepthBuffer(Camera* camera, std::list<RenderObject>* objects, Scene* scene)
{
    glEnable(GL_DEPTH_TEST); // Activate the depth test
    glEnable(GL_CULL_FACE); // Activate the culling
    glCullFace(GL_BACK);   // We are drawing front face

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D); // no texture here
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D); // no texture here

    glDisable(GL_BLEND);   // no blending
    glDepthMask(GL_TRUE);  // Writing on z-buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);  // No writing on color buffer

    drawObjects(camera, objects, scene, false, false, false, false); // Your function to draw your scene
    drawObjects(camera, scene->getZOrder(), scene, false, false, false, false); // Your function to draw your scene
}

void Magic3D::RendererOpenGL_FixedFunction::drawLighting(Camera* camera, std::list<RenderObject>* objects, Scene *scene)
{
    glBlendFunc(GL_ONE, GL_ONE); // The blending function scr+dst, to add all the lighting
    glDepthMask(GL_FALSE);  // We stop writing to z-buffer now. We made this in the first pass, now we have it
    glEnable(GL_STENCIL_TEST); // We enable the stencil testing

    /*Matrix4 cm = camera->getView();
    float* cmatrix = (float*)&cm;*/

    std::list<Object*>* lights = scene->getLights();
    std::list<Object*>::const_iterator it_l = lights->begin();
    while (it_l != lights->end())
    {
        //Light* light = static_cast<Light*>(*it_l++);

        glDisable(GL_BLEND); // We don't want lighting. We are only writing in stencil buffer for now
        glClear(GL_STENCIL_BUFFER_BIT); // We clear the stencil buffer
        glDepthFunc(GL_LESS); // We change the z-testing function to LESS, to avoid little bugs in shadow
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // We dont draw it to the screen
        glStencilFunc(GL_ALWAYS, 0, 0); // We always draw whatever we have in the stencil buffer

        //for (/*For all triangles in your scene*/)
        {
            // NOTE: If you draw a shadow volume for EVERY single triangle, it will
            // lag a lot. The common way to do this is to determine the silhouette of the object and
            // create the shadow volume from that. Here it's just for learning or for a very little scene ;)

            //ComputeShadowVolume(); // Compute the shadow volume for this triangle

            glCullFace(GL_FRONT); // We are drawing the back faces first
            glStencilOp(GL_KEEP, GL_INCR, GL_KEEP); // We increment if the depth test fails

            //drawVolumes(camera, scene->getShadows(), scene); // Draw the shadow volume of this triangle
            //drawObjects(camera, objects, scene, false, false, false);

            glCullFace(GL_BACK); // We are now drawing the front faces
            glStencilOp(GL_KEEP, GL_DECR, GL_KEEP); // We decrement if the depth test fails

            //drawVolumes(camera, scene->getShadows(), scene); // ** Edited ** Draw the shadow volume of this triangle
            //drawObjects(camera, objects, scene, false, false, false);
        }
    }

        // We draw our lighting now that we created the shadows area in the stencil buffer
        glDepthFunc(GL_LEQUAL); // we put it again to LESS or EQUAL (or else you will get some z-fighting)
        glCullFace(GL_BACK); // we draw the front face
        glEnable(GL_BLEND); // We enable blending
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // We enable color buffer
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Drawing will not affect the stencil buffer
        glStencilFunc(GL_EQUAL, 0x0, 0xff); // And the most important thing, the stencil function. Drawing if equal to 0

        // Now, draw your scene with the lighting method you want
        glEnable(GL_LIGHTING); // Let's use openGL lighting
        //prepareLight(light, cmatrix); // set all the light parameters (color, attenuation, but no ambient)

        drawObjects(camera, objects, scene, true, false, true, false); // Now draw your scene (we're still not writing to the depth buffer)
        drawObjects(camera, scene->getZOrder(), scene, true, false, true, false); // Now draw your scene (we're still not writing to the depth buffer)

        glDisable(GL_LIGHTING); // We dont want lighting anymore
    // Now we do all of this for each light in the scene

    glDisable(GL_STENCIL_TEST); // Now we can disable the stencil buffer test
}

void Magic3D::RendererOpenGL_FixedFunction::drawTexture(Camera* camera, std::list<RenderObject>* objects, Scene* scene)
{
    glColor4f(1,1,1,1); // giving them white color
    glCullFace(GL_BACK); // Drawing front faces
    glDepthFunc(GL_LEQUAL); // we put it again to LESS or EQUAL (or else you will get some z-fighting).. annoying lol
    glEnable(GL_BLEND); // We enable blending
    glBlendFunc(GL_DST_COLOR,GL_ZERO); // We modulate: scr*dst

    //With Textures
    drawObjects(camera, objects, scene, true, true, false, true); // Drawing our scene with textures On
    drawObjects(camera, scene->getZOrder(), scene, true, true, false, true); // Drawing our scene with textures On

    glDepthMask(GL_TRUE); // And finally, we enable writing to z-buffer.
                          // If this is not Enabled, when you will Clear your depth buffer bit, it will not work
}

void Magic3D::RendererOpenGL_FixedFunction::drawObjects(Camera* camera, std::list<RenderObject>* objects, Scene* scene, bool blend, bool textures, bool lights, bool gizmos)
{
    renderObjects(camera, objects, scene, blend, textures, lights, gizmos);
}

void Magic3D::RendererOpenGL_FixedFunction::drawVolumes(Camera* camera, std::list<RenderObject>* objects, Scene* scene)
{
    renderObjects(camera, objects, scene, false, false, false, false);
}

bool Magic3D::RendererOpenGL_FixedFunction::prepareLights(Camera* camera, Scene* scene, const RenderObject* object)
{
    bool hasLights = false;
    std::list<Object*>* lights = scene->getLights();

    if (lights && lights->size() > 0)
    {
        Matrix4 cm = camera->getView();
        float* cmatrix = (float*)&cm;

        std::list<Object*>::const_iterator it_l = lights->begin();
        while (it_l != lights->end())
        {
            Light* light = static_cast<Light*>(*it_l++);

            if (prepareLight(light, cmatrix, object));
            {
                glEnable(GL_LIGHTING);
                hasLights = true;
            }
        }
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if (hasLights)
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat*)&scene->getColorAmbient());
    }

    return hasLights;
}

bool Magic3D::RendererOpenGL_FixedFunction::prepareLight(Light* light, float* matrix, const RenderObject* object)
{
    bool on = false;

    if (light->getIndex() != eLIGHT_OFF)
    {
        on = true;

        glPushMatrix();
        glLoadMatrixf(matrix);

        renderLight(light, object);

        glPopMatrix();
    }

    return on;
}

bool Magic3D::RendererOpenGL_FixedFunction::prepareMatrix(Camera* camera, const RenderObject* object, float z)
{
    Matrix4 m = object->object->getType() == eOBJECT_LIGHT ? camera->getView() : object->modelView;
    float* matrix = (float*)&m;

    if (object->object->getRender() == eRENDER_2D)
    {
        matrix[14] = -z - matrix[14] - object->object->getMaxSizeFromParent();
    }

    if (object->object->getType() == eOBJECT_PARTICLES && !static_cast<Particles*>(object->object)->isFollowingEmitter())
    {
        Vector3 pos = (camera->getView() * Vector4(static_cast<Particles*>(object->object)->getWorldPosition(), 1.0f)).getXYZ();

        object->object->setZOrderFactor(pos.getZ());
    }
    else
    {
        object->object->setZOrderFactor(m.getTranslation().getZ());
    }

    glLoadMatrixf(matrix);

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderObject(Camera* camera, const RenderObject *object, bool textures, bool lights, bool gizmos)
{
    if (gizmos && object->object->isShowingGrid())
    {
        renderGrid(camera, object->object);
    }

    bool selected = object->object->isSelected();
    bool isParticles = object->object->getType() == eOBJECT_PARTICLES || object->object->getType() == eOBJECT_LIGHT;

    if (isParticles && !textures)
    {
        return false;
    }

    if (isParticles || selected)
    {
        glEnable(GL_COLOR_MATERIAL);
    }
    else
    {
        glDisable(GL_COLOR_MATERIAL);
    }

    if (object->object->getRender() == eRENDER_2D || isParticles)
    {
        if (!isParticles)
        {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthMask(GL_FALSE);
    }

    if (selected)
    {
        glColor4f(0.0f, 0.5f, 1.0f, 1.0f);
    }
    else
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

    std::vector<Mesh*>* meshes = object->object->getMeshes();
    std::vector<Mesh*>::const_iterator it_m = meshes->begin();
    while (it_m != meshes->end())
    {
        Mesh* mesh = *it_m++;

        if (mesh->isVisible() && (mode != eRENDER_MODE_WIREFRAME || mesh->getData() != object->object->getCollisionMesh()))
        {
            if (lights)
            {
                if (mesh->isIlluminated() && object->object->getType() != eOBJECT_LIGHT)
                {
                    glEnable(GL_LIGHTING);
                }
                else
                {
                    glDisable(GL_LIGHTING);
                }
            }
            if (mesh->isShadowed())
            {
                object->object->setShadowed(true);
            }

            if (textures)
            {
                renderTextures(mesh);
            }

            if (mesh->isDoubleSide())
            {
                glDisable(GL_CULL_FACE);
            }

            renderMeshData(mesh->getData(), object->object->isSelected());

            glEnable(GL_CULL_FACE);
        }
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);

#if defined(_MGE_) || defined(DEBUG)
    if (gizmos && (object->object->isSelected() || mode == eRENDER_MODE_WIREFRAME))
    {
        renderCollision(camera, object->object);
        if (object->object->getType() == eOBJECT_MODEL)
        {
            renderSkeleton(camera, object->object);
        }
    }
#endif

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderObjects(Camera* camera, std::list<RenderObject>* objects, Scene *scene, bool blend, bool textures, bool lights, bool gizmos)
{
    bool result = true;

    if (camera && objects)
    {
        float zFactor = 0.01f;
        float zSize = zFactor * objects->size();
        float z = zSize;

        lastTexture0 = 0;
        lastTexture1 = 0;

        bool hasLights = lights;

        if (blend)
        {
            glEnable(GL_BLEND);
        }

        if (lights)
        {
            hasLights = prepareLights(camera, scene, NULL);
        }

        bool wire = mode == eRENDER_MODE_WIREFRAME;
        if (wire)
        {
#if !defined(MAGIC3D_OES1)
            glPushAttrib(GL_POLYGON_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
        }

        std::list<RenderObject>::const_iterator it_o = objects->begin();
        while (it_o != objects->end())
        {
            const RenderObject* object = &(*it_o++);
            object->object->setShadowed(false);

            rendererObjects++;

            glPushMatrix();

            prepareMatrix(camera, object, z);

            renderObject(camera, object, textures, hasLights, gizmos);

            glPopMatrix();
        }

        if (wire)
        {
#if !defined(MAGIC3D_OES1)
            glPopAttrib();
#endif
        }

        if (hasLights)
        {
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
            glDisable(GL_LIGHT3);
            glDisable(GL_LIGHT4);
            glDisable(GL_LIGHT5);
            glDisable(GL_LIGHT6);
            glDisable(GL_LIGHT7);
        }
    }
    else
    {
        result = false;
    }

    return result;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderTextures(Mesh* mesh)
{
    std::vector<Material*>* materials = mesh->getMaterials();
    std::vector<Material*>::const_iterator it_mat = materials->begin();

    bool noTexture0 = true;
    bool noTexture1 = true;

    while (it_mat != materials->end())
    {
        Material* material = *it_mat++;
        Texture* texture0 = material->getTexture(0);
        Texture* texture1 = material->getTexture(1);

        glAlphaFunc(GL_GEQUAL, material->getAlphaTest());

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  (const GLfloat*)&material->getColorAmbient());
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  (const GLfloat*)&material->getColorDiffuse());
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (const GLfloat*)&material->getColorEmission());
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const GLfloat*)&material->getColorSpecular());
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->getShininess());

        if (texture0 || texture1)
        {
            glEnable(GL_BLEND);
        }
        switch (material->getBlend())
        {
            case eBLEND_ALPHA:    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); break;
            case eBLEND_ADD:      glBlendFunc( GL_SRC_ALPHA, GL_ONE ); break;
            case eBLEND_MULTIPLY: glBlendFunc( GL_DST_COLOR, GL_ZERO ); break;
            case eBLEND_DIVIDE:   glBlendFunc( GL_ONE, GL_ONE ); break;
            case eBLEND_INVERT:   glBlendFunc( GL_ONE, GL_ZERO ); break;
            case eBLEND_COLOR:    glBlendFunc( GL_SRC_COLOR, GL_DST_COLOR ); break;
        }

        if (texture0 && mode != eRENDER_MODE_WIREFRAME)
        {
            noTexture0 = false;
            glActiveTexture(GL_TEXTURE0);
            glEnable(GL_TEXTURE_2D);
            if (texture0->id != lastTexture0)
            {
                glBindTexture(GL_TEXTURE_2D, texture0->id);

                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

                rendererTexChanges++;
                lastTexture0 = texture0->id;
            }
        }

        if (texture1 && mode != eRENDER_MODE_WIREFRAME)
        {
            noTexture1 = false;
            glActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);
            if (texture1->id != lastTexture1)
            {
                glBindTexture(GL_TEXTURE_2D, texture1->id);

                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

                rendererTexChanges++;
                lastTexture1 = texture1->id;
            }
        }
    }

    if (noTexture0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        lastTexture0 = 0;
    }

    if (noTexture1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        lastTexture1 = 0;
    }

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderMeshData(MeshData* data, bool selected)
{
    if (!data)
    {
        return false;
    }

    if (data->isVBO())
    {
#if defined(MAGIC3D_OES1)
        glBindBuffer(GL_ARRAY_BUFFER, data->getVboID().data);
#else
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, data->getVboID().data);
#endif

        glVertexPointer  (3, GL_FLOAT, sizeof(Vertex3D), 0);
        glNormalPointer  (   GL_FLOAT, sizeof(Vertex3D), (void*)(sizeof(Vector3)));
        glColorPointer(4, GL_FLOAT, sizeof(Vertex3D), (void*)(sizeof(Vector3) + sizeof(Vector3)));

        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), (void*)(sizeof(Vector3) + sizeof(Vector3) + sizeof(ColorRGBA)));
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), (void*)(sizeof(Vector3) + sizeof(Vector3) + sizeof(ColorRGBA) + sizeof(Texture2D)));

#if defined(MAGIC3D_OES1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->getVboID().index);
#else
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, data->getVboID().index);
        glIndexPointer(GL_UINT, 0, 0);
#endif

        int size = data->getTrianglesCount() * 3;
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

        rendererTriangles += data->getTrianglesCount();
        rendererDrawCalls++;

#if defined(MAGIC3D_OES1)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#else
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
#endif

    }
    else if (data->getVertices()->size() > 0)
    {
        glVertexPointer  (3, GL_FLOAT, sizeof(Vertex3D), &data->getVertices()->front().position);
        glNormalPointer  (   GL_FLOAT, sizeof(Vertex3D), &data->getVertices()->front().normal);
        if (selected)
        {
            glDisableClientState(GL_COLOR_ARRAY);
        }
        else
        {
            glColorPointer(4, GL_FLOAT, sizeof(Vertex3D), &data->getVertices()->front().color);
        }
        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), &data->getVertices()->front().uv[0]);
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), &data->getVertices()->front().uv[1]);

        int size = data->getTrianglesCount() * 3;
        switch (data->getType())
        {
            case eMESH_TRIANGLES:
            {
                glDrawElements(GL_TRIANGLES,      size, GL_UINT, &data->getTriangles()->front());
                break;
            }
            case eMESH_TRIANGLES_STRIP:
            {
                glDrawElements(GL_TRIANGLE_STRIP, size, GL_UINT, &data->getTriangles()->front());
                break;
            }
        }

        if (selected)
        {
            glEnableClientState(GL_COLOR_ARRAY);
        }

        rendererTriangles += data->getTrianglesCount();
        rendererDrawCalls++;
    }

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderGrid(Camera *camera, Object *object)
{
#if !defined(MAGIC3D_OES1)
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);

    glEnable(GL_COLOR_MATERIAL);

    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisableClientState(GL_NORMAL_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    glPushMatrix();
    glLoadIdentity();
    Matrix4 m = camera->getView() * Matrix4(object->getRender() == eRENDER_2D ? Quaternion::rotationX(Math::radians(90.0f)) : Quaternion::identity(), object->getPositionFromParent());
    float* matrix = (float*)&m;
    glLoadMatrixf(matrix);
    glScalef(getGrid().getX(), getGrid().getY(), getGrid().getZ());

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
        glColorPointer(4, GL_FLOAT, sizeof(Vector4), c);
        glDrawElements(GL_QUADS, size, GL_UINT, q);

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

    int size = sizeof(gridQuads) / sizeof(unsigned int);

#if !defined(MAGIC3D_OES1)
    glVertexPointer(3, GL_FLOAT, 0, gridPositions);
    glColorPointer(4, GL_FLOAT, 0, gridColors);

    glDrawElements(GL_QUADS, size, GL_UINT, gridQuads);
#endif

    glPopMatrix();
    rendererTriangles += size / 4 / 2;
    rendererDrawCalls++;

    glEnableClientState(GL_NORMAL_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_CULL_FACE);

#if !defined(MAGIC3D_OES1)
    glPopAttrib();
#endif

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderCollision(Camera *camera, Object *object)
{
#if !defined(MAGIC3D_OES1)
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glBlendFunc( GL_ONE, GL_ZERO );
    glDisable(GL_LIGHTING);

    glEnable(GL_COLOR_MATERIAL);

    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    int size = 0;

    glPushMatrix();
    glLoadIdentity();

    Matrix4 m = camera->getView() * Matrix4(object->getRotation(), object->getPosition());
    float* matrix = (float*)&m;
    glLoadMatrixf(matrix);
    //if (object->getRigidBody())
    //{
        //glTranslatef(object->getShapePosition().getX(), object->getShapePosition().getY(), object->getShapePosition().getZ());
    //}
    glScalef(1.0f / object->getScale().getX(), 1.0f / object->getScale().getY(), 1.0f / object->getScale().getZ());
    switch (object->getShape())
    {
        case ePHYSICS_SHAPE_TRIANGLEMESH:
        case ePHYSICS_SHAPE_CONVEXHULL:
        {
            MeshData* data = object->getCollisionMesh();
            if (data)
            {
                size = data->getTrianglesCount() * 3;
            }

            renderMeshData(data, false);

            break;
        }
        case ePHYSICS_SHAPE_CAPSULE:
        {
            glPushMatrix();
            glTranslatef(0.0f, object->getShapeSize().getY() * 0.5f, 0.0f);
            glScalef(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f);
            size = sizeof(sphereTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, spherePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
            glPopMatrix();

            glPushMatrix();
            glScalef(object->getRadius() * 2.0f, object->getShapeSize().getY(), object->getRadius() * 2.0f);
            size = sizeof(cylinderTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, cylinderPositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, cylinderTriangles);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0.0f, -object->getShapeSize().getY() * 0.5f, 0.0f);
            glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
            glScalef(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f);
            size = sizeof(sphereTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, spherePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
            glPopMatrix();
            break;
        }
        case ePHYSICS_SHAPE_SPHERE:
        {
            glPushMatrix();
            glScalef(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f);
            size = sizeof(sphereTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, spherePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
            glPopMatrix();

            glPushMatrix();
            glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
            glScalef(object->getRadius() * 2.0f, object->getRadius() * 2.0f, object->getRadius() * 2.0f);
            size = sizeof(sphereTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, spherePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, sphereTriangles);
            glPopMatrix();
            break;
        }
        case ePHYSICS_SHAPE_CYLINDER:
        {
            glPushMatrix();
            glScalef(object->getShapeSize().getX(), object->getShapeSize().getY(), object->getShapeSize().getZ());
            size = sizeof(cylinderTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, cylinderPositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, cylinderTriangles);
            glPopMatrix();
            break;
        }
        case ePHYSICS_SHAPE_CONE:
        {
            glPushMatrix();
            if (object->getRender() == eRENDER_2D)
            {
                glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
            }
            glScalef(object->getRadius() * 2.0f, object->getShapeSize().getY(), object->getRadius() * 2.0f);
            size = sizeof(coneTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, conePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, coneTriangles);
            glPopMatrix();
            break;
        }
        default:
        {
            glPushMatrix();
            glScalef(object->getShapeSize().getX(), object->getShapeSize().getY(), object->getShapeSize().getZ());
            size = sizeof(cubeTriangles) / sizeof(unsigned int);
            glVertexPointer(3, GL_FLOAT, 0, cubePositions);
            glDrawElements(GL_TRIANGLES, size, GL_UINT, cubeTriangles);
            glPopMatrix();
            break;
        }
    }
    glPopMatrix();
    rendererTriangles += size / 3;
    rendererDrawCalls++;

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_CULL_FACE);

#if !defined(MAGIC3D_OES1)
    glPopAttrib();
#endif

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderSkeleton(Camera* camera, Object* object)
{
    Skeleton* skeleton = ((Model*)(object->isClone() ? ((Clone*)object)->getObject() : object))->getSkeleton();
    if (object->getType() != eOBJECT_MODEL || !skeleton)
    {
        return false;
    }

#if !defined(MAGIC3D_OES1)
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glBlendFunc( GL_ONE, GL_ZERO );
    glDisable(GL_LIGHTING);

    glEnable(GL_COLOR_MATERIAL);

    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    std::vector<Bone*>* bones = skeleton->getBones();

    std::vector<Bone*>::const_iterator it_b = bones->begin();
    while (it_b != bones->end())
    {
        Bone* b = *it_b++;

        renderBone(camera, object, b);
    }

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

#if !defined(MAGIC3D_OES1)
    glPopAttrib();
#endif

    return true;
}

bool Magic3D::RendererOpenGL_FixedFunction::renderBone(Camera* camera, Object* object, Bone* bone)
{
    int size = 0;

    Matrix4 p = camera->getView() * object->getMatrixFromParent() * (bone->getParent() ? bone->getParent()->getMatrixFromParent() : bone->getMatrixFromParent());
    Matrix4 m = camera->getView() * object->getMatrixFromParent() * bone->getMatrixFromParent();

    if (bone->isSelected())
    {
        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if (bone->getParent() && !bone->getParent()->getParent())
    {
        glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    }

    glPushMatrix();
    glLoadIdentity();

    float linePositions[6] = {
        p.getTranslation().getX(),
        p.getTranslation().getY(),
        p.getTranslation().getZ(),
        m.getTranslation().getX(),
        m.getTranslation().getY(),
        m.getTranslation().getZ()
    };
    unsigned int lineIndexes[2] = {0, 1};
    glVertexPointer(3, GL_FLOAT, 0, linePositions);
    size = sizeof(lineIndexes) / sizeof(unsigned int);
    glPointSize(5.0f);
    if (bone->getChilds()->size() == 0)
    {
        glDrawElements(GL_POINTS, 1, GL_UINT, &lineIndexes[1]);
    }

    if (bone->getParent() && bone->getParent()->isSelected())
    {
        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if (bone->getParent() && !bone->getParent()->getParent())
    {
        glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    }
    glDrawElements(GL_POINTS, 1, GL_UINT, &lineIndexes[0]);

    glPointSize(1.0f);
    glDrawElements(GL_LINES, size, GL_UINT, lineIndexes);

    glPopMatrix();

    rendererTriangles += size / 3;
    rendererDrawCalls++;

    return true;
}

void Magic3D::RendererOpenGL_FixedFunction::renderLight(Light* light, const RenderObject* object)
{
    if (light)
    {
        GLenum index = eLIGHT_0;

        switch (light->getIndex())
        {
            case eLIGHT_0: index = GL_LIGHT0; break;
            case eLIGHT_1: index = GL_LIGHT1; break;
            case eLIGHT_2: index = GL_LIGHT2; break;
            case eLIGHT_3: index = GL_LIGHT3; break;
            case eLIGHT_4: index = GL_LIGHT4; break;
            case eLIGHT_5: index = GL_LIGHT5; break;
            case eLIGHT_6: index = GL_LIGHT6; break;
            case eLIGHT_7: index = GL_LIGHT7; break;
            default: index = GL_LIGHT0;
        }

        glEnable(index);

        Matrix4 m = light->getMatrixFromParent();

        Vector3 pos = m.getTranslation();//light->getPosition();
        Vector3 dir = m.getCol2().getXYZ();//light->getPosition();

        glLightfv(index, GL_AMBIENT,  (const GLfloat*)&light->getColorAmbient());
        glLightfv(index, GL_DIFFUSE,  (const GLfloat*)&light->getColorDiffuse());
        glLightfv(index, GL_SPECULAR, (const GLfloat*)&light->getColorSpecular());

        GLfloat position[4];

        switch (light->getLightType())
        {
            case eLIGHT_TYPE_POINT:
            case eLIGHT_TYPE_SPOT:
            {
                position[0] = pos.getX();
                position[1] = pos.getY();
                position[2] = pos.getZ();
                position[3] = 1.0f;
                break;
            }
            case eLIGHT_TYPE_DIRECTIONAL:
            {
                position[0] = dir.getX();
                position[1] = dir.getY();
                position[2] = dir.getZ();
                position[3] = 0.0f;
                break;
            }
            default: break;
        }

        glLightfv(index, GL_POSITION, position);

        glLightf(index, GL_CONSTANT_ATTENUATION, light->getAttenuationConstant());
        glLightf(index, GL_LINEAR_ATTENUATION, light->getAttenuationLinear());
        glLightf(index, GL_QUADRATIC_ATTENUATION, light->getAttenuationQuadratic());

        if (light->getLightType() == eLIGHT_TYPE_SPOT)
        {
            glLightf(index,  GL_SPOT_CUTOFF, light->getCutOff());
            glLightf(index,  GL_SPOT_EXPONENT, light->getExponent());
            glLightfv(index, GL_SPOT_DIRECTION, (const GLfloat*)&dir/*light->getDirectionFront()*/);
        }
        else
        {
            glLightf(index, GL_SPOT_CUTOFF, 180.0f);
        }

    }
}
#endif
