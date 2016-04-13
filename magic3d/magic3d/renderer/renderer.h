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

#ifndef MAGIC3D_RENDERER_H
#define MAGIC3D_RENDERER_H

#include <magic3d/resource/resourcemngr.h>
#include <magic3d/scene.h>

namespace Magic3D
{

class Window;
class Object;
class Image;
class Scene;

enum RENDER_MODE
{
    eRENDER_MODE_SOLID,
    eRENDER_MODE_WIREFRAME,
    eRENDER_MODE_NORMALS,
    eRENDER_MODE_UVS,
    eRENDER_MODE_DEPTH,
    eRENDER_MODE_SCREEN_WIREFRAME
};

enum GIZMO_TYPE
{
    eGIZMO_TYPE_BOX,
    eGIZMO_TYPE_SPHERE,
    eGIZMO_TYPE_CAPSULE,
    eGIZMO_TYPE_CYLINDER,
    eGIZMO_TYPE_CONE,
    eGIZMO_TYPE_MESH,
    eGIZMO_TYPE_BOUNDING_BOX
};

enum SHADER_TYPE
{
    eSHADER_TYPE_VERTEX,
    eSHADER_TYPE_FRAGMENT
};

struct PlaneVertex
{
    Vector3 vertex;
    Texture2D uv;
};

class ViewPort
{   
private:
    ViewPort* parent;

protected:
    Vector4 area;

    Camera* orthographic;
    Camera* perspective;
    int width;
    int height;

    ViewPort* stereoscopy;

    virtual void updateStereoscopy();

public:
    ViewPort(Vector4 area, int width, int height);
    virtual ~ViewPort();

    void setArea(Vector4 area);
    void setArea(float x, float y, float z, float w);
    const Vector4& getArea();

    void setOrthographic(Camera* orthographic);
    Camera* getOrthographic();

    void setPerspective(Camera* perspective);
    Camera* getPerspective();

    void setWidth(int width);
    int getWidth();

    void setHeight(int height);
    int getHeight();

    void setParent(ViewPort* parent);
    ViewPort* getParent();
};

class FBO
{
public:
    FBO() {}
    virtual ~FBO() {}
};

class Renderer : public btIDebugDraw
{
protected:
    static int vboCreated;
    static int vboDestroyed;
    std::vector<ViewPort*> viewports;

    std::vector<Object*> selectedObjects;
    std::vector<Vector4> distortions;
    MeshData* debugLines[2];
    MeshData* debugPoints[2];
    std::vector<PlaneVertex> posteffectsVertices;
    std::vector<vindex> posteffectsTriangles;
    std::vector<LineIndexes> posteffectsLines;

    static Renderer* instance;
    Window* window;

    Scene* scene;

    Matrix4 matrix_projection;
    Matrix4 matrix_model;
    Matrix4 matrix_view;
    Matrix4 matrix_modelView;
    Matrix4 matrix_shadows;
    Matrix4 matrix_camera;
    Vector3 grid2D;
    Vector3 grid3D;

    Texture* lastTexture0;
    Texture* lastTexture1;
    BLEND lastBlend;

    PlaneVertex planeVertices[4];
    vindex planeTriangles[4];

    int currentViewPort;
    int rendererObjects;
    int rendererTriangles;
    int rendererDrawCalls;
    int rendererTexChanges;
    int rendererShadows;
    int rendererReflective;

    int version;

    float shadowsMS;
    float reflectionsMS;
    float defaultMS;
    float fboMS;
    float screenMS;
    float spritesMS;
    float gizmosMS;

    RENDER_MODE mode;

    bool extVBO;
    bool extFBO;
    bool extMapBuffer;
    bool extDepthTexture;
    bool showGizmos;
    bool showGizmosPhysics;
    bool showOctree;
    bool showOctreeObjects;
    bool screenEffects;

    Renderer();
    virtual ~Renderer();

    bool isRenderingScreenEffects();

private:
    int debugMode;

    bool updateObjects(std::vector<Object*>* objects);
    void finishDebug();

public:
    GUILabel* lblFPS;
    GUILabel* lblINFO;
    GUILabel* lblCURSOR;

    static bool start();
    static bool finish();
    virtual void initialize() = 0;

    static Renderer* getInstance();

    virtual void setDefaultFBO(unsigned int fbo) = 0;
    virtual unsigned int getDefaultFBO() = 0;

    virtual int getVersion();

    virtual bool hasMapBuffer();

    virtual bool render(Scene* scene) = 0;
    virtual void renderScreen(Camera* camera) = 0;

    virtual Window* createWindow(bool internal = false);
    virtual void resizeWindow(int width, int height) = 0;

