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

#ifndef MAGIC3D_OBJECT_H
#define MAGIC3D_OBJECT_H

#include <magic3d/mesh.h>
#include <magic3d/physics/physics_object.h>
#include <magic3d/model/skeleton.h>

namespace Magic3D
{

#define M3D_OBJECT_XML                  "Object"
#define M3D_OBJECT_XML_NAME             "name"
#define M3D_OBJECT_XML_TYPE             "type"
#define M3D_OBJECT_XML_RENDER           "render"
#define M3D_OBJECT_XML_ENABLED          "enabled"
#define M3D_OBJECT_XML_VISIBLE          "visible"
#define M3D_OBJECT_XML_SCRIPTED         "scripted"
#define M3D_OBJECT_XML_SCRIPT           "script"
#define M3D_OBJECT_XML_ZORDER           "zorder"
#define M3D_OBJECT_XML_PICK             "pick"
#define M3D_OBJECT_XML_POSITION         "position"
#define M3D_OBJECT_XML_ROTATION         "rotation"
#define M3D_OBJECT_XML_SCALE            "scale"
#define M3D_OBJECT_XML_PARENT           "parent"
#define M3D_OBJECT_XML_PARENT_BONE      "parent_bone"
#define M3D_OBJECT_XML_PARENT_POSITION  "parent_position"
#define M3D_OBJECT_XML_PARENT_ROTATION  "parent_rotation"
#define M3D_OBJECT_XML_PARENT_SCALE     "parent_scale"
#define M3D_OBJECT_XML_FLAG             "flag"
#define M3D_OBJECT_XML_BILLBOARD        "billboard"
#define M3D_OBJECT_XML_COLLISIONMESH    "collision_mesh"

#define M3D_OBJECT_INSTANCE_NAME        "instance_name"
#define M3D_OBJECT_INSTANCE_SPAWN       "instance_spawn"

enum BILLBOARD
{
    eBILLBOARD_NONE,
    eBILLBOARD_HORIZONTAL,
    eBILLBOARD_VERTICAL,
    eBILLBOARD_BOTH
};

enum DISTORTION
{
    eDISTORTION_BUBBLE,
    eDISTORTION_WAVE,
    eDISTORTION_NOISE
};

class ObjectAI
{
public:
    virtual ~ObjectAI() = 0;
    virtual void AI() = 0;
    virtual ObjectAI* spawn() const = 0;
};

class Layer;
class ObjectInstance;
class Object : public PhysicsObject
{
private:
    std::vector<Mesh*> meshes;
    std::vector<Object*> children;
    std::vector<ObjectInstance*> instances;

    Object* parent;
    Bone* parentBone;

    Layer* layer;

    ObjectAI* AI;

protected:
    std::string name;

private:
    std::string script;

    Matrix4 matrix;

    Box box;

    Quaternion rotation;

    Vector3 position;
    Vector3 scale;
    Vector3 axis;
    Vector3 euler;

    Vector3 upward;
    Vector3 forward;
    Vector3 rightward;

    float angle;

    float zOrderFactor;

    RENDER render;

    int collisionMesh;
    int flag;

    BILLBOARD billboard;

    bool enabled;
    bool visible;
    bool selected;
    bool grid;
    bool frustum;
    bool frustumEffect;
    bool pick;
    bool zOrder;
    bool uniqueUpdate;
    bool scripted;
    bool shadowed;
    bool reflective;
    bool glowed;

    bool parentPosition;
    bool parentRotation;
    bool parentScale;

    void init(RENDER render, std::string name);
    void updateAxisAngleFromRotation();

protected:
    Object(const Object &object, std::string name);
    virtual void updateMatrix();

    void setParent(Object* parent, bool removeChild);
    void removeChild(Object* child);    

public:
    Object(OBJECT type, RENDER render, std::string name);
    virtual ~Object();

    virtual Object* spawn(std::string name) const;
    Object* spawn(std::string name, std::string layerName, bool keepChildrenLayer);
    void killChildren();

    void addInstance(ObjectInstance* instance);
    void removeInstance(ObjectInstance* instance);

    virtual bool update();
    virtual bool updateMeshes();

    virtual RENDER getRender();
    const std::string& getName();
    bool setName(std::string name);

    const std::string& getScript();
    void setScript(std::string script);

    void setZOrder(bool zOrder);
    bool isZOrder();
    void setZOrderFactor(float factor);
    float getZOrderFactor();

    void setEnabled(bool enabled);
    bool isEnabled();

