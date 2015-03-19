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

#if defined(MAGIC3D_LEGACY)
#ifndef MAGIC3D_RENDERER_OPENGL_FIXEDFUNCTION_H
#define MAGIC3D_RENDERER_OPENGL_FIXEDFUNCTION_H

#include <magic3d/renderer/opengl/renderer_opengl.h>

namespace Magic3D
{

class RendererOpenGL_FixedFunction : public Renderer_OpenGL
{
private:
    void drawShadows(Camera* camera, std::list<RenderObject>* objects, Scene* scene);

    void drawDepthBuffer(Camera* camera, std::list<RenderObject>* objects, Scene* scene);
    void drawLighting(Camera* camera, std::list<RenderObject>* objects, Scene* scene);
    void drawTexture(Camera* camera, std::list<RenderObject>* objects, Scene* scene);
    void drawObjects(Camera* camera, std::list<RenderObject>* objects, Scene* scene, bool blend, bool textures, bool lights, bool gizmos);
    void drawVolumes(Camera* camera, std::list<RenderObject>* objects, Scene* scene);

    bool prepareLights(Camera* camera, Scene* scene, const RenderObject* object);
    bool prepareLight(Light* light, float* matrix, const RenderObject* object);
    bool prepareMatrix(Camera* camera, const RenderObject *object, float z);
    bool renderObject(Camera* camera, const RenderObject *object, bool textures, bool lights, bool gizmos);

    bool renderObjects(Camera* camera, std::list<RenderObject>* objects, Scene* scene, bool blend, bool textures, bool lights, bool gizmos);
    bool renderMeshData(MeshData* data, bool selected);
    bool renderTextures(Mesh* mesh);
    bool renderGrid(Camera *camera, Object *object);
    bool renderCollision(Camera* camera, Object* object);
    bool renderSkeleton(Camera* camera, Object* object);
    bool renderBone(Camera* camera, Object* object, Bone* bone);

    void renderLight(Light* light, const RenderObject* object);

protected:
    RendererOpenGL_FixedFunction();
    virtual ~RendererOpenGL_FixedFunction();
    virtual void initialize();

public:
    static bool start();

    virtual bool render(Scene* scene);
};

}
#endif // MAGIC3D_RENDERER_OPENGL_FIXEDFUNCTION_H
#endif
