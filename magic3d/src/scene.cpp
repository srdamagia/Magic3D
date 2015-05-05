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

Magic3D::Scene* Magic3D::Scene::instance = NULL;

bool SortZOrder(Magic3D::RenderObject& lo, Magic3D::RenderObject& ro)
{
    return lo.object->getZOrderFactor() < ro.object->getZOrderFactor();
}

Magic3D::Layer::Layer(std::string name)
{
    this->name = name;

    visible = true;
}

Magic3D::Layer::~Layer()
{
    lights.clear();
    objects2D.clear();
    objects3D.clear();
}

void Magic3D::Layer::setName(std::string name)
{
    this->name = name;
}

const std::string& Magic3D::Layer::getName()
{
    return name;
}

void Magic3D::Layer::addObject(Object* object)
{
    if (object)
    {
        object->setLayer(this);
        if (object->getType() == eOBJECT_LIGHT)
        {
            lights.push_back(object);
        }
        else
        {
            switch (object->getRender())
            {
                case eRENDER_2D: objects2D.push_back(object); break;
                case eRENDER_3D: objects3D.push_back(object); break;
            }
        }
    }
}

void Magic3D::Layer::removeObject(std::string name)
{
    removeObject(ResourceManager::getObjects()->get(name));
}

void Magic3D::Layer::removeObject(Object* object)
{
    if (object)
    {
        if (object->getType() == eOBJECT_LIGHT)
        {
            removeObjectFromList(object->getName(), &lights);
        }
        else
        {
            switch (object->getRender())
            {
                case eRENDER_2D: removeObjectFromList(object->getName(), &objects2D); break;
                case eRENDER_3D: removeObjectFromList(object->getName(), &objects3D); break;
            }
        }
    }
}

int Magic3D::Layer::getObjectIndex(std::string name, std::vector<Object*>* list)
{
    int result = -1;
    std::vector<Object*>::const_iterator it_o = list->begin();
    int i = 0;
    while (it_o != list->end())
    {
        Object* object = *it_o++;

        if (object->getName().compare(name) == 0)
        {
            result = i;
            break;
        }

        i++;
    }

    return result;
}

void Magic3D::Layer::removeObjectFromList(std::string name, std::vector<Object*>* objects)
{
    std::vector<Object*>::const_iterator it_o = objects->begin();

    int index = 0;
    while (it_o != objects->end())
    {
        Object* object = *it_o++;

        if (object->getName().compare(name) == 0)
        {
            object->setLayer(NULL);
            objects->erase(objects->begin() + index);
            break;
        }

        index++;
    }
}

void Magic3D::Layer::clearList(std::vector<Object*>* objects)
{
    std::vector<Object*>::const_iterator it_o = objects->begin();

    int index = 0;
    while (it_o != objects->end())
    {
        Object* object = *it_o++;

        object->setLayer(NULL);
        index++;
    }

    objects->clear();
}

void Magic3D::Layer::clear()
{
    clearList(&lights);
    clearList(&objects2D);
    clearList(&objects3D);
}

std::vector<Magic3D::Object*>* Magic3D::Layer::getLights()
{
    return &lights;
}

std::vector<Magic3D::Object*>* Magic3D::Layer::getObjects2D()
{
    return &objects2D;
}

std::vector<Magic3D::Object*>* Magic3D::Layer::getObjects3D()
{
    return &objects3D;
}

void Magic3D::Layer::setVisible(bool visible)
{
    this->visible = visible;
}

bool Magic3D::Layer::isVisible()
{
    return visible;
}


//************************************************************************************
Magic3D::Scene::Scene()
{
    octree = new Octree();
    addLayer(new Layer(M3D_MAIN_LAYER_NAME));

    oldColorClear   = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    colorClear   = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    colorAmbient = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    fogColor     = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
    fogStart     = 0.0f;
    fogFinish    = 0.0f;
    volumeMaster = 1.0f;
    gravity      = Vector3(0.0, -10.0f, 0.0f);
    wind         = Vector3(0.0, 0.0f, 0.0f);

    fboCamera    = NULL;
    shadowsLight = NULL;

    showFPS    = Magic3D::getInstance()->getConfiguration().SHOW_FPS;
    showINFO   = Magic3D::getInstance()->getConfiguration().SHOW_INFORMATION;
    showWINDOW = Magic3D::getInstance()->getConfiguration().SHOW_COORDINATES;

    lblLoading = NULL;
    sptLoading = NULL;
    camLoading = new Camera("m3d_scene_cam_loading");
    camLoading->setProjectionType(ePROJECTION_ORTHOGRAPHIC);
    camLoading->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));

    timeLoading = 0.0f;
    loading = false;
    scripted = false;
    additive = false;
    uniqueUpdate = true;

    activeLights = false;

    doc             = NULL;
    sceneXML        = NULL;
    layerXML        = NULL;
    objectXML       = NULL;
    currentLayerXML = NULL;
    modelXML        = NULL;

    frustumMS       = 0.0f;
    scriptMS        = 0.0f;
}

Magic3D::Scene::~Scene()
{
    clear(false, false);

    if (camLoading)
    {
        delete camLoading;
        camLoading = NULL;
    }

    while (!layers.empty()) {
        Layer* layer = *layers.begin();
        layers.erase(layers.begin());
        if (layer)
        {
            delete layer;
        }
    }

    if (octree)
    {
        delete octree;
        octree = NULL;
    }
}

bool Magic3D::Scene::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new Scene();
    }

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Scene sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Scene not started.");
    }

    return result;
}

bool Magic3D::Scene::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Scene sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Scene not finished.");
    }

    return !instance;
}

Magic3D::Scene* Magic3D::Scene::getInstance()
{
    return instance;
}

void Magic3D::Scene::addLayer(Layer* layer)
{
    if (layer)
    {
        layers.push_back(layer);
    }
}

void Magic3D::Scene::removeLayer(std::string name)
{
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    int index = 0;
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer->getName().compare(name) == 0)
        {
            layers.erase(layers.begin() + index);
            delete layer;
            break;
        }

        index++;
    }
}

Magic3D::Layer* Magic3D::Scene::getLayer(std::string name)
{
    Layer* result = NULL;
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer->getName().compare(name) == 0)
        {
            result = layer;
            break;
        }
    }

    return result;
}

int Magic3D::Scene::getLayerIndex(std::string name)
{
    int result = -1;
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    int i = 0;
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer->getName().compare(name) == 0)
        {
            result = i;
            break;
        }

        i++;
    }

    return result;
}

std::vector<Magic3D::Layer*>* Magic3D::Scene::getLayers()
{
    return &layers;
}

Magic3D::Octree* Magic3D::Scene::getOctree()
{
    return octree;
}

