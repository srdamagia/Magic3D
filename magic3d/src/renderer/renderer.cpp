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

#if !defined(MAGIC3D_LEGACY)
#include <magic3d/renderer/opengl/renderer_shaders.h>
#else
#include <magic3d/renderer/opengl/renderer_fixedfunction.h>
#endif

#include <magic3d/renderer/window_internal.h>
#if defined(__WIN32__)
#include <magic3d/renderer/window_win.h>
#endif

int Magic3D::Renderer::vboCreated = 0;
int Magic3D::Renderer::vboDestroyed = 0;
Magic3D::Renderer* Magic3D::Renderer::instance = NULL;

Magic3D::ViewPort::ViewPort(Vector4 area, int width, int height)
{
    this->area = area;
    this->width = width;
    this->height = height;
    orthographic = NULL;
    perspective = NULL;

    parent = NULL;
    stereoscopy = NULL;
}

Magic3D::ViewPort::~ViewPort()
{
    if (parent)
    {
        parent->stereoscopy = NULL;
        parent = NULL;
    }

    if (stereoscopy)
    {
        stereoscopy->setParent(NULL);
    }
}

void Magic3D::ViewPort::setArea(Vector4 area)
{
    this->area = area;
}

void Magic3D::ViewPort::setArea(float x, float y, float z, float w)
{
    this->area.setX(x);
    this->area.setY(y);
    this->area.setZ(z);
    this->area.setW(w);
}

const Magic3D::Vector4& Magic3D::ViewPort::getArea()
{
    return area;
}

void Magic3D::ViewPort::setOrthographic(Camera* orthographic)
{
    this->orthographic = orthographic;

    updateStereoscopy();
}

Magic3D::Camera* Magic3D::ViewPort::getOrthographic()
{
    return orthographic;
}

void Magic3D::ViewPort::setPerspective(Camera* perspective)
{
    this->perspective = perspective;

    updateStereoscopy();
}

Magic3D::Camera* Magic3D::ViewPort::getPerspective()
{
    return perspective;
}

void Magic3D::ViewPort::setWidth(int width)
{
    this->width = width;

    updateStereoscopy();
}

int Magic3D::ViewPort::getWidth()
{
    return width;
}

void Magic3D::ViewPort::setHeight(int height)
{
    this->height = height;

    updateStereoscopy();
}

int Magic3D::ViewPort::getHeight()
{
    return height;
}

void Magic3D::ViewPort::setParent(ViewPort* parent)
{
    if (this->parent)
    {
        this->parent->stereoscopy = NULL;
    }
    this->parent = parent;
    if (this->parent)
    {
        this->parent->stereoscopy = this;
        this->parent->updateStereoscopy();
    }
}

Magic3D::ViewPort* Magic3D::ViewPort::getParent()
{
    return parent;
}

void Magic3D::ViewPort::updateStereoscopy()
{
    if (stereoscopy)
    {
        stereoscopy->width = width;
        stereoscopy->height = height;
        stereoscopy->perspective = perspective;
        stereoscopy->orthographic = orthographic;
    }
}

//******************************************************************************

