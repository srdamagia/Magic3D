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

#include <magic3d/physics/physics_object.h>
#include <magic3d/object.h>
#include <magic3d/tween/tweens.h>

Magic3D::PhysicsObject::PhysicsObject(const PhysicsObject& physicsObject)
{
    this->type          = physicsObject.type;
    this->shapePosition = physicsObject.shapePosition;
    this->shapeSize = physicsObject.shapeSize;
    this->shapeRotation = physicsObject.shapeRotation;
    this->shapeTranslation = physicsObject.shapeTranslation;

    this->radius = physicsObject.radius;
    this->margin = physicsObject.margin;

    this->mass = physicsObject.mass;
    this->friction = physicsObject.friction;
    this->restitution = physicsObject.restitution;
    this->dampingLinear = physicsObject.dampingLinear;
    this->dampingAngular = physicsObject.dampingAngular;

    this->shape = physicsObject.shape;
    this->shapeOwner = false;
    this->ghost = physicsObject.ghost;

    this->body = NULL;
    this->collision = physicsObject.collision ? physicsObject.collision->spawn() : NULL;

    this->needTransform = true;
    this->needUpdateEuler = true;

    std::vector<Tween*>::const_iterator it_t = physicsObject.tweens.begin();

    while (it_t != physicsObject.tweens.end())
    {
        Tween* tween = *it_t++;
        this->tweens.push_back(tween->spawn());
    }

    if (physicsObject.body)
    {
        switch (physicsObject.type)
        {
            case eOBJECT_BONE:
            {
                Physics::getInstance()->add(this);
                break;
            }
            default: break;
        }
    }
}

Magic3D::PhysicsObject::PhysicsObject(OBJECT type)
{
    this->type       = type;
    body             = NULL;
    collision        = NULL;
    shape            = ePHYSICS_SHAPE_BOX;
    shapeOwner       = false;
    shapePosition    = Vector3(0.0f, 0.0f, 0.0f);
    shapeSize        = Vector3(0.0f, 0.0f, 0.0f);
    shapeRotation    = Vector3(1.0f, 1.0f, 1.0f);
    shapeTranslation = Vector3(1.0f, 1.0f, 1.0f);
    radius           = 0.0f;
    margin           = 0.04f;
    mass             = 0.0f;
    friction         = 0.5f;
    restitution      = 0.0f;
    dampingLinear    = 0.0f;
    dampingAngular   = 0.0f;
    ghost            = false;

    needTransform    = true;
    needUpdateEuler  = true;
}

Magic3D::PhysicsObject::~PhysicsObject()
{
    clearTweens();
    clearConstraints();

    if (body)
    {
        Physics::getInstance()->remove(this);
    }

    if (collision)
    {
        delete collision;
        collision = NULL;
    }
}

Magic3D::PhysicsObject* Magic3D::PhysicsObject::spawn() const
{
    return (new PhysicsObject(*this));
}

bool Magic3D::PhysicsObject::updateTweens()
{
    bool result = true;
    bool update = false;

    std::vector<Tween*>::const_iterator it_m = tweens.begin();
    while (it_m != tweens.end())
    {
        Tween* tween = *it_m++;

        update = tween->update();
        result = result && update;
    }

    return result;
}

void Magic3D::PhysicsObject::addConstraint(PhysicsConstraint* constraint)
{
    if (constraint->getPhysicsObject())
    {
        constraint->getPhysicsObject()->removeConstraint(constraint->getPhysicsObject()->getConstraintIndex(constraint));
    }
    constraint->setPhysicsObject(this);
    constraints.push_back(constraint);
}

void Magic3D::PhysicsObject::removeConstraint(int index)
{
    if (index >= 0 && index < (int)constraints.size())
    {
        PhysicsConstraint* constraint = constraints.at(index);
        constraints.erase(constraints.begin() + index);
        if (constraint)
        {
            constraint->disconnect();
            delete constraint;
        }
    }
}