std::list<Magic3D::Object*>* Magic3D::Scene::getVisibleLights(Camera* camera)
{
    lights.clear();

    activeLights = false;
    shadowsLight = NULL;

    if (!loading)
    {
        std::vector<Layer*>::const_iterator it_l = layers.begin();
        while (it_l != layers.end())
        {
            Layer* layer = *it_l++;

            if (layer && layer->isVisible())
            {
                std::vector<Object*>* objects = layer->getLights();

                std::vector<Object*>::const_iterator it_o = objects->begin();
                while (it_o != objects->end())
                {
                    Object* object = *it_o++;
                    Light* light = NULL;

                    if (object)
                    {
                        light = static_cast<Light*>(object);
                        object->setInFrustum(false);
                    }

                    if (object && object->isVisible())
                    {
                        Matrix4 model = light->getMatrixFromParent();
                        Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
                        Matrix4 modelView = Matrix4::identity();

                        bool add = false;
                        if (camera)
                        {
                            /*Object* bounding = object;

                            if (light->getOccluder())
                            {
                                bounding = light->getOccluder();
                            }
                            else
                            {
                                add = true;
                            }*/

                            camera->updateFrustum(model, modelView);
                            if (object->isInEffectFrustum() || camera->frustumSphere(pos, light->getFar(), false))
                            {
                                light->setFrustumLensFlare(camera->frustumVertex(pos, true, true, true));
                                object->setInFrustum(true);
                                add = true;
                            }
                        }
                        else
                        {
                            add = true;
                        }

                        if (add)
                        {
                            if (light && light->isEnabled())
                            {
                                activeLights = true;
                                light->updateRenderMatrix(camera);
                            }
                            //light->setRenderMatrix(modelView);

                            if (light->isShadows())
                            {
                                if (!shadowsLight || lights.empty())
                                {
                                    shadowsLight = light;
                                }
                                else
                                {
                                    Vector3 cameraPos = camera->getMatrixFromParent().getTranslation();
                                    Point3 lightBorder = Point3(model.getTranslation() + normalize(cameraPos - model.getTranslation()) * light->getFar());
                                    Vector3 currentPos = shadowsLight->getMatrixFromParent().getTranslation();
                                    Point3 shadowBorder = Point3(currentPos + normalize(cameraPos - currentPos) * shadowsLight->getFar());

                                    float distCurrent       = dist(Point3(currentPos),             Point3(cameraPos));
                                    float distCurrentBorder = dist(Point3(shadowBorder),           Point3(cameraPos));

                                    float distLight         = dist(Point3(model.getTranslation()), Point3(cameraPos));
                                    float distLightBorder   = dist(Point3(lightBorder),            Point3(cameraPos));

                                    float minLight = Math::min(distLight, distLightBorder);
                                    float minShadow = Math::min(distCurrent, distCurrentBorder);

                                    if (minLight < minShadow)
                                    {
                                        shadowsLight = light;
                                    }
                                }
                            }
                            lights.push_back(object);
                        }
                    }
                }
            }
        }
    }

    if (shadowsLight && !shadowsLight->isEnabled())
    {
        activeLights = true;
        shadowsLight->updateRenderMatrix(camera);
    }

    return &lights;
}

void Magic3D::Scene::updateVisibleObjects2D(Camera* camera, bool profile)
{
    objects2D.clear();

    if (!loading)
    {
        std::vector<Layer*>::const_iterator it_l = layers.begin();
        while (it_l != layers.end())
        {
            Layer* layer = *it_l++;

            if (layer && layer->isVisible())
            {
                std::vector<Object*>* objects = layer->getObjects2D();

                std::vector<Object*>::const_iterator it_o = objects->begin();
                while (it_o != objects->end())
                {
                    Object* object = *it_o++;

                    if (object)
                    {
                        object->setInFrustum(false);
                    }

                    if (object && object->isVisible())
                    {
                        Box box = object->getBoundingBox();
                        Vector3 pos = box.getCenter();
                        Matrix4 model = object->getMatrixFromParent();
                        Matrix4 modelView = Matrix4::identity();

                        if (camera)
                        {
                            camera->updateFrustum(model, modelView);
                            if (object->isInEffectFrustum() || camera->frustumBox(pos, box.getWidth(), box.getHeight(), box.getDepth(), true))
                            {
                                object->setInFrustum(true);
                                objects2D.push_back(RenderObject(object, modelView, model));
                            }
                        }
                        else
                        {
                            objects2D.push_back(RenderObject(object, modelView, model));
                        }
                    }
                }
            }
        }

        std::list<Object*>::const_iterator it_li = lights.begin();
        while (it_li != lights.end())
        {
            Light* light = static_cast<Light*>(*it_li++);
            if (light->isFrustumLensFlare() && (light->isShowingFlare() || light->isShowingLens()))
            {
                Matrix4 viewMatrix = light->getMatrixFromParent();
                objects2D.push_back(RenderObject(light, viewMatrix, viewMatrix));
            }
        }
    }

    if (profile)
    {
        Renderer* renderer = Renderer::getInstance();
        if (showFPS && renderer->lblFPS)
        {
            Matrix4 viewMatrix = camera ? camera->getView() * renderer->lblFPS->getMatrixFromParent() : renderer->lblFPS->getMatrixFromParent();
            if (!renderer->lblFPS->getParent())
            {
                renderer->lblFPS->setParent(renderer->getCurrentViewPort()->getOrthographic());
            }
            objects2D.push_back(RenderObject(renderer->lblFPS, viewMatrix, viewMatrix));
        }

        if (showINFO && renderer->lblINFO)
        {
            Matrix4 viewMatrix = camera ? camera->getView() * renderer->lblINFO->getMatrixFromParent() : renderer->lblINFO->getMatrixFromParent();
            if (!renderer->lblINFO->getParent())
            {
                renderer->lblINFO->setParent(renderer->getCurrentViewPort()->getOrthographic());
            }
            objects2D.push_back(RenderObject(renderer->lblINFO, viewMatrix, viewMatrix));
        }

        if (showWINDOW && renderer->lblCURSOR)
        {
            Matrix4 viewMatrix = camera ? camera->getView() * renderer->lblCURSOR->getMatrixFromParent() : renderer->lblCURSOR->getMatrixFromParent();
            if (!renderer->lblCURSOR->getParent())
            {
                renderer->lblCURSOR->setParent(renderer->getCurrentViewPort()->getOrthographic());
            }
            objects2D.push_back(RenderObject(renderer->lblCURSOR, viewMatrix, viewMatrix));
        }
    }

    if (loading)
    {
        if (lblLoading)
        {
            Matrix4 viewMatrix = camera ? camera->getView() * lblLoading->getMatrixFromParent() : lblLoading->getMatrixFromParent();
            objects2D.push_back(RenderObject(lblLoading, viewMatrix, viewMatrix));
        }

        if (sptLoading)
        {
            Matrix4 viewMatrix = camera ? camera->getView() * sptLoading->getMatrixFromParent() : sptLoading->getMatrixFromParent();
            objects2D.push_back(RenderObject(sptLoading, viewMatrix, viewMatrix));
        }
    }
}

void Magic3D::Scene::prepareUpdate(Camera* camera, bool reflectives, bool reflections, bool anyType)
{
    if (reflections)
    {
        this->reflections.clear();
        reflectionsZOrder.clear();
    }
    else
    {
        objects3D.clear();
        sounds.clear();
        shadows.clear();
        if (reflectives)
        {
            this->reflectives.clear();
        }
        glow.clear();
        zorder.clear();
    }
}