Magic3D::Renderer::Renderer()
{
    lblFPS = NULL;
    lblINFO = NULL;
    lblCURSOR = NULL;

    window = NULL;

    grid2D = Vector3(0.0f, 0.0f, 0.0f);
    grid3D = Vector3(0.0f, 0.0f, 0.0f);

    lastTexture0 = 0;
    lastTexture1 = 0;
    lastBlend = eBLEND_COUNT;

    currentViewPort    = 0;
    rendererObjects    = 0;
    rendererTriangles  = 0;
    rendererDrawCalls  = 0;
    rendererTexChanges = 0;
    rendererShadows    = 0;
    rendererReflective = 0;

    version            = 0;

    shadowsMS     = 0.0f;
    reflectionsMS = 0.0f;
    defaultMS     = 0.0f;
    fboMS         = 0.0f;
    screenMS      = 0.0f;
    spritesMS     = 0.0f;
    gizmosMS      = 0.0f;

    extVBO             = false;
    extFBO             = false;
    extMapBuffer       = false;
    extDepthTexture    = false;
    showGizmos         = false;
    showGizmosPhysics  = false;
    showOctree         = false;
    showOctreeObjects  = false;
    screenEffects      = false;

    mode = eRENDER_MODE_SOLID;

    debugMode = DBG_DrawWireframe | /*DBG_DrawAabb |*/ DBG_DrawContactPoints | DBG_NoHelpText | DBG_DrawConstraints | DBG_DrawConstraintLimits;

    debugLines[0]  = new MeshData(eMESH_TRIANGLES, "debugLines3D");
    debugPoints[0] = new MeshData(eMESH_TRIANGLES, "debugPoints3D");
    debugLines[1]  = new MeshData(eMESH_TRIANGLES, "debugLines2D");
    debugPoints[1] = new MeshData(eMESH_TRIANGLES, "debugPoints2D");

    int vcount = Magic3D::getInstance()->getConfiguration().SCREEN_VERTICES;
    float size = 1.0f / (float)(vcount - 1);
    float sizeVertex = size * 2.0f;
    float sizeUV = size;
    float start = (float)(vcount - 1) * 0.5f * sizeVertex;
    for (int r = 0; r < vcount; r++)
    {
        for (int c = 0; c < vcount; c++)
        {
            PlaneVertex vertex;
            vertex.vertex = Vector3((-start) + (float)c * sizeVertex, start - (float)r * sizeVertex, 0.0f);
            vertex.uv = Texture2D((float)c * sizeUV, 1.0f - (float)r * sizeUV);
            posteffectsVertices.push_back(vertex);
        }
    }

    MeshData::createPlaneStrip(&posteffectsTriangles, &posteffectsLines, vcount, vcount);
}

Magic3D::Renderer::~Renderer()
{
    removeAllViewPorts();
}

bool Magic3D::Renderer::start()
{
#if !defined(MAGIC3D_LEGACY)
    RendererOpenGL_Shaders::start();
#else
    RendererOpenGL_FixedFunction::start();
#endif

    return instance;
}

void Magic3D::Renderer::finishDebug()
{
    if (debugLines[0])
    {
        delete debugLines[0];
        debugLines[0] = NULL;
    }

    if (debugPoints[0])
    {
        delete debugPoints[0];
        debugPoints[0] = NULL;
    }

    if (debugLines[1])
    {
        delete debugLines[1];
        debugLines[1] = NULL;
    }

    if (debugPoints[1])
    {
        delete debugPoints[1];
        debugPoints[1] = NULL;
    }
}
bool Magic3D::Renderer::finish()
{
    bool result = true;

    if (getInstance()->window)
    {
        getInstance()->window->finish();
        delete getInstance()->window;
        getInstance()->window = NULL;
    }

    if (instance)
    {
        instance->finishDebug();
        delete instance;
        instance = NULL;
        result = instance == NULL;
    }

    if (result)
    {
        Log::log(eLOG_SUCCESS, "Renderer sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Renderer not finished.");
    }

    return result;
}

Magic3D::Renderer* Magic3D::Renderer::getInstance()
{
    return instance;
}

int Magic3D::Renderer::getVersion()
{
    return version;
}

bool Magic3D::Renderer::hasMapBuffer()
{
    return extMapBuffer;
}

Magic3D::Window* Magic3D::Renderer::createWindow(bool internal)
{
    if (!window)
    {
        if (internal)
        {
            window = WindowInternal::getInstance();
        }
        else
        {
#if !defined(MAGIC3D_OES)
#if defined(__WIN32__)
            window = WindowWin::getInstance();
#endif
#endif
        }
        if (window)
        {
            window->start();
        }
    }

    return window;
}

Magic3D::Window* Magic3D::Renderer::getWindow()
{
    return getInstance()->window;
}

void Magic3D::Renderer::addViewPort(ViewPort* view)
{
    viewports.push_back(view);
}

void Magic3D::Renderer::removeAllViewPorts()
{
    while (!viewports.empty()) {
        ViewPort* viewport = *viewports.begin();
        viewports.erase(viewports.begin());
        if (viewport)
        {
            delete viewport;
        }
    }
}

std::vector<Magic3D::ViewPort*>* Magic3D::Renderer::getViewPorts()
{
    return &viewports;
}

Magic3D::ViewPort* Magic3D::Renderer::getCurrentViewPort()
{
    ViewPort* result = NULL;
    if (currentViewPort >= 0 && currentViewPort < (int)viewports.size())
    {
        result = viewports.at(currentViewPort);
    }

    return result;
}