    void setVisible(bool visible);
    bool isVisible();

    void setShadowed(bool shadowed);
    bool isShadowed();

    void setReflective(bool reflective);
    bool isReflective();

    void setGlowed(bool glowed);
    bool isGlowed();

    void setInFrustum(bool frustum);
    bool isInFrustum();
    bool isInParentFrustum();

    void setInEffectFrustum(bool frustum);
    bool isInEffectFrustum();

    void setPickable(bool pick);
    bool isPickable();

    const std::vector<Object*>* getChildren();
    const std::vector<ObjectInstance*>* getInstances();

    void setParent(Object* parent);
    Object* getParent();

    void setParentBone(Bone* bone);
    Bone* getParentBone();

    void setParentRelation(bool position, bool rotation, bool scale);
    bool isParentPosition();
    bool isParentRotation();
    bool isParentScale();

    void setLayer(Layer* layer);
    Layer* getLayer();

    virtual void setPosition(Vector3 position);
    virtual Vector3 getPosition();
    Vector3 getPositionFromParent();

    virtual void setScale(Vector3 scale);
    virtual Vector3 getScale();
    Vector3 getScaleFromParent();

    void setRotationEuler(Vector3 rotation);
    void setRotationEuler(float angle, Vector3 axis);
    const Vector3& getRotationEuler();
    Vector3 getRotationEuler(float& angle);
    virtual void setRotation(Quaternion rotation);
    virtual Quaternion getRotation();
    Quaternion getRotationFromParent();

    virtual void lookAt(Vector3 position, Vector3 up);

    void rotate(Vector3 rotation);

    const Vector3& getDirectionUp();
    Vector3 getDirectionDown();
    const Vector3& getDirectionFront();
    Vector3 getDirectionBack();
    Vector3 getDirectionLeft();
    const Vector3& getDirectionRight();

    virtual void addMesh(Mesh* mesh);
    virtual Mesh* getMesh(std::string name);
    virtual int getMeshIndex(std::string name);
    virtual void clearMeshes();
    virtual std::vector<Mesh*>* getMeshes();

    virtual const Box& getBoundingBox();
    virtual void updateBoundingBox();
    virtual void updateBoundingBox(bool updateMesh);
    virtual float getMaxSizeFromParent();

    void setMatrix(Matrix4 matrix);
    void applyMatrix(Matrix4 matrix);
    virtual const Matrix4& getMatrix();
    virtual Matrix4 getMatrixFromParent();
    virtual Matrix4 getCompleteMatrixFromParent();

    void setSelected(bool selected);
    bool isSelected();

    void setShowGrid(bool grid);
    bool isShowingGrid();

    void setFlag(int flag);
    int getFlag();

    void setBillboard(BILLBOARD billboard);
    BILLBOARD getBillboard();

    void setCollisionMeshIndex(int mesh);
    int getCollisionMeshIndex();
    virtual MeshData* getCollisionMesh();

    void setAI(ObjectAI* AI);
    ObjectAI* getAI();

    void setScripted(bool scripted);
    bool isScripted();

    void move(Vector3 factor);
    void size(Vector3 scale);

    void setMeshTexture(std::string mesh, std::string texture);
    std::string getMeshTexture(std::string mesh);
    void setMeshColor(std::string mesh, ColorRGBA color);
    ColorRGBA getMeshColor(std::string mesh);
    void setMeshVisible(std::string mesh, bool visible);
    bool isMeshVisible(std::string mesh);

    void play(bool reverse);
    void playAnimation(std::string animation, float interpolation, bool reverse);
    void stop();
    bool isPlaying();

    void setCurrentAnimation(std::string animation);
    std::string getCurrentAnimation();

    void setAnimationFrame(int frame);
    int getAnimationFrame();
    int getAnimationFrameCount();

    virtual Object* getObject(std::string name);
    virtual Object* getChild(std::string name);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class ObjectInstance : public Object
{
private:
    Object* instance;
    bool spawnOnLoad;
    Box box;
protected:
    ObjectInstance(const ObjectInstance &objectInstance, std::string name);
public:
    ObjectInstance(std::string name);
    virtual ~ObjectInstance();

    virtual ObjectInstance* spawn(std::string name) const;

    void setInstance(Object* object, bool removeOld = true);
    Object* getInstance();

    void setSpawnOnLoad(bool spawn);
    bool isSpawnedOnLoad();

    virtual const Box& getBoundingBox();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_OBJECT_H