    virtual unsigned int createTexture(Image* image, bool mipmap, bool clamp) = 0;
    virtual void deleteTexture(unsigned int texture) = 0;
    virtual void setTextureWrap(Texture* texture, bool clamp) = 0;
    virtual void blurTexture(FBO* fbo, int amount, float strength, bool gaussian) = 0;

    virtual unsigned int getRenderToTextureColor() = 0;
    virtual unsigned int getRenderToTextureDepth() = 0;

    virtual unsigned int getShadowsTexture() = 0;
    virtual unsigned int getReflectionsTexture() = 0;

    virtual unsigned int getGlowTextureColor() = 0;
    virtual unsigned int getGlowTextureDepth() = 0;

    virtual unsigned int getScreenTextureColor() = 0;
    virtual unsigned int getScreenTextureDepth() = 0;

    virtual int getRenderToTextureWidth() = 0;
    virtual int getRenderToTextureHeight() = 0;

    virtual int getShadowsTextureWidth() = 0;
    virtual int getShadowsTextureHeight() = 0;

    virtual int getReflectionsTextureWidth() = 0;
    virtual int getReflectionsTextureHeight() = 0;

    virtual int getGlowTextureWidth() = 0;
    virtual int getGlowTextureHeight() = 0;

    virtual int getScreenTextureWidth() = 0;
    virtual int getScreenTextureHeight() = 0;

    virtual bool renderShadows(Scene* scene) = 0;
    virtual bool renderReflections(Scene* scene, Camera* camera, FBO* fbo, ViewPort* view) = 0;
    virtual bool renderGlow(Scene* scene, Camera* camera, ViewPort* view) = 0;

    Window* getWindow();
    STEREOSCOPY getEye(ViewPort* view);

    void addViewPort(ViewPort* view);
    void removeAllViewPorts();
    std::vector<ViewPort*>* getViewPorts();
    ViewPort* getCurrentViewPort();
    ViewPort* getViewPort(int index);

    float getShadowsMS();
    float getReflectionsMS();
    float getDefaultMS();
    float getFboMS();
    float getScreenMS();
    float getSpritesMS();
    float getGizmosMS();

    int getObjectsCount();
    int getShadowsCount();
    int getReflectiveCount();
    int getTrianglesCount();
    int getDrawCallsCount();
    int getTexChangesCount();
    int getVBOCount();

    RENDER_MODE getRenderMode();
    void setRenderMode(RENDER_MODE mode);

    Vector3& getGrid2D();
    void setGrid2D(Vector3 grid);

    Vector3& getGrid3D();
    void setGrid3D(Vector3 grid);

    bool isShowingGizmos();
    void setShowGizmos(bool show);

    bool isShowingGizmosPhysics();
    void setShowGizmosPhysics(bool show);

    bool isShowingOctree();
    void setShowOctree(bool show);
    bool isShowingOctreeObjects();
    void setShowOctreeObjects(bool show);

    bool isUsingScreenEffects();
    void setUsingScreenEffects(bool effects);

    float getScreenAspectX(bool invert);
    float getScreenAspectY(bool invert);

    virtual RENDER_ID createVBO(void* vertices, int vcount, void* indexes, int icount) = 0;
    virtual bool createVBO(MeshData* data) = 0;
    virtual bool updateVBO(MeshData* data) = 0;
    virtual bool updateVBOVertices(uint id, float* data, int size) = 0;
    virtual RENDER_ID copyVBO(const RENDER_ID& id) = 0;
    virtual float* mapVBO(uint id) = 0;
    virtual bool unmapVBO() = 0;
    virtual vindex* mapIndexes(uint id) = 0;
    virtual bool unmapIndexes() = 0;
    virtual void deleteVBO(RENDER_ID id) = 0;

    virtual void setShaderDefault(Shader* shader) = 0;
    virtual void setShaderGizmo(Shader* shader) = 0;
    virtual void setShaderBlur(Shader* shader) = 0;
    virtual void setMaterialScreen(Material* material) = 0;
    virtual void setMaterialDefault(Material* material) = 0;
    virtual Shader* createShader(std::string file) = 0;
    virtual bool updateShader(Shader* shader) = 0;
    virtual void deleteShader(unsigned int shader) = 0;
    virtual bool compileShader(unsigned int& shader, SHADER_TYPE type, std::string file) = 0;
    virtual bool linkProgram(unsigned int prog) = 0;
    virtual bool validateProgram(unsigned int prog) = 0;
    virtual bool updateShaderUserVariables(Shader* shader) = 0;