Magic3D::ViewPort* Magic3D::Renderer::getViewPort(int index)
{
    ViewPort* result = NULL;
    if (index >= 0 && index < (int)viewports.size())
    {
        result = viewports.at(index);
    }

    return result;
}

float Magic3D::Renderer::getShadowsMS()
{
    return shadowsMS;
}

float Magic3D::Renderer::getReflectionsMS()
{
    return reflectionsMS;
}

float Magic3D::Renderer::getDefaultMS()
{
    return defaultMS;
}

float Magic3D::Renderer::getFboMS()
{
    return fboMS;
}

float Magic3D::Renderer::getScreenMS()
{
    return screenMS;
}

float Magic3D::Renderer::getSpritesMS()
{
    return spritesMS;
}

float Magic3D::Renderer::getGizmosMS()
{
    return gizmosMS;
}

int Magic3D::Renderer::getObjectsCount()
{
    return rendererObjects;
}

int Magic3D::Renderer::getShadowsCount()
{
    return rendererShadows;
}

int Magic3D::Renderer::getReflectiveCount()
{
    return rendererReflective;
}

int Magic3D::Renderer::getTrianglesCount()
{
    return rendererTriangles;
}

int Magic3D::Renderer::getDrawCallsCount()
{
    return rendererDrawCalls;
}

int Magic3D::Renderer::getTexChangesCount()
{
    return rendererTexChanges;
}

int Magic3D::Renderer::getVBOCount()
{
    return vboCreated - vboDestroyed;
}

Magic3D::RENDER_MODE Magic3D::Renderer::getRenderMode()
{
    return mode;
}

void Magic3D::Renderer::setRenderMode(RENDER_MODE mode)
{
    this->mode = mode;
}

Magic3D::Vector3& Magic3D::Renderer::getGrid2D()
{
    return grid2D;
}

void Magic3D::Renderer::setGrid2D(Vector3 grid)
{
    grid2D = grid;
}

Magic3D::Vector3& Magic3D::Renderer::getGrid3D()
{
    return grid3D;
}

void Magic3D::Renderer::setGrid3D(Vector3 grid)
{
    grid3D = grid;
}

bool Magic3D::Renderer::isShowingGizmos()
{
    return showGizmos;
}

void Magic3D::Renderer::setShowGizmos(bool show)
{
    showGizmos = show;
}

bool Magic3D::Renderer::isShowingGizmosPhysics()
{
    return showGizmosPhysics;
}

void Magic3D::Renderer::setShowGizmosPhysics(bool show)
{
    showGizmosPhysics = show;
}

bool Magic3D::Renderer::isShowingOctree()
{
    return showOctree;
}

void Magic3D::Renderer::setShowOctree(bool show)
{
    showOctree = show;
}

bool Magic3D::Renderer::isShowingOctreeObjects()
{
    return showOctreeObjects;
}

void Magic3D::Renderer::setShowOctreeObjects(bool show)
{
    showOctreeObjects = show;
}

bool Magic3D::Renderer::isRenderingScreenEffects()
{
    return getRenderMode() == eRENDER_MODE_DEPTH || (screenEffects && (getRenderMode() == eRENDER_MODE_SOLID || getRenderMode() == eRENDER_MODE_SCREEN_WIREFRAME));
}

bool Magic3D::Renderer::isUsingScreenEffects()
{
    return screenEffects;
}

void Magic3D::Renderer::setUsingScreenEffects(bool effects)
{
    screenEffects = effects;
}

float Magic3D::Renderer::getScreenAspectX(bool invert)
{
    float result;

    if (invert)
    {
        result = getWindow()->getWidth() > getWindow()->getHeight() ? 1.0f : (float)getWindow()->getHeight() / (float)getWindow()->getWidth();
    }
    else
    {
        result = getWindow()->getWidth() > getWindow()->getHeight() ? 1.0f : (float)getWindow()->getWidth() / (float)getWindow()->getHeight();
    }

    return result;
}

float Magic3D::Renderer::getScreenAspectY(bool invert)
{
    float result;

    if (invert)
    {
        result = getWindow()->getWidth() > getWindow()->getHeight() ? (float)getWindow()->getWidth() / (float)getWindow()->getHeight() : 1.0f;
    }
    else
    {
        result = getWindow()->getWidth() > getWindow()->getHeight() ? (float)getWindow()->getHeight() / (float)getWindow()->getWidth() : 1.0f;
    }

    return result;
}

