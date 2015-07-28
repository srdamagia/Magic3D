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

#ifndef MAGIC3D_SCENE_H
#define MAGIC3D_SCENE_H

#include <magic3d/octree.h>
#include <magic3d/type.h>
#include <magic3d/gui/guilabel.h>
#include <magic3d/gui/joystick.h>
#include <magic3d/model/model.h>

namespace Magic3D
{

class Camera;
class Light;

#define M3D_SCENE_FILE                    ".mge"
#define M3D_MAIN_LAYER_NAME               "magic3d"

#define M3D_SCENE_XML                     "Scene"
#define M3D_SCENE_XML_LAYER               "Layer"
#define M3D_SCENE_XML_LAYERS              "layers"
#define M3D_SCENE_XML_OBJECTS             "objects"
#define M3D_SCENE_XML_MESHES              "meshes"
#define M3D_SCENE_XML_LAYER_NAME          "name"
#define M3D_SCENE_XML_LAYER_VISIBLE       "visible"
#define M3D_SCENE_XML_CAMERA_ORTHOGRAPHIC "camera_orthographic"
#define M3D_SCENE_XML_CAMERA_PERSPECTIVE  "camera_perspective"
#define M3D_SCENE_XML_COLOR_CLEAR         "color_clear"
#define M3D_SCENE_XML_COLOR_AMBIENT       "color_ambient"
#define M3D_SCENE_XML_FOG_COLOR           "fog_color"
#define M3D_SCENE_XML_FOG_START           "fog_start"
#define M3D_SCENE_XML_FOG_FINISH          "fog_finish"
#define M3D_SCENE_XML_GRAVITY             "gravity"
#define M3D_SCENE_XML_WIND                "wind"
#define M3D_SCENE_XML_VOLUME_MASTER       "volume_master"
#define M3D_SCENE_XML_SCRIPTED            "scripted"
#define M3D_SCENE_XML_FBO_CAMERA          "fbo_camera"
#define M3D_SCENE_XML_SCREEN_EFFECTS      "screen_effects"

class Layer
{
private:
    std::vector<Object*> cameras;
    std::vector<Object*> lights;
    std::vector<Object*> objects2D;
    std::vector<Object*> objects3D;

    std::string name;

    bool visible;

    void removeObjectFromList(std::string name, std::vector<Object*>* objects);
    void clearList(std::vector<Object*>* objects);

public:
    Layer(std::string name);
    ~Layer();

    void setName(std::string name);
    const std::string& getName();

    void addObject(Object* object);
    void removeObject(std::string name);
    void removeObject(Object* object);
    int getObjectIndex(std::string name, std::vector<Object*>* list);

    std::vector<Object*>* getCameras();
    std::vector<Object*>* getLights();
    std::vector<Object*>* getObjects2D();
    std::vector<Object*>* getObjects3D();

    void setVisible(bool visible);
    bool isVisible();

    void clear();
};

struct RenderObject
{
    RenderObject(Object* object, Matrix4& modelView, Matrix4& model) {this->object = object; this->modelView = modelView; this->model = model;}
    Object* object;
    Matrix4 modelView;
    Matrix4 model;
};

class Scene : public Xml
{
private:
    struct object_parents
    {
        std::string parent;
        std::string bone;
    };

    std::map<std::string, object_parents> parents;
    std::map<std::string, std::string> instances;
    std::map<std::string, std::string> spawns;

protected:
    std::vector<Layer*> layers;

private:
    Octree* octree;
    std::list<RenderObject> objects2D;
    std::list<RenderObject> objects3D;
    std::list<RenderObject> shadows;
    std::list<RenderObject> reflectives;
    std::list<RenderObject> reflections;
    std::list<RenderObject> reflectionsZOrder;
    std::list<RenderObject> glow;
    std::list<RenderObject> zorder;

    std::list<Object*> lights;
    std::list<Object*> sounds;

    static Scene* instance;

    GUILabel* lblLoading;
    Sprite*   sptLoading;
    Camera*   camLoading;

    tinyxml2::XMLDocument* doc;

    XMLElement* sceneXML;
    XMLElement* layerXML;
    XMLElement* objectXML;

    Layer* currentLayerXML;
    Model* modelXML;

    Camera* fboCamera;
    Light* shadowsLight;

    char profileText[1024];
    std::string script;