Magic3D::PhysicsConstraint* Magic3D::PhysicsObject::getConstraint(int index)
{
    PhysicsConstraint* result = NULL;
    if (index >= 0 && index < (int)constraints.size())
    {
        result = constraints.at(index);
    }

    return result;
}

int Magic3D::PhysicsObject::getConstraintIndex(PhysicsConstraint* constraint)
{
    int result = -1;
    std::vector<PhysicsConstraint*>::const_iterator it_c = constraints.begin();
    int index = 0;
    while (it_c != constraints.end())
    {
        PhysicsConstraint* c = *it_c++;

        if (constraint == c)
        {
            result = index;
            break;
        }
        index++;
    }

    return result;
}

int Magic3D::PhysicsObject::getConstraintCount()
{
    return constraints.size();
}

void Magic3D::PhysicsObject::clearConstraints()
{
    while (!constraints.empty())
    {
        PhysicsConstraint* constraint = *constraints.begin();
        constraints.erase(constraints.begin());
        if (constraint)
        {
            Physics::getInstance()->removeConstraint(constraint);
            delete constraint;
        }
    }
}

std::vector<Magic3D::PhysicsConstraint*>* Magic3D::PhysicsObject::getConstraints()
{
    return &constraints;
}

void Magic3D::PhysicsObject::addTween(Tween* tween)
{
    if (tween->getPhysicsObject())
    {
        tween->getPhysicsObject()->removeTween(tween->getPhysicsObject()->getTweenIndex(tween));
    }
    tween->setPhysicsObject(this);
    tweens.push_back(tween);
}

void Magic3D::PhysicsObject::removeTween(int index)
{
    if (index >= 0 && index < (int)tweens.size())
    {
        Tween* tween = tweens.at(index);
        tweens.erase(tweens.begin() + index);
        if (tween)
        {
            delete tween;
        }
    }
}

Magic3D::Tween* Magic3D::PhysicsObject::getTween(int index)
{
    Tween* result = NULL;
    if (index >= 0 && index < (int)tweens.size())
    {
        result = tweens.at(index);
    }

    return result;
}

int Magic3D::PhysicsObject::getTweenIndex(Tween* tween)
{
    int result = -1;
    std::vector<Tween*>::const_iterator it_m = tweens.begin();
    int index = 0;
    while (it_m != tweens.end())
    {
        Tween* t = *it_m++;

        if (tween == t)
        {
            result = index;
            break;
        }
        index++;
    }

    return result;
}

int Magic3D::PhysicsObject::getTweenCount()
{
    return tweens.size();
}

void Magic3D::PhysicsObject::clearTweens()
{
    while (!tweens.empty())
    {
        Tween* tween = *tweens.begin();
        tweens.erase(tweens.begin());
        if (tween)
        {
            delete tween;
        }
    }
}

std::vector<Magic3D::Tween*>* Magic3D::PhysicsObject::getTweens()
{
    return &tweens;
}

Magic3D::OBJECT Magic3D::PhysicsObject::getType()
{
    return type;
}