void Magic3D::Renderer::addDistortion(Vector3 position, DISTORTION type, float radius, float frequency, float factor, float wave)
{
    ViewPort* view = getCurrentViewPort();
    if (!Magic3D::getInstance()->getConfiguration().STEREOSCOPY && view && view->getPerspective())
    {
        Camera* camera = view->getPerspective();

        Vector3 pos = camera->getPosition2D(position, view);
        Vector3 radiusPos = camera->getPosition2D(position + camera->getView().getCol1().getXYZ() * radius, view);
        Vector3 tmpPos;
        
        float ax = getScreenAspectX(false);
        float ay = getScreenAspectY(false);

        Vector3 factors = Vector3(2.0f * ax, 2.0f * ay, 1.0f);

        tmpPos = mulPerElem(pos, factors);

        pos.setX(-1.0f + tmpPos.getX());
        pos.setY(-1.0f + (ay * 2.0f - tmpPos.getY()));

        tmpPos = mulPerElem(radiusPos, factors);

        radiusPos.setX(-1.0f + tmpPos.getX());
        radiusPos.setY(-1.0f + (ay * 2.0f - tmpPos.getY()));

        Vector3 aspect = Vector3(0.0f, 0.0f, 0.0f);
        switch (getWindow()->getOrientation())
        {
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                aspect = Vector3((1.0f - ax) * -2.0f, (1.0f - ay) * -2.0f, 0.0f);
                pos = (Matrix4(Quaternion::rotationZ(Math::radians(180.0f)), aspect) * Vector4(pos, 1.0f)).getXYZ();
                radiusPos = (Matrix4(Quaternion::rotationZ(Math::radians(180.0f)), aspect) * Vector4(radiusPos, 1.0f)).getXYZ();
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            {
                pos = (Matrix4::rotationZ(Math::radians(-90.0f)) * Vector4(pos, 1.0f)).getXYZ();
                radiusPos = (Matrix4::rotationZ(Math::radians(-90.0f)) * Vector4(radiusPos, 1.0f)).getXYZ();
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                aspect = Vector3((1.0f - ay) * -2.0f, (1.0f - ax) * -2.0f, 0.0f);
                pos = (Matrix4(Quaternion::rotationZ(Math::radians(90.0f)), aspect) * Vector4(pos, 1.0f)).getXYZ();
                radiusPos = (Matrix4(Quaternion::rotationZ(Math::radians(90.0f)), aspect) * Vector4(radiusPos, 1.0f)).getXYZ();
                break;
            }
            default:
            {
            }
        }

        float finalRadius = Math::limitRange(-1.0f, dist(Point3(pos), Point3(radiusPos)), 1.0f);
        pos.setZ(finalRadius);
        Vector4 params = Vector4((float)type, frequency, factor, wave);

        distortions.push_back(Vector4(pos, 1.0f));
        distortions.push_back(params);
    }
}

void Magic3D::Renderer::drawLine(Vector3 start, Vector3 finish, bool orthographic, ColorRGBA color)
{
    if (orthographic)
    {
        debugLines[1]->addLine(start, finish, color);
    }
    else
    {
        debugLines[0]->addLine(start, finish, color);
    }
}

void Magic3D::Renderer::drawPoint(Vector3 position, float size, bool orthographic, ColorRGBA color)
{
    if (orthographic)
    {
        debugPoints[1]->addPoint(position, size, color);
    }
    else
    {
        debugPoints[0]->addPoint(position, size, color);
    }
}

bool Magic3D::Renderer::isProfileObject(Object* object)
{
    return object == lblFPS || object == lblINFO || object == lblCURSOR;
}

Magic3D::STEREOSCOPY Magic3D::Renderer::getEye(ViewPort* view)
{
    STEREOSCOPY eye = eSTEREOSCOPY_CENTER;

    if (Magic3D::getInstance()->getConfiguration().STEREOSCOPY)
    {
        if (view->getParent())
        {
            eye = eSTEREOSCOPY_RIGHT;
        }
        else
        {
            eye = eSTEREOSCOPY_LEFT;
        }
    }

    return eye;
}