    ColorRGBA oldColorClear;
    ColorRGBA colorClear;
    ColorRGBA colorAmbient;
    ColorRGBA fogColor;

    Vector3 gravity;
    Vector3 wind;

    float fogStart;
    float fogFinish;

    float volumeMaster;

    float timeLoading;
    float frustumMS;
    float scriptMS;

    bool showFPS;
    bool showINFO;
    bool showWINDOW;

    bool loading;
    bool scripted;
    bool additive;

    bool uniqueUpdate;

    bool activeLights;

    bool updateObjects(std::vector<Object*>* objects);
    void playObjectsTweens(std::vector<Object*>* objects);
    void connectObjectsConstraints(std::vector<Object*>* objects);

    void setCurrentLayerXML(bool next);

    void prepareUpdate(Camera* camera, bool reflectives = true, bool reflections = false, bool anyType = false);
    bool updateVisibleObjectsFromOctreeNode(Octree* octree, Camera* camera, bool reflectives, bool reflections, bool anyType);

    Scene();
    virtual ~Scene();

public:
    static bool start();
    static bool finish();

    static Scene* getInstance();

    virtual bool update();
    void addObject(Object* object);
    void addObject(std::string layer, Object* object);
    void addObject(Layer* layer, Object* object);
    void removeObject(std::string name);
    void removeObject(std::string layer, std::string name);
    void removeObject(Layer* layer, Object*);
    int getObjectCount();

    void addLayer(Layer* layer);
    void removeLayer(std::string name);
    Layer* getLayer(std::string name);
    int getLayerIndex(std::string name);
    std::vector<Layer*>* getLayers();

    void updateVisibleObjects2D(Camera* camera, bool profile = true);    
    void updateVisibleObjects3D(Camera* camera, bool reflectives = true, bool reflections = false, bool anyType = false);
    void updateVisibleObjectsOctree(Camera* camera, bool reflectives = true, bool reflections = false, bool anyType = false);
    void updateFrustumCulling(Camera* camera2D, Camera* camera3D);
    float getFrustumMS();

    Octree* getOctree();

    std::list<Object*>* getVisibleLights(Camera* camera);

    bool isActiveLights();
    std::list<Object*>* getLights();
    std::list<Object*>* getSounds();

    std::list<RenderObject>* getVisibleObjects2D();
    std::list<RenderObject>* getVisibleObjects3D();
    std::list<RenderObject>* getShadows();
    std::list<RenderObject>* getReflectives();
    std::list<RenderObject>* getReflections();
    std::list<RenderObject>* getReflectionsZOrder();
    std::list<RenderObject>* getGlows();
    std::list<RenderObject>* getZOrder();

    void setColorClear(ColorRGBA color);
    const ColorRGBA& getColorClear();

    void setColorAmbient(ColorRGBA color);
    const ColorRGBA& getColorAmbient();

    void setFogColor(ColorRGBA color);
    const ColorRGBA& getFogColor();
    void setFogStart(float start);
    float getFogStart();
    void setFogFinish(float finish);
    float getFogFinish();

    void setGravity(const Vector3& gravity);
    const Vector3& getGravity();

    void setWind(const Vector3& wind);
    const Vector3& getWind();

    void setVolumeMaster(float volume);
    float getVolumeMaster();

    void setFBOCamera(Camera* camera);
    Camera* getFBOCamera();

    Light* getShadowsLight();

    void showFPSText(bool show);
    bool isShowingFPSText();
    void showINFOText(bool show);
    bool isShowingINFOText();
    void showWINDOWText(bool show);
    bool isShowingWINDOWText();

    bool load(std::string mge, bool additive);
    bool load();
    bool isLoading();
    void stopAllSprites(std::string sequenceName);
    void stopAllSprites(SpriteSequence* sequence, std::string sequenceName);
    void stopAllModels(std::string sequenceName);
    void stopAllModels(AnimationSequence* sequence, std::string sequenceName);
    void clear(bool clearResources = false, bool addMainLayer = true);
    void playObjectsTweens();
    void connectObjectsConstraints();
    void spawnInstances();

    bool getUniqueUpdateFlag();

    void setScript(std::string script);
    std::string getScript();

    void setScripted(bool scripted);
    bool isScripted();
    float getScriptMS();

    void addScriptMS(float ms);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_SCENE_H