void Magic3D::PhysicsObject::setPosition(Vector3 position)
{
    if (position.getX())
    {

    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPosition()
{
    return Vector3(0.0f, 0.0f, 0.0f);
}

void Magic3D::PhysicsObject::setRotation(Quaternion rotation)
{
    if (rotation.getW())
    {

    }
}

Magic3D::Quaternion Magic3D::PhysicsObject::getRotation()
{
    return Quaternion::identity();
}

void Magic3D::PhysicsObject::setScale(Vector3 scale)
{
    if (scale.getX())
    {

    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getScale()
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Magic3D::RENDER Magic3D::PhysicsObject::getRender()
{
    return eRENDER_3D;
}

void Magic3D::PhysicsObject::lookAt(Vector3 position, Vector3 up)
{
    if (position.getX() && up.getX())
    {

    }
}

void Magic3D::PhysicsObject::setRigidBody(btRigidBody* body)
{
    this->body = body;
}

btRigidBody* Magic3D::PhysicsObject::getRigidBody()
{
    return body;
}

void Magic3D::PhysicsObject::setShape(PHYSICS_SHAPE shape)
{
    if (getType() != eOBJECT_BONE)
    {
        static_cast<Object*>(this)->setCollisionMeshIndex(-1);
    }
    this->shape = shape;
}

Magic3D::PHYSICS_SHAPE Magic3D::PhysicsObject::getShape()
{
    return shape;
}

void Magic3D::PhysicsObject::setShapeOwner(bool owner)
{
    this->shapeOwner = owner;
}

bool Magic3D::PhysicsObject::isShapeOwner()
{
    return shapeOwner;
}

void Magic3D::PhysicsObject::setShapePosition(Vector3 position)
{
    shapePosition = position;
    needTransform = true;

}

const Magic3D::Vector3& Magic3D::PhysicsObject::getShapePosition()
{
    return shapePosition;
}

void Magic3D::PhysicsObject::setShapeSize(Vector3 size)
{
    shapeSize = size;
}

const Magic3D::Vector3& Magic3D::PhysicsObject::getShapeSize()
{
    return shapeSize;
}

void Magic3D::PhysicsObject::setShapeRotation(Vector3 rotation)
{
    shapeRotation = rotation;
    needTransform = true;
}

const Magic3D::Vector3& Magic3D::PhysicsObject::getShapeRotation()
{
    return shapeRotation;
}

void Magic3D::PhysicsObject::setShapeTranslation(Vector3 translation)
{
    shapeTranslation = translation;
    needTransform = true;
}

const Magic3D::Vector3& Magic3D::PhysicsObject::getShapeTranslation()
{
    return shapeTranslation;
}

void Magic3D::PhysicsObject::setRadius(float radius)
{
    this->radius = radius;
}

float Magic3D::PhysicsObject::getRadius()
{
    return radius;
}

void Magic3D::PhysicsObject::setMargin(float margin)
{
    this->margin = margin;
}

float Magic3D::PhysicsObject::getMargin()
{
    return margin;
}

void Magic3D::PhysicsObject::setMass(float mass)
{
    this->mass = mass;
}

float Magic3D::PhysicsObject::getMass()
{
    return mass;
}

void Magic3D::PhysicsObject::setFriction(float friction)
{
    this->friction = friction;
}

float Magic3D::PhysicsObject::getFriction()
{
    return friction;
}

void Magic3D::PhysicsObject::setRestitution(float restitution)
{
    this->restitution = restitution;
}

float Magic3D::PhysicsObject::getRestitution()
{
    return restitution;
}

void Magic3D::PhysicsObject::setDampingLinear(float damping)
{
    this->dampingLinear = damping;
}

float Magic3D::PhysicsObject::getDampingLinear()
{
    return dampingLinear;
}

void Magic3D::PhysicsObject::setDampingAngular(float damping)
{
    this->dampingAngular = damping;
}

float Magic3D::PhysicsObject::getDampingAngular()
{
    return dampingAngular;
}

void Magic3D::PhysicsObject::setGhost(bool ghost)
{
    this->ghost = ghost;
}

bool Magic3D::PhysicsObject::isGhost()
{
    return ghost;
}

void Magic3D::PhysicsObject::setPhysicsCollision(PhysicsCollision* collision)
{
    if (this->collision)
    {
        delete this->collision;
    }

    this->collision = collision;
}

Magic3D::PhysicsCollision* Magic3D::PhysicsObject::getPhysicsCollision()
{
    return collision;
}

void Magic3D::PhysicsObject::activatePhysics(bool activate)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(activate);
    }
}

bool Magic3D::PhysicsObject::isActivatedPhysics()
{
    return getRigidBody() && getRigidBody()->isActive();
}

void Magic3D::PhysicsObject::applyPhysicsCentralForce(Vector3 force)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyCentralForce(btVector3(force.getX(), force.getY(), force.getZ()));
    }
}

