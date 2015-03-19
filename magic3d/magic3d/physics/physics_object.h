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

#ifndef MAGIC3D_PHYSICS_OBJECT_H
#define MAGIC3D_PHYSICS_OBJECT_H

#include <magic3d/xml.h>
#include <magic3d/physics/physics.h>
#include <magic3d/physics/physics_constraint.h>

namespace Magic3D
{

#define M3D_PHYSICS_OBJECT_XML                  "Physics"
#define M3D_PHYSICS_OBJECT_XML_PHYSICS          "physics"
#define M3D_PHYSICS_OBJECT_XML_SHAPE            "shape"
#define M3D_PHYSICS_OBJECT_XML_SHAPEPOSITION    "shape_position"
#define M3D_PHYSICS_OBJECT_XML_SHAPESIZE        "shape_size"
#define M3D_PHYSICS_OBJECT_XML_SHAPEROTATION    "shape_rotation"
#define M3D_PHYSICS_OBJECT_XML_SHAPETRANSLATION "shape_translation"
#define M3D_PHYSICS_OBJECT_XML_RADIUS           "radius"
#define M3D_PHYSICS_OBJECT_XML_MARGIN           "margin"
#define M3D_PHYSICS_OBJECT_XML_MASS             "mass"
#define M3D_PHYSICS_OBJECT_XML_FRICTION         "friction"
#define M3D_PHYSICS_OBJECT_XML_RESTITUTION      "restitution"
#define M3D_PHYSICS_OBJECT_XML_DAMPINGLINEAR    "damping_linear"
#define M3D_PHYSICS_OBJECT_XML_DAMPINGANGULAR   "damping_angular"
#define M3D_PHYSICS_OBJECT_XML_GHOST            "ghost"
#define M3D_TWEENS_XML                          "Tweens"
#define M3D_PHYSICS_CONSTRAINTS_XML             "Constraints"

enum OBJECT
{
    eOBJECT_CUSTOM,
    eOBJECT_CAMERA,
    eOBJECT_SPRITE,
    eOBJECT_MODEL,
    eOBJECT_LIGHT,
    eOBJECT_PARTICLES,
    eOBJECT_TERRAIN,
    eOBJECT_GUI_LABEL,
    eOBJECT_GUI_WINDOW,
    eOBJECT_GUI_BUTTON,
    eOBJECT_GUI_SLIDER,
    eOBJECT_SOUND,
    eOBJECT_WATER,
    eOBJECT_VEGETATION,
    eOBJECT_CLOTH,
    eOBJECT_JOYSTICK,
    eOBJECT_BONE,
    eOBJECT_INSTANCE
};

enum RENDER
{
    eRENDER_2D,
    eRENDER_3D
};

class Object;
class Tween;
class PhysicsObject : public Xml
{
private:
    btRigidBody* body;
    PhysicsCollision* collision;
    std::vector<PhysicsConstraint*> constraints;
    std::vector<Tween*> tweens;

    Vector3 shapePosition;
    Vector3 shapeSize;
    Vector3 shapeRotation;
    Vector3 shapeTranslation;

    float radius;
    float margin;

    float mass;
    float friction;
    float restitution;
    float dampingLinear;
    float dampingAngular;

    bool ghost;

    PHYSICS_SHAPE shape;
    bool shapeOwner;

protected:
    OBJECT type;
    bool needTransform;
    bool needUpdateEuler;

    PhysicsObject(const PhysicsObject& physicsObject);

public:
    PhysicsObject(OBJECT type);
    virtual ~PhysicsObject();
    virtual PhysicsObject* spawn() const;

    virtual bool updateTweens();

    virtual void addConstraint(PhysicsConstraint* constraint);
    virtual void removeConstraint(int index);
    virtual PhysicsConstraint* getConstraint(int index);
    virtual int getConstraintIndex(PhysicsConstraint* constraint);
    virtual int getConstraintCount();
    virtual void clearConstraints();
    virtual std::vector<PhysicsConstraint*>* getConstraints();

    virtual void addTween(Tween* tween);
    virtual void removeTween(int index);
    virtual Tween* getTween(int index);
    virtual int getTweenIndex(Tween* tween);
    virtual int getTweenCount();
    virtual void clearTweens();
    virtual std::vector<Tween*>* getTweens();

    OBJECT getType();
    virtual void setPosition(Vector3 position);
    virtual Vector3 getPosition();
    virtual void setRotation(Quaternion rotation);
    virtual Quaternion getRotation();
    virtual void setScale(Vector3 scale);
    virtual Vector3 getScale();
    virtual RENDER getRender();
    virtual void lookAt(Vector3 position, Vector3 up);

    void setRigidBody(btRigidBody* body);
    btRigidBody* getRigidBody();

    void setShape(PHYSICS_SHAPE shape);
    PHYSICS_SHAPE getShape();

    void setShapeOwner(bool owner);
    bool isShapeOwner();

    void setShapePosition(Vector3 position);
    const Vector3& getShapePosition();
    void setShapeSize(Vector3 size);
    const Vector3& getShapeSize();
    void setShapeRotation(Vector3 rotation);
    const Vector3& getShapeRotation();
    void setShapeTranslation(Vector3 translation);
    const Vector3& getShapeTranslation();

    void setRadius(float radius);
    float getRadius();
    void setMargin(float margin);
    float getMargin();
    void setMass(float mass);
    float getMass();
    void setFriction(float friction);
    float getFriction();
    void setRestitution(float restitution);
    float getRestitution();
    void setDampingLinear(float damping);
    float getDampingLinear();
    void setDampingAngular(float damping);
    float getDampingAngular();

    void setGhost(bool ghost);
    bool isGhost();

    void setPhysicsCollision(PhysicsCollision* collision);
    PhysicsCollision* getPhysicsCollision();

    void activatePhysics(bool activate);
    bool isActivatedPhysics();

    void applyPhysicsCentralForce(Vector3 force);
    void applyPhysicsCentralImpulse(Vector3 impulse);
    void applyPhysicsDamping(float damping);
    void applyPhysicsForce(Vector3 force, Vector3 position);
    void applyPhysicsGravity();
    void applyPhysicsImpulse(Vector3 impulse, Vector3 position);
    void applyPhysicsTorque(Vector3 torque);
    void applyPhysicsTorqueImpulse(Vector3 torque);

    void setPhysicsFriction(float friction);
    float getPhysicsFriction();
    void setPhysicsGravity(Vector3 gravity);
    Vector3 getPhysicsGravity();
    void setPhysicsDamping(float linear, float angular);
    float getPhysicsLinearDamping();
    float getPhysicsAngularDamping();
    void setPhysicsAngularFactor(Vector3 factor);
    Vector3 getPhysicsAngularFactor();
    void setPhysicsCentreOfMass(Vector3 position);
    Vector3 getPhysicsCentreOfMass();
    void setPhysicsAngularVelocity(Vector3 velocity);
    Vector3 getPhysicsAngularVelocity();
    void setPhysicsLinearVelocity(Vector3 velocity);
    Vector3 getPhysicsLinearVelocity();
    void setPhysicsLinearFactor(Vector3 factor);
    Vector3 getPhysicsLinearFactor();
    Vector3 getPhysicsTotalForce();
    Vector3 getPhysicsTotalTorque();
    bool isPhysicsSleeping();

    void resetPhysics();

    Object* getAsObject();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_PHYSICS_OBJECT_H