void Magic3D::Scene::updateVisibleObjects3D(Camera* camera, bool reflectives, bool reflections, bool anyType)
{
    prepareUpdate(camera, reflectives, reflections, anyType);

    if (loading)
    {
        return;
    }

    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer && layer->isVisible())
        {
            std::vector<Object*>* objects = layer->getObjects3D();

            std::vector<Object*>::const_iterator it_o = objects->begin();
            while (it_o != objects->end())
            {
                Object* object = *it_o++;

                if (object && !reflections)
                {
                    object->setInFrustum(false);
                }

                if (object && object->isVisible() && object->getType() != eOBJECT_CAMERA)
                {
                    Box box = object->getBoundingBox();
                    Vector3 pos = box.getCenter();
                    Matrix4 model = object->getMatrixFromParent();
                    Matrix4 modelView = Matrix4::identity();

                    bool add = false;
                    if (camera)
                    {
                        camera->updateFrustum(model, modelView);
                        if (object->isInEffectFrustum() || camera->frustumBox(pos, box.getWidth(), box.getHeight(), box.getDepth(), true))
                        {
                            if (!reflections)
                            {
                                object->setInFrustum(true);
                            }
                            add = true;
                        }
                    }
                    else
                    {
                        add = true;
                    }

                    if (add)
                    {
                        if (anyType)
                        {
                            object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                            objects3D.push_back(RenderObject(object, modelView, model));
                        }
                        else
                        {
                            switch (object->getType())
                            {
                                case eOBJECT_SOUND:
                                {
                                    if (!reflections)
                                    {
                                        sounds.push_back(object);
                                    }
                                    break;
                                }
                                default:
                                {
                                    if (!reflections)
                                    {
                                        if (object->isShadowed())
                                        {
                                            shadows.push_back(RenderObject(object, modelView, model));
                                        }
                                        if (object->isGlowed())
                                        {
                                            glow.push_back(RenderObject(object, modelView, model));
                                        }
                                    }
                                    if (object->isReflective())
                                    {
                                        if (reflectives)
                                        {
                                            this->reflectives.push_back(RenderObject(object, modelView, model));
                                        }
                                    }
                                    else
                                    {
                                        if (object->isZOrder())
                                        {
                                            if (reflections)
                                            {
                                                object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                                                reflectionsZOrder.push_back(RenderObject(object, modelView, model));
                                            }
                                            else
                                            {
                                                object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                                                zorder.push_back(RenderObject(object, modelView, model));
                                            }
                                        }
                                        else
                                        {
                                            if (reflections)
                                            {
                                                this->reflections.push_back(RenderObject(object, modelView, model));
                                            }
                                            else
                                            {
                                                objects3D.push_back(RenderObject(object, modelView, model));
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (anyType)
    {
        getVisibleLights(camera);
        std::list<Object*>::const_iterator it_li = lights.begin();
        while (it_li != lights.end())
        {
            Light* light = static_cast<Light*>(*it_li++);
            if (light->isInFrustum())
            {
                Matrix4 viewMatrix = light->getMatrixFromParent();
                objects3D.push_back(RenderObject(light, viewMatrix, viewMatrix));
            }
        }
        objects3D.sort(SortZOrder);
    }
    else
    {
        if (reflections)
        {
            reflectionsZOrder.sort(SortZOrder);
        }
        else
        {
            zorder.sort(SortZOrder);
        }
    }
}

void Magic3D::Scene::updateVisibleObjectsOctree(Camera* camera, bool reflectives, bool reflections, bool anyType)
{
    prepareUpdate(camera, reflectives, reflections, anyType);

    if (loading || !camera)
    {
        return;
    }

    updateVisibleObjectsFromOctreeNode(getOctree(), camera, reflectives, reflections, anyType);

    if (anyType)
    {
        getVisibleLights(camera);
        std::list<Object*>::const_iterator it_li = lights.begin();
        while (it_li != lights.end())
        {
            Light* light = static_cast<Light*>(*it_li++);
            if (light->isInFrustum())
            {
                Matrix4 viewMatrix = light->getMatrixFromParent();
                objects3D.push_back(RenderObject(light, viewMatrix, viewMatrix));
            }
        }
        objects3D.sort(SortZOrder);
    }
    else
    {
        if (reflections)
        {
            reflectionsZOrder.sort(SortZOrder);
        }
        else
        {
            zorder.sort(SortZOrder);
        }
    }
}

void Magic3D::Scene::updateVisibleObjectsFromOctreeNode(Octree* octree, Camera* camera, bool reflectives, bool reflections, bool anyType)
{
    Matrix4 model = Matrix4::identity();
    Matrix4 modelView = Matrix4::identity();
    camera->updateFrustum(model, modelView);

    Box box = octree->getBoundingBox();
    Vector3 pos = box.getCenter();
    bool inFrustum = camera->frustumBox(pos, box.getWidth(), box.getHeight(), box.getDepth(), true);

    if (inFrustum)
    {
        octree->setInFrustum(true);
        const std::list<Object*>* objects = octree->getObjects();
        std::list<Object*>::const_iterator it_o = objects->begin();
        while (it_o != objects->end())
        {
            Object* object = *it_o++;
            Layer* layer = object->getLayer();

            if (layer && layer->isVisible())
            {
                if (object && !reflections)
                {
                    object->setInFrustum(false);
                }

                if (object && object->isVisible() && object->getType() != eOBJECT_CAMERA)
                {
                    model = object->getMatrixFromParent();

                    bool add = false;
                    if (camera)
                    {
                        modelView = camera->getView() * model;
                        if (!reflections)
                        {
                            object->setInFrustum(true);
                        }
                        add = true;
                    }
                    else
                    {
                        add = true;
                    }

                    if (add)
                    {
                        if (anyType)
                        {
                            object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                            objects3D.push_back(RenderObject(object, modelView, model));
                        }
                        else
                        {
                            switch (object->getType())
                            {
                                case eOBJECT_LIGHT:
                                {
                                    break;
                                }
                                case eOBJECT_SOUND:
                                {
                                    if (!reflections)
                                    {
                                        sounds.push_back(object);
                                    }
                                    break;
                                }
                                default:
                                {
                                    if (!reflections)
                                    {
                                        if (object->isShadowed())
                                        {
                                            shadows.push_back(RenderObject(object, modelView, model));
                                        }
                                        if (object->isGlowed())
                                        {
                                            glow.push_back(RenderObject(object, modelView, model));
                                        }
                                    }
                                    if (object->isReflective())
                                    {
                                        if (reflectives)
                                        {
                                            this->reflectives.push_back(RenderObject(object, modelView, model));
                                        }
                                    }
                                    else
                                    {
                                        if (object->isZOrder())
                                        {
                                            if (reflections)
                                            {
                                                object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                                                reflectionsZOrder.push_back(RenderObject(object, modelView, model));
                                            }
                                            else
                                            {
                                                object->setZOrderFactor(camera->getBoxMinZ(model, object->getBoundingBox()));
                                                zorder.push_back(RenderObject(object, modelView, model));
                                            }
                                        }
                                        else
                                        {
                                            if (reflections)
                                            {
                                                this->reflections.push_back(RenderObject(object, modelView, model));
                                            }
                                            else
                                            {
                                                objects3D.push_back(RenderObject(object, modelView, model));
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int flags = octree->getActiveNodes(), index = 0; flags > 0; flags >>= 1, index++)
        {
            if ((flags & 1) == 1)
            {
                updateVisibleObjectsFromOctreeNode(octree->getChild(index), camera, reflectives, reflections, anyType);
            }
        }
    }
    else
    {
        octree->resetChildFrustum();
    }
}

void Magic3D::Scene::updateFrustumCulling(Camera* camera2D, Camera* camera3D)
{
    Magic3D::getInstance()->setTimeReference();
    updateVisibleObjects2D(camera2D);
    updateVisibleObjectsOctree(camera3D);
    frustumMS = Magic3D::getInstance()->getTimeSinceReference();
}

float Magic3D::Scene::getFrustumMS()
{
    return frustumMS;
}

bool Magic3D::Scene::isActiveLights()
{
    return activeLights;
}

std::list<Magic3D::Object*>* Magic3D::Scene::getLights()
{
    return &lights;
}

std::list<Magic3D::Object*>* Magic3D::Scene::getSounds()
{
    return &sounds;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getVisibleObjects2D()
{
    return &objects2D;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getVisibleObjects3D()
{
    return &objects3D;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getShadows()
{
    return &shadows;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getReflectives()
{
    return &reflectives;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getReflections()
{
    return &reflections;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getReflectionsZOrder()
{
    return &reflectionsZOrder;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getGlows()
{
    return &glow;
}

std::list<Magic3D::RenderObject>* Magic3D::Scene::getZOrder()
{
    return &zorder;
}

float Magic3D::Scene::getScriptMS()
{
    return scriptMS;
}

void Magic3D::Scene::addScriptMS(float ms)
{
    scriptMS += ms;
}

bool Magic3D::Scene::update()
{
    uniqueUpdate = !uniqueUpdate;
    bool result = true;
    bool update = false;

    Renderer* renderer = Renderer::getInstance();

    if (showFPS)
    {
        if (!renderer->lblFPS)
        {
            Font* font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
            if (font)
            {
                renderer->lblFPS    = new GUILabel("m3d_scene_lbl_fps", 18);
                if (renderer->lblFPS->getMeshes()->at(0)->getMaterials()->size() > 0)
                {
                    renderer->lblFPS->getMeshes()->at(0)->getMaterials()->at(0)->setColorDiffuse(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
                }
                renderer->lblFPS->getMeshes()->at(0)->setIlluminated(false);
                renderer->lblFPS->setPosition(-594.0f, 2.0f);
                //renderer->lblFPS->setPosition(-92.0f, 2.0f);
                renderer->lblFPS->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
                renderer->lblFPS->setParent(Renderer::getInstance()->getCurrentViewPort()->getOrthographic());
            }
        }

        if (renderer->lblFPS)
        {
            sprintf(profileText, "FPS.....: % 07d - Time....: % 01.6f - Time sin: % 01.6f", Magic3D::Magic3D::getInstance()->getFPS(), Magic3D::Magic3D::getInstance()->getTime(), Magic3D::Magic3D::getInstance()->getTimeSin());
            //sprintf(profileText, "FPS: %4d", Magic3D::Magic3D::getInstance()->getFPS());

            renderer->lblFPS->setText(profileText);
            renderer->lblFPS->update();
        }
    }

    if (showINFO)
    {
        if (!renderer->lblINFO)
        {
            Font* font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
            if (font)
            {
                renderer->lblINFO    = new GUILabel("m3d_scene_lbl_info", 16);
                if (renderer->lblINFO->getMeshes()->at(0)->getMaterials()->size() > 0)
                {
                    renderer->lblINFO->getMeshes()->at(0)->getMaterials()->at(0)->setColorDiffuse(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
                }
                renderer->lblINFO->getMeshes()->at(0)->setIlluminated(false);
                //renderer->lblINFO->setPosition(-254.0f, 24.0f);
                renderer->lblINFO->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
                renderer->lblINFO->setParent(Renderer::getInstance()->getCurrentViewPort()->getOrthographic());
            }
        }

        if (renderer->lblINFO && renderer->lblINFO->isVisible())
        {
            Renderer* renderer = Renderer::getInstance();
            Magic3D* magic3D = Magic3D::getInstance();
            
            bool simple = false;
            
            if (simple)
            {
                renderer->lblINFO->setPosition(-246.0f, 24.0f);
                sprintf(profileText,
                        "Physics MS:      %11.6f\n"\
                        "Update MS:       %11.6f\n"\
                        "Script MS:       %11.6f\n"\
                        "Script Memory:   %8d Kb\n"\
                        "Frustum MS:      %11.6f\n"\
                        "Render MS:       %11.6f\n",
                        magic3D->getPhysicsMS(),
                        magic3D->getUpdateMS(),
                        getScriptMS(),
                        Script::getInstance()->getMemoryUsed(),
                        getFrustumMS(),
                        magic3D->getRenderMS());
            }
            else if (renderer->getWindow()->getHeight() >= 480)
            {
                renderer->lblINFO->setPosition(-246.0f, 24.0f);
                sprintf(profileText,
                        "Rendered:        %8d\n"\
                        "Objects:         %8d\n"\
                        "Shadowed:        %8d\n"\
                        "Reflective:      %8d\n"\
                        "Triangles:       %8d\n"\
                        "VBO:             %8d\n"\
                        "Bodies:          %8d\n"\
                        "Constraints:     %8d\n"\
                        "Draw Calls:      %8d\n"\
                        "Texture Changes: %8d\n"\
                        "Physics MS:      %11.6f\n"\
                        "Update MS:       %11.6f\n"\
                        "Script MS:       %11.6f\n"\
                        "Script Memory:   %8d Kb\n"\
                        "Frustum MS:      %11.6f\n"\
                        "Octree Nodes:    %8d\n"\
                        "Octree Rendered: %8d\n"\
                        "Render MS:       %11.6f\n"\
                        "    shadows      %11.6f\n"\
                        "    reflections  %11.6f\n"\
                        "    default      %11.6f\n"\
                        "    fbo          %11.6f\n"\
                        "    screen       %11.6f\n"\
                        "    sprites      %11.6f\n"\
                        "    gizmos       %11.6f",

                        renderer->getObjectsCount(),
                        getObjectCount(),
                        renderer->getShadowsCount(),
                        renderer->getReflectiveCount(),
                        renderer->getTrianglesCount(),
                        renderer->getVBOCount(),
                        Physics::getInstance()->getBodiesCount(),
                        Physics::getInstance()->getConstraintsCount(),
                        renderer->getDrawCallsCount(),
                        renderer->getTexChangesCount(),
                        magic3D->getPhysicsMS(),
                        magic3D->getUpdateMS(),
                        getScriptMS(),
                        Script::getInstance()->getMemoryUsed(),
                        getFrustumMS(),
                        getOctree()->getNodesCount(),
                        getOctree()->getRenderedNodes(),
                        magic3D->getRenderMS(),
                        renderer->getShadowsMS(),
                        renderer->getReflectionsMS(),
                        renderer->getDefaultMS(),
                        renderer->getFboMS(),
                        renderer->getScreenMS(),
                        renderer->getSpritesMS(),
                        renderer->getGizmosMS());
            }
            else
            {
                renderer->lblINFO->setPosition(-477.0f, 24.0f);
                sprintf(profileText,
                        "Rendered:        %8d  Render MS:       %11.6f\n"\
                        "Objects:         %8d      shadows      %11.6f\n"\
                        "Shadowed:        %8d      reflections  %11.6f\n"\
                        "Reflective:      %8d      default      %11.6f\n"\
                        "Triangles:       %8d      fbo          %11.6f\n"\
                        "VBO:             %8d      screen       %11.6f\n"\
                        "Bodies:          %8d      sprites      %11.6f\n"\
                        "Constraints:     %8d      gizmos       %11.6f\n"\
                        "Draw Calls:      %8d\n"\
                        "Texture Changes: %8d\n"\
                        "Physics MS:      %11.6f\n"\
                        "Update MS:       %11.6f\n"\
                        "Script MS:       %11.6f\n"\
                        "Script Memory:   %8d Kb\n"\
                        "Frustum MS:      %11.6f\n"\
                        "Octree Nodes:    %8d\n"\
                        "Octree Rendered: %8d\n",
                        
                        renderer->getObjectsCount(),                   magic3D->getRenderMS(),
                        getObjectCount(),                              renderer->getShadowsMS(),
                        renderer->getShadowsCount(),                   renderer->getReflectionsMS(),
                        renderer->getReflectiveCount(),                renderer->getDefaultMS(),
                        renderer->getTrianglesCount(),                 renderer->getFboMS(),
                        renderer->getVBOCount(),                       renderer->getScreenMS(),
                        Physics::getInstance()->getBodiesCount(),      renderer->getSpritesMS(),
                        Physics::getInstance()->getConstraintsCount(), renderer->getGizmosMS(),
                        renderer->getDrawCallsCount(),
                        renderer->getTexChangesCount(),
                        magic3D->getPhysicsMS(),
                        magic3D->getUpdateMS(),
                        getScriptMS(),
                        Script::getInstance()->getMemoryUsed(),
                        getFrustumMS(),
                        getOctree()->getNodesCount(),
                        getOctree()->getRenderedNodes());
            }
            
            renderer->lblINFO->setText(profileText);
            renderer->lblINFO->update();
        }
    }

    if (showWINDOW)
    {
        Window* w = Renderer::getInstance()->getWindow();

        if (!renderer->lblCURSOR)
        {
            Font* font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
            if (font)
            {
                renderer->lblCURSOR    = new GUILabel("m3d_scene_lbl_window", 18);
                if (renderer->lblCURSOR->getMeshes()->at(0)->getMaterials()->size() > 0)
                {
                    renderer->lblCURSOR->getMeshes()->at(0)->getMaterials()->at(0)->setColorDiffuse(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
                }
                renderer->lblCURSOR->getMeshes()->at(0)->setIlluminated(false);
                renderer->lblCURSOR->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
                renderer->lblCURSOR->setParent(Renderer::getInstance()->getCurrentViewPort()->getOrthographic());
            }
        }

        if (renderer->lblCURSOR && w)
        {
            sprintf(profileText, "x: %-4d - y: %-4d", w->getCursorX(), w->getCursorY());
            renderer->lblCURSOR->setText(profileText);
            renderer->lblCURSOR->setPosition(-(w->getWidth() - 4.0f), w->getHeight() - 22.0f);
            renderer->lblCURSOR->update();
        }
    }

    if (loading)
    {
        camLoading->update();
        if (!lblLoading)
        {
            Font* font = ResourceManager::getFonts()->get(M3D_DEFAULT_FONT);
            if (font)
            {
                lblLoading    = new GUILabel("m3d_scene_lbl_loading", 16);
                lblLoading->getMeshes()->at(0)->getMaterials()->clear();
                lblLoading->getMeshes()->at(0)->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_LOADING_FONT));
                lblLoading->getMeshes()->at(0)->setIlluminated(false);
                lblLoading->setPosition(0.0f, 0.0f);
                lblLoading->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
                lblLoading->setText("Loading...");
                lblLoading->setParent(Renderer::getInstance()->getCurrentViewPort()->getOrthographic());
            }
        }

        if (!sptLoading)
        {
            Texture* tex = ResourceManager::getTextures()->get(M3D_DEFAULT_PROGRESS_NAME);
            if (tex)
            {
                sptLoading = new Sprite("m3d_scene_spt_loading");
                sptLoading->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_LOADING_SPRITE));
                sptLoading->getMeshes()->at(0)->setIlluminated(false);
                sptLoading->setPosition(0.0f, 0.0f);
                sptLoading->setRotationEuler(Vector3(0.0f, 180.0f, 0.0f));
                sptLoading->setSize(32.0f, 32.0f);
                sptLoading->setParent(Renderer::getInstance()->getCurrentViewPort()->getOrthographic());
            }
        }

        Window* w = Renderer::getInstance()->getWindow();

        if (w && lblLoading && sptLoading)
        {
            lblLoading->setText("Loading...");
            lblLoading->setPosition(-125.0f, w->getHeight() - 22.0f);
            lblLoading->update();

            sptLoading->setPosition(-34.0f, w->getHeight() - 34.0f);

            timeLoading += Magic3D::getInstance()->getElapsedTime();

            if (timeLoading >= 0.016f )
            {
                timeLoading = 0.0f;
                sptLoading->rotate(Vector3(0.0f, 0.0f, -10.0f));
            }
            sptLoading->update();
        }

        renderer->render(this);
    }

    if (!loading)
    {
        scriptMS = 0.0f;

        octree->update();

        if (isScripted())
        {
            std::string function_Update("update");
            Script::getInstance()->execute(function_Update, 0);
        }

        std::vector<Layer*>::const_iterator it_l = layers.begin();
        while (it_l != layers.end())
        {
            Layer* layer = *it_l++;

            if (layer)
            {
                update = updateObjects(layer->getObjects2D());
                result = result && update;
                update = updateObjects(layer->getLights());
                result = result && update;
                update = updateObjects(layer->getObjects3D());
                result = result && update;
            }
        }
    }

    return result;
}

bool Magic3D::Scene::updateObjects(std::vector<Object*>* objects)
{
    bool result = true;
    bool update = false;

    std::vector<Object*>::const_iterator it_o = objects->begin();
    while (it_o != objects->end())
    {
        Object* object = *it_o++;

        update = object->update();
        result = result && update;

        object->setInEffectFrustum(false);
    }

    return result;
}

void Magic3D::Scene::addObject(Object* object)
{
    addObject(M3D_MAIN_LAYER_NAME, object);
}

void Magic3D::Scene::addObject(std::string layer, Object* object)
{
    addObject(getLayer(layer), object);
}

void Magic3D::Scene::addObject(Layer* layer, Object* object)
{
    if (object && layer)
    {
        layer->addObject(object);

        if (object->getRender() == eRENDER_3D)
        {
            octree->add(object, true);
        }
    }
}

void Magic3D::Scene::removeObject(std::string name)
{
    removeObject(M3D_MAIN_LAYER_NAME, name);
}

void Magic3D::Scene::removeObject(std::string layer, std::string name)
{
    removeObject(getLayer(layer),  ResourceManager::getObjects()->get(name));
}

void Magic3D::Scene::removeObject(Layer* layer, Object* object)
{
    if (layer && object)
    {
        layer->removeObject(object);
        if (object->getOctree())
        {
            object->getOctree()->remove(object);
        }
    }
}

int Magic3D::Scene::getObjectCount()
{
    int count = 0;

    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer)
        {
            count += layer->getObjects2D()->size() + layer->getObjects3D()->size();
        }
    }

    return count;
}

void Magic3D::Scene::setColorClear(ColorRGBA color)
{
    colorClear = color;
}

const Magic3D::ColorRGBA& Magic3D::Scene::getColorClear()
{
    return colorClear;
}

void Magic3D::Scene::setColorAmbient(ColorRGBA color)
{
    colorAmbient = color;
}

const Magic3D::ColorRGBA& Magic3D::Scene::getColorAmbient()
{
    return colorAmbient;
}

void Magic3D::Scene::setFogColor(ColorRGBA color)
{
    fogColor = color;
}

const Magic3D::ColorRGBA& Magic3D::Scene::getFogColor()
{
    return fogColor;
}

void Magic3D::Scene::setFogStart(float start)
{
    fogStart = start;
}

float Magic3D::Scene::getFogStart()
{
    return fogStart;
}

void Magic3D::Scene::setFogFinish(float finish)
{
    fogFinish = finish;
}

float Magic3D::Scene::getFogFinish()
{
    return fogFinish;
}

void Magic3D::Scene::setVolumeMaster(float volume)
{
    volumeMaster = volume;
}

float Magic3D::Scene::getVolumeMaster()
{
    return volumeMaster;
}

void Magic3D::Scene::setFBOCamera(Camera* camera)
{
    fboCamera = camera;
}

Magic3D::Camera* Magic3D::Scene::getFBOCamera()
{
    return fboCamera;
}

Magic3D::Light* Magic3D::Scene::getShadowsLight()
{
    return shadowsLight;
}

void Magic3D::Scene::setGravity(const Vector3& gravity)
{
    this->gravity = gravity;

    Physics::getInstance()->setGravity(gravity);
}

const Magic3D::Vector3& Magic3D::Scene::getGravity()
{
    return gravity;
}

void Magic3D::Scene::setWind(const Vector3& wind)
{
    this->wind = wind;
}

const Magic3D::Vector3& Magic3D::Scene::getWind()
{
    return wind;
}

void Magic3D::Scene::showFPSText(bool show)
{
    showFPS = show;
}

bool Magic3D::Scene::isShowingFPSText()
{
    return showFPS;
}

void Magic3D::Scene::showINFOText(bool show)
{
    showINFO = show;
}

bool Magic3D::Scene::isShowingINFOText()
{
    return showINFO;
}

void Magic3D::Scene::showWINDOWText(bool show)
{
    showWINDOW = show;
}

bool Magic3D::Scene::isShowingWINDOWText()
{
    return showWINDOW;
}

void Magic3D::Scene::stopAllSprites(std::string sequenceName)
{
    stopAllSprites(NULL, sequenceName);
}

void Magic3D::Scene::stopAllSprites(SpriteSequence* sequence, std::string sequenceName)
{
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        std::vector<Object*>* objects = layer->getObjects2D();
        std::vector<Object*>::const_iterator it_o = objects->begin();
        while (it_o != objects->end())
        {
            Object* obj = *it_o++;

            if (obj->getType() == eOBJECT_SPRITE)
            {
                Sprite* sprite = static_cast<Sprite*>(obj);

                if (sprite && ((!sequence || sequence == sprite->getCurrentSequence())) && sprite->getCurrentSequenceName().compare(sequenceName) == 0)
                {
                    sprite->setCurrentSequence(NULL);
                }
            }
        }
    }
}

void Magic3D::Scene::stopAllModels(std::string sequenceName)
{
    stopAllModels(NULL, sequenceName);
}

void Magic3D::Scene::stopAllModels(AnimationSequence* sequence, std::string sequenceName)
{
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        std::vector<Object*>* objects = layer->getObjects3D();
        std::vector<Object*>::const_iterator it_o = objects->begin();
        while (it_o != objects->end())
        {
            Object* obj = *it_o++;

            if (obj->getType() == eOBJECT_MODEL)
            {
                Model* model = static_cast<Model*>(obj);

                AnimationSequence* current = model && model->getSkeleton() ? model->getSkeleton()->getAnimation()->getCurrentSequence() : NULL;
                if (current)
                {
                    if ((!sequence || sequence == current) && current->getName().compare(sequenceName) == 0)
                    {
                        model->getSkeleton()->getAnimation()->setCurrentSequence(NULL);
                    }
                }
            }
        }
    }
}

void Magic3D::Scene::clear(bool clearResources, bool addMainLayer)
{
    additive = false;

    if (Script::getInstance())
    {
        Script::getInstance()->stop();
    }

    fboCamera = NULL;
    shadowsLight = NULL;

    octree->clear();

    objects2D.clear();
    objects3D.clear();
    shadows.clear();
    reflectives.clear();
    reflections.clear();
    reflectionsZOrder.clear();
    zorder.clear();
    lights.clear();
    sounds.clear();

    while (!layers.empty()) {
        Layer* layer = *layers.begin();
        layers.erase(layers.begin());
        if (layer)
        {
            if (!clearResources)
            {
                layer->clear();
            }
            delete layer;
        }
    }

    Renderer* renderer = Renderer::getInstance();
    if (renderer->lblFPS)
    {
        delete renderer->lblFPS;
        renderer->lblFPS = NULL;
    }

    if (renderer->lblINFO)
    {
        delete renderer->lblINFO;
        renderer->lblINFO = NULL;
    }

    if (renderer->lblCURSOR)
    {
        delete renderer->lblCURSOR;
        renderer->lblCURSOR = NULL;
    }

    if (lblLoading)
    {
        delete lblLoading;
        lblLoading = NULL;
    }

    if (sptLoading)
    {
        delete sptLoading;
        sptLoading = NULL;
    }

    if (clearResources)
    {
        ResourceManager::getInstance()->clear();

        if (Script::getInstance())
        {
            Script::getInstance()->clear();
        }

        Physics::getInstance()->stop();
        Physics::getInstance()->play();
    }

    if (addMainLayer)
    {
        addLayer(new Layer(M3D_MAIN_LAYER_NAME));
    }
}

void Magic3D::Scene::playObjectsTweens(std::vector<Object*>* objects)
{
    std::vector<Object*>::const_iterator it_o = objects->begin();
    while (it_o != objects->end())
    {
        Object* object = *it_o++;

        std::vector<Tween*>* tweens = object->getTweens();
        std::vector<Tween*>::const_iterator it_t = tweens->begin();
        while (it_t != tweens->end())
        {
            Tween* tween = *it_t++;

            if (tween->isPlayingAtStart())
            {
                tween->play();
            }
        }

        if (object->getType() == eOBJECT_MODEL)
        {
            Skeleton* skeleton = static_cast<Model*>(object)->getSkeleton();
            if (skeleton)
            {
                for (int i = 0; i < skeleton->getBoneCount(); i++)
                {
                    Bone* bone = skeleton->getBone(i);
                    std::vector<Tween*>* tweens = bone->getTweens();
                    std::vector<Tween*>::const_iterator it_t = tweens->begin();
                    while (it_t != tweens->end())
                    {
                        Tween* tween = *it_t++;

                        if (tween->isPlayingAtStart())
                        {
                            tween->play();
                        }
                    }
                }
            }
        }
    }
}

void Magic3D::Scene::playObjectsTweens()
{
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer)
        {
            playObjectsTweens(layer->getObjects2D());
            playObjectsTweens(layer->getLights());
            playObjectsTweens(layer->getObjects3D());
        }
    }
}

void Magic3D::Scene::connectObjectsConstraints(std::vector<Object*>* objects)
{
    std::vector<Object*>::const_iterator it_o = objects->begin();
    while (it_o != objects->end())
    {
        Object* object = *it_o++;

        std::vector<PhysicsConstraint*>* constraints = object->getConstraints();
        std::vector<PhysicsConstraint*>::const_iterator it_t = constraints->begin();
        while (it_t != constraints->end())
        {
            PhysicsConstraint* constraint = *it_t++;

            if (constraint->isEnabled())
            {
                constraint->connect();
            }
        }

        if (object->getType() == eOBJECT_MODEL)
        {
            Skeleton* skeleton = static_cast<Model*>(object)->getSkeleton();
            if (skeleton)
            {
                for (int i = 0; i < skeleton->getBoneCount(); i++)
                {
                    Bone* bone = skeleton->getBone(i);
                    std::vector<PhysicsConstraint*>* constraints = bone->getConstraints();
                    std::vector<PhysicsConstraint*>::const_iterator it_t = constraints->begin();
                    while (it_t != constraints->end())
                    {
                        PhysicsConstraint* constraint = *it_t++;

                        if (constraint->isEnabled())
                        {
                            constraint->connect();
                        }
                    }
                }
            }
        }
    }
}

void Magic3D::Scene::connectObjectsConstraints()
{
    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer)
        {
            connectObjectsConstraints(layer->getObjects2D());
            connectObjectsConstraints(layer->getLights());
            connectObjectsConstraints(layer->getObjects3D());
        }
    }
}

void Magic3D::Scene::spawnInstances()
{
    std::map<std::string, std::string>::const_iterator it_i = spawns.begin();

    while (it_i != spawns.end())
    {
        Object* obj = ResourceManager::getObjects()->get((*it_i).first);

        std::string inst = (*it_i).second;

        Object* instance = ResourceManager::getObjects()->get(inst);

        if (obj && instance)
        {
            Object* spawn = instance->spawn(obj->getName());

            spawn->setParent(obj->getParent());
            spawn->setParentBone(obj->getParentBone());
            spawn->setParentRelation(obj->isParentPosition(), obj->isParentRotation(), obj->isParentScale());
            spawn->applyMatrix(obj->getMatrix());
            spawn->setEnabled(obj->isEnabled());
            spawn->setVisible(obj->isVisible());
            spawn->setPickable(obj->isPickable());
            spawn->setFlag(obj->getFlag());

            Layer* layer = instance->getLayer();

            if (layer)
            {
                layer->removeObject(obj);
                addObject(layer, spawn);
            }

            ResourceManager::getObjects()->replace(obj->getName(), spawn);
        }

        it_i++;
    }

    spawns.clear();
}

bool Magic3D::Scene::load(std::string mge, bool additive)
{
    if (loading)
    {
        return false;
    }

    this->additive = additive;

    bool result = false;
    if (!mge.empty())
    {
        doc = new XMLDocument();

        std::string fileName = mge.rfind(M3D_SCENE_FILE) != std::string::npos ? mge.c_str() : std::string(mge + M3D_SCENE_FILE).c_str();
        if (!additive)
        {
            script = fileName.substr(0, fileName.find(M3D_SCENE_FILE));
        }

        if (doc->LoadFile(std::string(ResourceManager::getInstance()->getPath() + fileName).c_str()) == XML_SUCCESS)
        {
            Log::logFormat(eLOG_RENDERER, "Start loading scene: %s", fileName.c_str());
            XMLElement* root = doc->FirstChildElement();
            // should always have a valid root but handle gracefully if it does
            if (root)
            {
#ifndef MAGIC3D_IOS
                XMLElement* pElem = root->FirstChildElement("Project")->FirstChildElement("data");

                if (pElem)
                {
                    ResourceManager::getInstance()->setPath(Magic3D::getApplicationPath() + std::string(pElem->GetText()));
                    ResourceManager::getInstance()->setUserPath(Magic3D::getApplicationPath() + std::string(pElem->GetText()));
                }
#endif
                load(root);
            }

            result = true;
        }
        else
        {
            if (doc)
            {
                delete doc;
                doc = NULL;
            }
        }
    }

    return result;
}


bool Magic3D::Scene::isLoading()
{
    return loading;
}

bool Magic3D::Scene::getUniqueUpdateFlag()
{
    return uniqueUpdate;
}

void Magic3D::Scene::setScript(std::string script)
{
    this->script = script;
}

std::string Magic3D::Scene::getScript()
{
    return script;
}

void Magic3D::Scene::setScripted(bool scripted)
{
    this->scripted = scripted;

    if (this->scripted)
    {
        Script::getInstance()->setMain(getScript());
    }
    else
    {
        Script::getInstance()->removeMain();
    }
}

bool Magic3D::Scene::isScripted()
{
    return scripted;
}

Magic3D::XMLElement* Magic3D::Scene::save(XMLElement* root)
{
    XMLElement* attribute = NULL;
    XMLElement* scene = root->GetDocument()->NewElement( M3D_SCENE_XML );
    scene->SetAttribute(M3D_SCENE_XML_LAYERS, (int)layers.size());
    root->LinkEndChild( scene );

    std::vector<Layer*>::const_iterator it_l = layers.begin();
    while (it_l != layers.end())
    {
        Layer* layer = *it_l++;

        if (layer)
        {
            XMLElement* layerXML = scene->GetDocument()->NewElement( M3D_SCENE_XML_LAYER );
            layerXML->SetAttribute(M3D_SCENE_XML_OBJECTS, (int)(layer->getObjects2D()->size() + layer->getObjects3D()->size()));
            scene->LinkEndChild(layerXML);

            attribute = layerXML->GetDocument()->NewElement( M3D_SCENE_XML_LAYER_NAME );
            attribute->LinkEndChild( attribute->GetDocument()->NewText( layer->getName().c_str() ));
            layerXML->LinkEndChild(attribute);

            attribute = layerXML->GetDocument()->NewElement( M3D_SCENE_XML_LAYER_VISIBLE );
            attribute->LinkEndChild( attribute->GetDocument()->NewText( layer->isVisible() ? "1" : "0" ));
            layerXML->LinkEndChild(attribute);

            std::vector<Object*>* objects = layer->getObjects2D();

            std::vector<Object*>::const_iterator it_o = objects->begin();
            while (it_o != objects->end())
            {
                Object* object = *it_o++;

                XMLElement* objectXML = layerXML->GetDocument()->NewElement( M3D_OBJECT_XML );
                layerXML->LinkEndChild( objectXML );

                object->save(objectXML);
            }

            objects = layer->getLights();

            it_o = objects->begin();
            while (it_o != objects->end())
            {
                Object* object = *it_o++;

                XMLElement* objectXML = layerXML->GetDocument()->NewElement( M3D_OBJECT_XML );
                objectXML->SetAttribute(M3D_SCENE_XML_MESHES, (int)object->getMeshes()->size());
                layerXML->LinkEndChild( objectXML );

                object->save(objectXML);
            }

            objects = layer->getObjects3D();

            it_o = objects->begin();
            while (it_o != objects->end())
            {
                Object* object = *it_o++;

                XMLElement* objectXML = layerXML->GetDocument()->NewElement( M3D_OBJECT_XML );
                objectXML->SetAttribute(M3D_SCENE_XML_MESHES, (int)object->getMeshes()->size());
                layerXML->LinkEndChild( objectXML );

                object->save(objectXML);
            }
        }
    }

    ViewPort* viewport = Renderer::getInstance()->getViewPort(0);

    if (viewport)
    {
        saveString(scene, M3D_SCENE_XML_CAMERA_ORTHOGRAPHIC, viewport->getOrthographic() ? viewport->getOrthographic()->getName() : std::string(M3D_XML_NULL));
        saveString(scene, M3D_SCENE_XML_CAMERA_PERSPECTIVE, viewport->getPerspective() ? viewport->getPerspective()->getName() : std::string(M3D_XML_NULL));
    }

    saveColorRGBA(scene, M3D_SCENE_XML_COLOR_CLEAR,   colorClear);
    saveColorRGBA(scene, M3D_SCENE_XML_COLOR_AMBIENT, colorAmbient);
    saveColorRGBA(scene, M3D_SCENE_XML_FOG_COLOR,     fogColor);

    saveFloat(scene, M3D_SCENE_XML_FOG_START,  fogStart);
    saveFloat(scene, M3D_SCENE_XML_FOG_FINISH, fogFinish);

    saveVector3(scene, M3D_SCENE_XML_GRAVITY, gravity);
    saveVector3(scene, M3D_SCENE_XML_WIND, wind);

    saveFloat(scene, M3D_SCENE_XML_VOLUME_MASTER, volumeMaster);

    saveInt(scene, M3D_SCENE_XML_SCRIPTED, isScripted() ? 1 : 0);

    saveString(scene, M3D_SCENE_XML_FBO_CAMERA, fboCamera ? fboCamera->getName() : std::string(M3D_XML_NULL));

    saveBool(scene, M3D_SCENE_XML_SCREEN_EFFECTS, Renderer::getInstance()->isUsingScreenEffects());

    return root;
}

Magic3D::XMLElement* Magic3D::Scene::load(XMLElement* root)
{
    Magic3D::getInstance()->pause();
    loading = true;

    colorClear = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    timeLoading = 0.0f;
    ViewPort* viewport = Renderer::getInstance()->getCurrentViewPort();
    Renderer::getInstance()->setUsingScreenEffects(false);

    if (viewport)
    {
        viewport->setOrthographic(camLoading);
    }

    if (layers.empty())
    {
        addLayer(new Layer(M3D_MAIN_LAYER_NAME));
    }
    if (root)
    {
        sceneXML = root->FirstChildElement(M3D_SCENE_XML);

        if (sceneXML)
        {
            parents.clear();
            instances.clear();
            spawns.clear();
            layerXML = sceneXML->FirstChildElement(M3D_SCENE_XML_LAYER);
            setCurrentLayerXML(false);

            load();
        }
        else
        {
            Log::log(eLOG_FAILURE, "Invalid scene file.");
            loading = false;
        }
    }

    return root;
}

void Magic3D::Scene::setCurrentLayerXML(bool next)
{
    currentLayerXML = NULL;

    if (next)
    {
        layerXML = layerXML->NextSiblingElement(M3D_SCENE_XML_LAYER);
    }

    if (layerXML)
    {
        XMLElement* xml = layerXML->FirstChildElement(M3D_SCENE_XML_LAYER_NAME);

        std::string layerName(xml->GetText());

        currentLayerXML = NULL;
        if (layerName.compare(M3D_MAIN_LAYER_NAME) != 0)
        {
            currentLayerXML = getLayer(layerName);
            if (!currentLayerXML)
            {
                currentLayerXML = new Layer(layerName);
                addLayer(currentLayerXML);
            }
        }
        else
        {
            currentLayerXML = getLayer(M3D_MAIN_LAYER_NAME);
        }

        xml = layerXML->FirstChildElement(M3D_SCENE_XML_LAYER_VISIBLE);

        if (currentLayerXML)
        {
            int value = atoi(xml->GetText());
            currentLayerXML->setVisible(value > 0);
        }
    }
}

bool Magic3D::Scene::load()
{
    bool result = false;
    if (modelXML)
    {
        if (!modelXML->load())
        {
            modelXML->load(objectXML);
            XMLElement* physics = objectXML->FirstChildElement(M3D_PHYSICS_OBJECT_XML);
            if (loadInt(physics, M3D_PHYSICS_OBJECT_XML_PHYSICS) == 1)
            {
                Physics::getInstance()->add(modelXML);
            }

            modelXML = NULL;
            objectXML = objectXML->NextSiblingElement(M3D_OBJECT_XML);

            if (!objectXML)
            {
                setCurrentLayerXML(true);
            }
        }
        result = true;
    }
    else if (layerXML)
    {
        if (!objectXML)
        {
            objectXML = layerXML->FirstChildElement(M3D_OBJECT_XML);
        }
        if (currentLayerXML && objectXML)
        {
            XMLElement* attribute = objectXML->FirstChildElement(M3D_OBJECT_XML_NAME);

            if (attribute)
            {
                std::string objName(attribute->GetText());
                XMLElement* attribute = objectXML->FirstChildElement(M3D_OBJECT_XML_TYPE);
                if (attribute)
                {
                    ResourceManager* mngr = ResourceManager::getInstance();
                    int obj = atoi(attribute->GetText());

                    Object* object = NULL;
                    modelXML = NULL;

                    bool created = false;
                    switch (obj)
                    {
                        case eOBJECT_CUSTOM:     break;
                        case eOBJECT_CAMERA:     object = mngr->addCamera(objName, created); break;
                        case eOBJECT_SPRITE:     object = mngr->addSprite(objName, created); break;
                        case eOBJECT_MODEL:
                        {
                            object = mngr->addModel(objName, created);
                            if (created)
                            {
                                modelXML = static_cast<Model*>(object);
                            }
                            break;
                        }
                        case eOBJECT_LIGHT:      object = mngr->addLight(objName, created); break;
                        case eOBJECT_PARTICLES:  object = mngr->addParticles(objName, created); break;
                        case eOBJECT_TERRAIN:    break;
                        case eOBJECT_WATER:      break;
                        case eOBJECT_VEGETATION: break;
                        case eOBJECT_GUI_LABEL:  object = mngr->addGUILabel(objName, M3D_DEFAULT_FONT_SIZE, created); break;
                        case eOBJECT_GUI_WINDOW: object = mngr->addGUIWindow(objName, created); break;
                        case eOBJECT_GUI_BUTTON: object = mngr->addGUIButton(objName, created); break;
                        case eOBJECT_GUI_SLIDER: break;
                        case eOBJECT_SOUND:      object = mngr->addSound(objName, created); break;
                        case eOBJECT_JOYSTICK:   object = mngr->addJoystick(objName, created); break;
                        case eOBJECT_INSTANCE:   object = mngr->addInstance(objName, created); break;
                    }

                    if (object)
                    {
                        if (created)
                        {
                            currentLayerXML->addObject(object);

                            if (object->getRender() == eRENDER_3D)
                            {
                                octree->add(object);
                            }
                        }

                        object->load(objectXML);
                        if (obj == eOBJECT_MODEL && !modelXML)
                        {
                            XMLElement* physics = objectXML->FirstChildElement(M3D_PHYSICS_OBJECT_XML);
                            if (loadInt(physics, M3D_PHYSICS_OBJECT_XML_PHYSICS) == 1)
                            {
                                Physics::getInstance()->add(object);
                            }
                        }

                        XMLElement* xmlParent = objectXML->FirstChildElement(M3D_OBJECT_XML_PARENT);
                        if (xmlParent && strcmp(xmlParent->GetText(), M3D_XML_NULL) != 0)
                        {
                            std::string parent = loadString(objectXML, M3D_OBJECT_XML_PARENT);
                            std::string bone = loadString(objectXML, M3D_OBJECT_XML_PARENT_BONE);

                            object_parents p;
                            p.parent = parent;
                            p.bone = bone;

                            parents[object->getName()] = p;
                        }

                        XMLElement* xmlInstance = objectXML->FirstChildElement(M3D_OBJECT_INSTANCE_NAME);
                        if (xmlInstance && strcmp(xmlInstance->GetText(), M3D_XML_NULL) != 0)
                        {
                            std::string instanceName = loadString(objectXML, M3D_OBJECT_INSTANCE_NAME);
                            bool spawn = loadBool(objectXML, M3D_OBJECT_INSTANCE_SPAWN);

                            if (spawn)
                            {
                                spawns[object->getName()] = instanceName;
                            }
                            instances[object->getName()] = instanceName;
                        }
                    }
                }
            }

            if (!modelXML)
            {
                objectXML = objectXML->NextSiblingElement(M3D_OBJECT_XML);
                update();
            }
        }

        if (!objectXML)
        {
            setCurrentLayerXML(true);
        }

        result = true;
    }
    else
    {
        bool finish = false;
        if (!parents.empty())
        {
            Object* obj = ResourceManager::getObjects()->get((*parents.begin()).first);

            object_parents p = (*parents.begin()).second;

            Object* parent = ResourceManager::getObjects()->get(p.parent);
            obj->setParent(parent);

            if (parent && parent->getType() == eOBJECT_MODEL && !p.bone.empty() && p.bone.compare(M3D_XML_NULL) != 0)
            {
                Model* model = static_cast<Model*>(parent);
                if (model->getSkeleton())
                {
                    obj->setParentBone(model->getSkeleton()->getBoneByName(p.bone));
                }
            }
            parents.erase(parents.begin());

            update();

            result = true;
        }
        else if (!instances.empty())
        {
            Object* obj = ResourceManager::getObjects()->get((*instances.begin()).first);

            std::string inst = (*instances.begin()).second;

            Object* instance = ResourceManager::getObjects()->get(inst);
            static_cast<ObjectInstance*>(obj)->setInstance(instance);

            instances.erase(instances.begin());

            update();

            result = true;
        }
        else if (!additive)
        {
            ViewPort* viewport = Renderer::getInstance()->getViewPort(0);

            if (viewport)
            {
                std::string orthographic = loadString(sceneXML, M3D_SCENE_XML_CAMERA_ORTHOGRAPHIC);
                std::string perspective = loadString(sceneXML, M3D_SCENE_XML_CAMERA_PERSPECTIVE);

                if (orthographic.compare(M3D_XML_NULL) != 0)
                {
                    viewport->setOrthographic((Camera*)ResourceManager::getObjects()->get(orthographic));
                }

                if (perspective.compare(M3D_XML_NULL) != 0)
                {
                    viewport->setPerspective((Camera*)ResourceManager::getObjects()->get(perspective));
                }
            }

            oldColorClear   = loadColorRGBA(sceneXML, M3D_SCENE_XML_COLOR_CLEAR);

            colorClear   = loadColorRGBA(sceneXML, M3D_SCENE_XML_COLOR_CLEAR);
            colorAmbient = loadColorRGBA(sceneXML, M3D_SCENE_XML_COLOR_AMBIENT);
            fogColor     = loadColorRGBA(sceneXML, M3D_SCENE_XML_FOG_COLOR);

            fogStart  = loadFloat(sceneXML, M3D_SCENE_XML_FOG_START);
            fogFinish = loadFloat(sceneXML, M3D_SCENE_XML_FOG_FINISH);

            gravity = loadVector3(sceneXML, M3D_SCENE_XML_GRAVITY);
            wind = loadVector3(sceneXML, M3D_SCENE_XML_WIND);
            Physics::getInstance()->setGravity(gravity);

            volumeMaster = loadFloat(sceneXML, M3D_SCENE_XML_VOLUME_MASTER);

            std::string fboCam = loadString(sceneXML, M3D_SCENE_XML_FBO_CAMERA);

            if (fboCam.compare(M3D_XML_NULL) != 0)
            {
                fboCamera = (Camera*)ResourceManager::getObjects()->get(fboCam);
            }

            Renderer::getInstance()->setUsingScreenEffects(loadBool(sceneXML, M3D_SCENE_XML_SCREEN_EFFECTS));

            setScripted(loadInt(sceneXML, M3D_SCENE_XML_SCRIPTED) == 0 ? false : true);

            finish = true;
        }
        else
        {
            colorClear = oldColorClear;

            finish = true;
        }

        if (finish)
        {
            if (doc)
            {
                delete doc;
                doc = NULL;
            }

            connectObjectsConstraints();
#ifndef _MGE_
            spawnInstances();
#endif

            result = true;
            loading = false;

            Log::log(eLOG_SUCCESS, "Scene loaded!");
        }
    }

    return result;
}