void Magic3D::PhysicsObject::applyPhysicsCentralImpulse(Vector3 impulse)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyCentralImpulse(btVector3(impulse.getX(), impulse.getY(), impulse.getZ()));
    }
}

void Magic3D::PhysicsObject::applyPhysicsDamping(float damping)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyDamping(damping);
    }
}

void Magic3D::PhysicsObject::applyPhysicsForce(Vector3 force, Vector3 position)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyForce(btVector3(force.getX(), force.getY(), force.getZ()), btVector3(position.getX(), position.getY(), position.getZ()));
    }
}

void Magic3D::PhysicsObject::applyPhysicsGravity()
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyGravity();
    }
}

void Magic3D::PhysicsObject::applyPhysicsImpulse(Vector3 impulse, Vector3 position)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyImpulse(btVector3(impulse.getX(), impulse.getY(), impulse.getZ()), btVector3(position.getX(), position.getY(), position.getZ()));
    }
}

void Magic3D::PhysicsObject::applyPhysicsTorque(Vector3 torque)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyTorque(btVector3(torque.getX(), torque.getY(), torque.getZ()));
    }
}

void Magic3D::PhysicsObject::applyPhysicsTorqueImpulse(Vector3 torque)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->applyTorqueImpulse(btVector3(torque.getX(), torque.getY(), torque.getZ()));
    }
}

void Magic3D::PhysicsObject::setPhysicsFriction(float friction)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setFriction(friction);
    }
}

float Magic3D::PhysicsObject::getPhysicsFriction()
{
    return getRigidBody() ? getRigidBody()->getFriction() : 0.0f;
}

void Magic3D::PhysicsObject::setPhysicsGravity(Vector3 gravity)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setGravity(btVector3(gravity.getX(), gravity.getY(), gravity.getZ()));
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsGravity()
{
    btVector3 g(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        g = getRigidBody()->getGravity();
    }
    return Vector3(g.getX(), g.getY(), g.getZ());
}

void Magic3D::PhysicsObject::setPhysicsDamping(float linear, float angular)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setDamping(linear, angular);
    }
}

float Magic3D::PhysicsObject::getPhysicsAngularDamping()
{
    return getRigidBody() ? getRigidBody()->getAngularDamping() : 0.0f;
}

float Magic3D::PhysicsObject::getPhysicsLinearDamping()
{
    return getRigidBody() ? getRigidBody()->getLinearDamping() : 0.0f;
}

void Magic3D::PhysicsObject::setPhysicsAngularFactor(Vector3 factor)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setAngularFactor(btVector3(factor.getX(), factor.getY(), factor.getZ()));
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsAngularFactor()
{
    btVector3 f(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        f = getRigidBody()->getAngularFactor();
    }
    return Vector3(f.getX(), f.getY(), f.getZ());
}

void Magic3D::PhysicsObject::setPhysicsCentreOfMass(Vector3 position)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        btTransform t(btQuaternion::getIdentity(), btVector3(position.getX(), position.getY(), position.getZ()));
        getRigidBody()->setCenterOfMassTransform(t);
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsCentreOfMass()
{
    btVector3 c(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        c = getRigidBody()->getCenterOfMassPosition();
    }
    return Vector3(c.getX(), c.getY(), c.getZ());
}

void Magic3D::PhysicsObject::setPhysicsAngularVelocity(Vector3 velocity)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setAngularVelocity(btVector3(velocity.getX(), velocity.getY(), velocity.getZ()));
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsAngularVelocity()
{
    btVector3 v(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        v = getRigidBody()->getAngularVelocity();
    }
    return Vector3(v.getX(), v.getY(), v.getZ());
}

void Magic3D::PhysicsObject::setPhysicsLinearVelocity(Vector3 velocity)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setLinearVelocity(btVector3(velocity.getX(), velocity.getY(), velocity.getZ()));
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsLinearVelocity()
{
    btVector3 v(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        v = getRigidBody()->getLinearVelocity();
    }
    return Vector3(v.getX(), v.getY(), v.getZ());
}