    virtual void addDistortion(Vector3 position, DISTORTION type, float radius, float frequency, float factor, float wave);
    virtual void addLine(Vector3 start, Vector3 finish, bool orthographic, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    virtual void addPoint(Vector3 position, float size, bool orthographic, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    virtual bool isProfileObject(Object* object);

    virtual void matrixToArray(float array[16], const Matrix4& matrix) = 0;
    virtual void arrayToMatrix(const float array[16], Matrix4& matrix) = 0;

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        addLine(Vector3(from.getX(), from.getY(), from.getZ()), Vector3(to.getX(), to.getY(), to.getZ()), false, ColorRGBA(color.getX(), color.getY(), color.getZ(), 1.0f));
    }

    virtual void drawTransform(const btTransform& transform, btScalar orthoLen)
    {
        btIDebugDraw::drawTransform(transform, orthoLen);
        btVector3 start = transform.getOrigin();
        if (orthoLen < 1.0f)
        {
            addPoint(Vector3(start.x(), start.y(), start.z()), 5.0f, false, ColorRGBA(0.0f, 0.5f, 0.0f, 1.0f));
        }
    }

    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        if (lifeTime > 0)
        {
            addPoint(Vector3(PointOnB.x(), PointOnB.y(), PointOnB.z()), 5.0f, false, ColorRGBA(color.x(), color.y(), color.z(), 1.0f));
            addLine(Vector3(PointOnB.x(), PointOnB.y(), PointOnB.z()), Vector3(PointOnB.x(), PointOnB.y(), PointOnB.z()) + Vector3(normalOnB.x(), normalOnB.y(), normalOnB.z()) * distance, false, ColorRGBA(color.x(), color.y(), color.z(), 1.0f));
        }
    }


    virtual void reportErrorWarning(const char* warningString)
    {
        Log::log(eLOG_FAILURE, warningString);
    }

    virtual void draw3dText(const btVector3& location,const char* textString)
    {
        if (textString && location.getX())
        {
        }
    }

    virtual void setDebugMode(int debugMode)
    {
        this->debugMode = debugMode;
    }

    virtual int getDebugMode() const
    {
        return debugMode;
    }
};

class PhysicsRenderer2D : public btIDebugDraw
{
private:
    int debugMode;
public:
    PhysicsRenderer2D()
    {
        debugMode = DBG_DrawWireframe | /*DBG_DrawAabb |*/ DBG_DrawContactPoints | DBG_NoHelpText | DBG_DrawConstraints | DBG_DrawConstraintLimits;
    }

    virtual ~PhysicsRenderer2D()
    {

    }

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        Renderer::getInstance()->addLine(Vector3(from.getX(), -from.getY(), from.getZ()), Vector3(to.getX(), -to.getY(), to.getZ()), true, ColorRGBA(color.getX(), color.getY(), color.getZ(), 1.0f));
    }

    virtual void drawTransform(const btTransform& transform, btScalar orthoLen)
    {        
        btVector3 start = transform.getOrigin();
        btScalar l = orthoLen * 0.1f;
        drawLine(start, start + transform.getBasis() * btVector3(l, 0, 0), btVector3(0.7f,0,0));
        drawLine(start, start + transform.getBasis() * btVector3(0, -l, 0), btVector3(0,0.7f,0));
        drawLine(start, start + transform.getBasis() * btVector3(0, 0, l), btVector3(0,0,0.7f));
        if (orthoLen < 1.0f)
        {
            Renderer::getInstance()->addPoint(Vector3(start.x(), -start.y(), start.z()), 5.0f, true, ColorRGBA(0.0f, 0.5f, 0.0f, 1.0f));
        }
    }

    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        if (lifeTime > 0)
        {
            Renderer::getInstance()->addPoint(Vector3(PointOnB.x(), -PointOnB.y(), PointOnB.z()), 5.0f, true, ColorRGBA(color.x(), color.y(), color.z(), 1.0f));
            Renderer::getInstance()->addLine(Vector3(PointOnB.x(), -PointOnB.y(), PointOnB.z()), Vector3(PointOnB.x(), -PointOnB.y(), PointOnB.z()) + Vector3(normalOnB.x(), -normalOnB.y(), normalOnB.z()) * distance, true, ColorRGBA(color.x(), color.y(), color.z(), 1.0f));
        }
    }
    virtual void reportErrorWarning(const char* warningString)
    {
        Log::log(eLOG_FAILURE, warningString);
    }

    virtual void draw3dText(const btVector3& location,const char* textString)
    {
        if (textString && location.getX())
        {
        }
    }

    virtual void setDebugMode(int debugMode)
    {
        this->debugMode = debugMode;
    }

    virtual int getDebugMode() const
    {
        return debugMode;
    }
};

}

#endif // MAGIC3D_RENDERER_H