void Magic3D::PhysicsObject::setPhysicsLinearFactor(Vector3 factor)
{
    if (getRigidBody())
    {
        getRigidBody()->activate(true);
        getRigidBody()->setLinearFactor(btVector3(factor.getX(), factor.getY(), factor.getZ()));
    }
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsLinearFactor()
{
    btVector3 f(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        f = getRigidBody()->getLinearFactor();
    }
    return Vector3(f.getX(), f.getY(), f.getZ());
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsTotalForce()
{
    btVector3 f(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        f = getRigidBody()->getTotalForce();
    }
    return Vector3(f.getX(), f.getY(), f.getZ());
}

Magic3D::Vector3 Magic3D::PhysicsObject::getPhysicsTotalTorque()
{
    btVector3 t(0.0f, 0.0f, 0.0f);
    if (getRigidBody())
    {
        t = getRigidBody()->getTotalTorque();
    }
    return Vector3(t.getX(), t.getY(), t.getZ());
}

bool Magic3D::PhysicsObject::isPhysicsSleeping()
{
    bool result = true;
    if (getRigidBody())
    {
        result = getRigidBody()->getActivationState() == ISLAND_SLEEPING;
    }

    return result;
}

void Magic3D::PhysicsObject::resetPhysics()
{
    Physics::getInstance()->reset(this);
}

Magic3D::Object* Magic3D::PhysicsObject::getAsObject()
{
    return getType() == eOBJECT_BONE ? static_cast<Bone*>(this)->getSkeleton()->getObject() : static_cast<Object*>(this);
}

Magic3D::XMLElement* Magic3D::PhysicsObject::save(XMLElement* root)
{
    if (root)
    {
        XMLElement* physics = root->GetDocument()->NewElement( M3D_PHYSICS_OBJECT_XML );
        root->LinkEndChild( physics );
        saveBool(physics,    M3D_PHYSICS_OBJECT_XML_PHYSICS,          getRigidBody());
        saveInt(physics,     M3D_PHYSICS_OBJECT_XML_SHAPE,            getShape());
        saveVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPEPOSITION,    getShapePosition());
        saveVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPESIZE,        getShapeSize());
        saveVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPEROTATION,    getShapeRotation());
        saveVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPETRANSLATION, getShapeTranslation());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_RADIUS,           getRadius());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_MARGIN,           getMargin());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_MASS,             getMass());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_FRICTION,         getFriction());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_RESTITUTION,      getRestitution());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_DAMPINGLINEAR,    getDampingLinear());
        saveFloat(physics,   M3D_PHYSICS_OBJECT_XML_DAMPINGANGULAR,   getDampingAngular());
        saveBool(physics,    M3D_PHYSICS_OBJECT_XML_GHOST,            isGhost());

        if (getType() != eOBJECT_BONE)
        {
            saveInt(physics, M3D_OBJECT_XML_COLLISIONMESH, static_cast<Object*>(this)->getCollisionMeshIndex());
        }

        if (getConstraintCount() > 0)
        {
            XMLElement* constraintXML = root->GetDocument()->NewElement( M3D_PHYSICS_CONSTRAINTS_XML );
            constraintXML->SetAttribute("constraints", getConstraintCount());
            root->LinkEndChild( constraintXML );

            std::vector<PhysicsConstraint*>::const_iterator it_c = constraints.begin();
            while (it_c != constraints.end())
            {
                PhysicsConstraint* constraint = *it_c++;

                constraint->save(constraintXML);
            }
        }

        if (getTweenCount() > 0)
        {
            XMLElement* tweenXML = root->GetDocument()->NewElement( M3D_TWEENS_XML );
            tweenXML->SetAttribute("tweens", getTweenCount());
            root->LinkEndChild( tweenXML );

            std::vector<Tween*>::const_iterator it_t = tweens.begin();
            while (it_t != tweens.end())
            {
                Tween* tween = *it_t++;

                tween->save(tweenXML);
            }
        }
    }
    return root;
}

Magic3D::XMLElement* Magic3D::PhysicsObject::load(XMLElement* root)
{
    if (root)
    {
        XMLElement* physics = root->FirstChildElement(M3D_PHYSICS_OBJECT_XML);
        shape            = PHYSICS_SHAPE(loadInt(physics, M3D_PHYSICS_OBJECT_XML_SHAPE));
        shapePosition    = loadVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPEPOSITION);
        shapeSize        = loadVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPESIZE);
        shapeRotation    = loadVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPEROTATION);
        shapeTranslation = loadVector3(physics, M3D_PHYSICS_OBJECT_XML_SHAPETRANSLATION);
        radius           = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_RADIUS);
        margin           = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_MARGIN);
        mass             = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_MASS);
        friction         = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_FRICTION);
        restitution      = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_RESTITUTION);
        dampingLinear    = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_DAMPINGLINEAR);
        dampingAngular   = loadFloat(physics, M3D_PHYSICS_OBJECT_XML_DAMPINGANGULAR);
        ghost            = loadBool(physics, M3D_PHYSICS_OBJECT_XML_GHOST);

        XMLElement* xml = root->FirstChildElement(M3D_PHYSICS_CONSTRAINTS_XML);
        if (constraints.empty() && xml)
        {
            xml = xml->FirstChildElement(M3D_PHYSICS_CONSTRAINT_XML);
            while (xml)
            {
                CONSTRAINT constraintType = (CONSTRAINT)loadInt(xml, M3D_TWEEN_XML_TYPE);
                PhysicsConstraint* constraint = new PhysicsConstraint(constraintType);

                if (constraint)
                {
                    constraint->load(xml);
                    addConstraint(constraint);
                }

                xml = xml->NextSiblingElement(M3D_PHYSICS_CONSTRAINT_XML);
            }
        }

        xml = root->FirstChildElement(M3D_TWEENS_XML);
        if (tweens.empty() && xml)
        {
            xml = xml->FirstChildElement(M3D_TWEEN_XML);
            while (xml)
            {
                TWEEN tweenType = (TWEEN)loadInt(xml, M3D_TWEEN_XML_TYPE);

                Tween* tween = NULL;
                switch (tweenType)
                {
                    case eTWEEN_ALPHA:     tween = new TweenAlpha(); break;
                    case eTWEEN_DELAY:     tween = new TweenDelay(); break;
                    case eTWEEN_ROTATE:    tween = new TweenRotate(); break;
                    case eTWEEN_SCALE:     tween = new TweenScale(); break;
                    case eTWEEN_TRANSLATE: tween = new TweenTranslate(); break;
                    case eTWEEN_GOTO:      tween = new TweenGoTo(); break;
                    case eTWEEN_LOOKAT:    tween = new TweenLookAt(); break;
                    case eTWEEN_CUSTOM:    tween = new Tween(eTWEEN_CUSTOM); break;
                }

                if (tween)
                {
                    tween->load(xml);
                    addTween(tween);
                }

                xml = xml->NextSiblingElement(M3D_TWEEN_XML);
            }
        }

        if (!getRigidBody())
        {
            bool needCollisionMesh = getShape() == ePHYSICS_SHAPE_CONVEXHULL || getShape() == ePHYSICS_SHAPE_TRIANGLEMESH;

            if (needCollisionMesh && getType() != eOBJECT_BONE)
            {
                static_cast<Object*>(this)->setCollisionMeshIndex(loadInt(physics, M3D_OBJECT_XML_COLLISIONMESH));
            }

            if ((getType() == eOBJECT_BONE || getType() != eOBJECT_MODEL) && loadBool(physics, M3D_PHYSICS_OBJECT_XML_PHYSICS))
            {
                Physics::getInstance()->add(this);
            }
        }
        else
        {
            Physics::getInstance()->reset(this);
        }
    }

    return root;
}
