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

#include <magic3d/physics/physics_constraint.h>
#include <magic3d/magic3d.h>

Magic3D::PhysicsConstraint::PhysicsConstraint(const PhysicsConstraint& constraint)
{
    this->type             = constraint.type;
    this->constraint       = NULL;
    this->physicsObject    = NULL;
    this->connected        = NULL;

    this->connectedObject  = constraint.connectedObject;
    this->connectedBone    = constraint.connectedBone;

    this->pivot          = constraint.pivot;
    this->axis           = constraint.axis;
    this->connectedPivot = constraint.connectedPivot;
    this->connectedAxis  = constraint.connectedAxis;

    this->linearLowerLimit  = constraint.linearLowerLimit;
    this->linearUpperLimit  = constraint.linearUpperLimit;
    this->angularLowerLimit = constraint.angularLowerLimit;
    this->angularUpperLimit = constraint.angularUpperLimit;

    this->breakingImpulse = constraint.breakingImpulse;

    this->limitLow   = constraint.limitLow;
    this->limitHigh  = constraint.limitHigh;
    this->swingSpan1 = constraint.swingSpan1;
    this->swingSpan2 = constraint.swingSpan2;
    this->twistSpan  = constraint.twistSpan;
    this->softness   = constraint.softness;
    this->bias       = constraint.bias;
    this->relaxation = constraint.relaxation;
    this->damping    = constraint.damping;
    this->thresh     = constraint.thresh;

    this->breakable   = constraint.breakable;
    this->angularOnly = constraint.angularOnly;

    this->enabled           = constraint.enabled;

    if (this->enabled)
    {
        connect();
    }
}

Magic3D::PhysicsConstraint::PhysicsConstraint(CONSTRAINT type)
{
    this->type       = type;
    constraint       = NULL;
    physicsObject    = NULL;
    connected        = NULL;

    connectedObject.clear();
    connectedBone.clear();

    pivot          = Vector3(0.0f, 0.0f, 0.0f);
    axis           = Vector3(0.0f, 1.0f, 0.0f);
    connectedPivot = Vector3(0.0f, 0.0f, 0.0f);
    connectedAxis  = Vector3(0.0f, 1.0f, 0.0f);

    linearLowerLimit  = Vector3(0.0f, 0.0f, 0.0f);
    linearUpperLimit  = Vector3(0.0f, 0.0f, 0.0f);
    angularLowerLimit = Vector3(0.0f, 0.0f, 0.0f);
    angularUpperLimit = Vector3(0.0f, 0.0f, 0.0f);

    breakingImpulse = SIMD_INFINITY;

    limitLow   = 1.0f;
    limitHigh  = -1.0f;
    swingSpan1 = BT_LARGE_FLOAT;
    swingSpan2 = BT_LARGE_FLOAT;
    twistSpan  = BT_LARGE_FLOAT;
    softness   = M3D_PHYSICS_CONSTRAINT_DEFAULT_SOFTNESS;
    bias       = M3D_PHYSICS_CONSTRAINT_DEFAULT_BIAS;
    relaxation = M3D_PHYSICS_CONSTRAINT_DEFAULT_RELAXATION;
    damping    = 0.01f;
    thresh     = 0.05f;

    breakable   = false;
    angularOnly = false;

    enabled = false;
}

Magic3D::PhysicsConstraint::~PhysicsConstraint()
{
    disconnect();

    physicsObject   = NULL;
}

Magic3D::PhysicsConstraint* Magic3D::PhysicsConstraint::spawn() const
{
    return (new PhysicsConstraint(*this));
}

Magic3D::CONSTRAINT Magic3D::PhysicsConstraint::getType()
{
    return type;
}

btTypedConstraint* Magic3D::PhysicsConstraint::getConstraint()
{
    return constraint;
}

void Magic3D::PhysicsConstraint::setPhysicsObject(PhysicsObject* physicsObject)
{
    this->physicsObject = physicsObject;
}

Magic3D::PhysicsObject* Magic3D::PhysicsConstraint::getPhysicsObject()
{
    return physicsObject;
}

void Magic3D::PhysicsConstraint::setConnected(std::string object, std::string bone)
{
    this->connectedObject = object;
    this->connectedBone = bone;
}

const std::string& Magic3D::PhysicsConstraint::getConnectedObject()
{
    return connectedObject;
}

const std::string& Magic3D::PhysicsConstraint::getConnectedBone()
{
    return connectedBone;
}

Magic3D::PhysicsObject* Magic3D::PhysicsConstraint::getConnected()
{
    if (!connected || (connectedObject.empty() && connected) || (!connectedBone.empty() && connected->getAsObject()->getName().compare(connectedBone) != 0) || (connectedBone.empty() && connected->getType() == eOBJECT_BONE))
    {
        if (!connectedObject.empty())
        {
            PhysicsObject* obj = ResourceManager::getObjects()->get(connectedObject);
            if (obj)
            {
                if (!connectedBone.empty() && obj->getType() == eOBJECT_MODEL)
                {
                    obj = static_cast<Model*>(obj)->getSkeleton(true)->getBoneByName(connectedBone);
                    if (obj && !obj->getRigidBody())
                    {
                        obj = NULL;
                    }
                }
                setConnected(obj);
            }
            else
            {
                setConnected(NULL);
            }

            if (!connected)
            {
                connectedObject.clear();
                connectedBone.clear();
            }
        }
        else
        {
            setConnected(NULL);
        }
    }

    return connected;
}

void Magic3D::PhysicsConstraint::setConnected(PhysicsObject* connected)
{
    if (this->connected)
    {
        this->connected->removeReference(this);
    }

    this->connected = connected;

    if (this->connected)
    {
        this->connected->addReference(this);
    }
}

void Magic3D::PhysicsConstraint::connect()
{
    if (!physicsObject || !physicsObject->getRigidBody())
    {
        return;
    }

    Physics::getInstance()->removeConstraint(this);
    getConnected();


    if (constraint)
    {
        if ((connected && &constraint->getRigidBodyB() != connected->getRigidBody()))
        {
            delete constraint;
            constraint = NULL;
        }
    }

    btVector3 p = btVector3(pivot.getX(), pivot.getY(), pivot.getZ());
    btVector3 a = btVector3(axis.getX(), axis.getY(), axis.getZ());

    btVector3 pc = btVector3(connectedPivot.getX(), connectedPivot.getY(), connectedPivot.getZ());
    btVector3 ac = btVector3(connectedAxis.getX(), connectedAxis.getY(), connectedAxis.getZ());

    btTransform frameInA = btTransform::getIdentity();
    btTransform frameInB = btTransform::getIdentity();
    frameInA.setOrigin(p);
    frameInB.setOrigin(pc);
    frameInA.getBasis().setEulerZYX(a.getX(), a.getY(), a.getZ());
    frameInB.getBasis().setEulerZYX(ac.getX(), ac.getY(), ac.getZ());

    bool referenceFrameA = false;

    switch (getType())
    {
        case eCONSTRAINT_CUSTOM:
        {
            if (!constraint)
            {
                if (connected)
                {
                    constraint = new btGeneric6DofConstraint(*physicsObject->getRigidBody(), *connected->getRigidBody(), frameInA, frameInB, referenceFrameA);
                }
                else
                {
                    constraint = new btGeneric6DofConstraint(*physicsObject->getRigidBody(), frameInA, referenceFrameA);
                }
            }

            btGeneric6DofConstraint* sixDOF = static_cast<btGeneric6DofConstraint*>(constraint);
            sixDOF->setFrames(frameInA, frameInB);
            sixDOF->setLinearLowerLimit(btVector3(linearLowerLimit.getX(), linearLowerLimit.getY(), linearLowerLimit.getZ()));
            sixDOF->setLinearUpperLimit(btVector3(linearUpperLimit.getX(), linearUpperLimit.getY(), linearUpperLimit.getZ()));
            sixDOF->setAngularLowerLimit(btVector3(angularLowerLimit.getX(), angularLowerLimit.getY(), angularLowerLimit.getZ()));
            sixDOF->setAngularUpperLimit(btVector3(angularUpperLimit.getX(), angularUpperLimit.getY(), angularUpperLimit.getZ()));
            break;
        }
        case eCONSTRAINT_POINT_TO_POINT:
        {
            if (!constraint)
            {
                if (connected)
                {
                    constraint = new btPoint2PointConstraint(*physicsObject->getRigidBody(), *connected->getRigidBody(), frameInA.getOrigin(), frameInB.getOrigin());
                }
                else
                {
                    constraint = new btPoint2PointConstraint(*physicsObject->getRigidBody(), frameInA.getOrigin());
                }
            }

            btPoint2PointConstraint* point2point = static_cast<btPoint2PointConstraint*>(constraint);
            point2point->setPivotA(frameInA.getOrigin());
            point2point->setPivotB(frameInB.getOrigin());

            break;
        }
        case eCONSTRAINT_HINGE:
        {
            if (!constraint)
            {
                if (connected)
                {
                    constraint = new btHingeConstraint(*physicsObject->getRigidBody(), *connected->getRigidBody(), frameInA, frameInB, referenceFrameA);
                }
                else
                {
                    constraint = new btHingeConstraint(*physicsObject->getRigidBody(), frameInA, referenceFrameA);
                }
            }

            btHingeConstraint* hinge = static_cast<btHingeConstraint*>(constraint);
            hinge->setFrames(frameInA, frameInB);
            hinge->setAngularOnly(angularOnly);
            hinge->setLimit(limitLow, limitHigh, softness, bias, relaxation);
            break;
        }
        case eCONSTRAINT_SLIDER:
        {
            if (!constraint)
            {
                if (connected)
                {
                    constraint = new btSliderConstraint(*physicsObject->getRigidBody(), *connected->getRigidBody(), frameInA, frameInB, referenceFrameA);
                }
                else
                {
                    constraint = new btSliderConstraint(*physicsObject->getRigidBody(), frameInA, referenceFrameA);
                }
            }

            btSliderConstraint* slider = static_cast<btSliderConstraint*>(constraint);
            slider->setFrames(frameInA, frameInB);

            /*slider->setDampingDirAng(0.0f);
            slider->setDampingDirLin(0.0f);
            slider->setDampingLimAng(0.0f);
            slider->setDampingLimLin(0.0f);
            slider->setDampingOrthoAng(0.0f);
            slider->setDampingOrthoLin(0.0f);

            slider->setRestitutionDirAng(0.0f);
            slider->setRestitutionDirLin(0.0f);
            slider->setRestitutionLimAng(0.0f);
            slider->setRestitutionLimLin(0.0f);
            slider->setRestitutionOrthoAng(0.0f);
            slider->setRestitutionOrthoLin(0.0f);

            slider->setSoftnessDirAng(0.0f);
            slider->setSoftnessDirLin(0.0f);
            slider->setSoftnessLimAng(0.0f);
            slider->setSoftnessLimLin(0.0f);
            slider->setSoftnessOrthoAng(0.0f);
            slider->setSoftnessOrthoLin(0.0f);

            slider->setLowerAngLimit(0.0f);
            slider->setLowerLinLimit(0.0f);
            slider->setUpperAngLimit(0.0f);
            slider->setUpperLinLimit(0.0f);*/
            break;
        }
        case eCONSTRAINT_CONE:
        {
            if (!constraint)
            {
                if (connected)
                {
                    constraint = new btConeTwistConstraint(*physicsObject->getRigidBody(), *connected->getRigidBody(), frameInA, frameInB);
                }
                else
                {
                    constraint = new btConeTwistConstraint(*physicsObject->getRigidBody(), frameInA);
                }
            }

            btConeTwistConstraint* cone = static_cast<btConeTwistConstraint*>(constraint);
            cone->setFrames(frameInA, frameInB);
            cone->setAngularOnly(angularOnly);
            cone->setDamping(damping);
            cone->setFixThresh(thresh);
            cone->setLimit(swingSpan1, swingSpan2, twistSpan, softness, bias, relaxation);
            break;
        }
        default: break;
    }

    if (constraint)
    {
        constraint->setBreakingImpulseThreshold(breakingImpulse);
    }
    Physics::getInstance()->addConstraint(this);
    enabled = true;
}

void Magic3D::PhysicsConstraint::disconnect()
{
    Physics::getInstance()->removeConstraint(this);
    if (constraint)
    {
        delete constraint;
        constraint = NULL;
    }
    setConnected(NULL);
    enabled = false;
}

bool Magic3D::PhysicsConstraint::isEnabled()
{
    return enabled;
}

void Magic3D::PhysicsConstraint::setPivot(Vector3 pivot)
{
    this->pivot = pivot;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getPivot()
{
    return pivot;
}

void Magic3D::PhysicsConstraint::setAxis(Vector3 axis)
{
    this->axis = axis;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getAxis()
{
    return axis;
}

void Magic3D::PhysicsConstraint::setConnectedPivot(Vector3 pivot)
{
    this->connectedPivot = pivot;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getConnectedPivot()
{
    return connectedPivot;
}

void Magic3D::PhysicsConstraint::setConnectedAxis(Vector3 axis)
{
    this->connectedAxis = axis;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getConnectedAxis()
{
    return this->connectedAxis;
}

void Magic3D::PhysicsConstraint::setLinearLowerLimit(Vector3 limit)
{
    this->linearLowerLimit = limit;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getLinearLowerLimit()
{
    return linearLowerLimit;
}

void Magic3D::PhysicsConstraint::setLinearUpperLimit(Vector3 limit)
{
    this->linearUpperLimit = limit;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getLinearUpperLimit()
{
    return linearUpperLimit;
}

void Magic3D::PhysicsConstraint::setAngularLowerLimit(Vector3 limit)
{
    this->angularLowerLimit = limit;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getAngularLowerLimit()
{
    return angularLowerLimit;
}

void Magic3D::PhysicsConstraint::setAngularUpperLimit(Vector3 limit)
{
    this->angularUpperLimit = limit;
}

Magic3D::Vector3 Magic3D::PhysicsConstraint::getAngularUpperLimit()
{
    return angularUpperLimit;
}

void Magic3D::PhysicsConstraint::setBreakingImpulse(float impulse)
{
    breakingImpulse = impulse;
}

float Magic3D::PhysicsConstraint::getBreakingImpulse()
{
    return breakingImpulse;
}

void Magic3D::PhysicsConstraint::setHingeLimit(float low, float high, float softness, float bias, float relaxation)
{
    limitLow = low;
    limitHigh = high;
    this->softness = softness;
    this->bias = bias;
    this->relaxation = relaxation;
}

void Magic3D::PhysicsConstraint::setConeLimit(float swingSpan1, float swingSpan2, float twistSpan, float softness, float bias, float relaxation)
{
    this->swingSpan1 = swingSpan1;
    this->swingSpan2 = swingSpan2;
    this->twistSpan = twistSpan;
    this->softness = softness;
    this->bias = bias;
    this->relaxation = relaxation;
}

float Magic3D::PhysicsConstraint::getLimitLow()
{
    return limitLow;
}

float Magic3D::PhysicsConstraint::getLimitHigh()
{
    return limitHigh;
}

float Magic3D::PhysicsConstraint::getSwingSpan1()
{
    return swingSpan1;
}

float Magic3D::PhysicsConstraint::getSwingSpan2()
{
    return swingSpan2;
}

float Magic3D::PhysicsConstraint::getTwistSpan()
{
    return twistSpan;
}

float Magic3D::PhysicsConstraint::getSoftness()
{
    return softness;
}

float Magic3D::PhysicsConstraint::getBias()
{
    return bias;
}

float Magic3D::PhysicsConstraint::getRelaxation()
{
    return relaxation;
}

void Magic3D::PhysicsConstraint::setDamping(float damping)
{
    this->damping = damping;
}

float Magic3D::PhysicsConstraint::getDamping()
{
    return damping;
}

void Magic3D::PhysicsConstraint::setThresh(float thresh)
{
    this->thresh = thresh;
}

float Magic3D::PhysicsConstraint::getThresh()
{
    return thresh;
}

void Magic3D::PhysicsConstraint::setBreakable(bool breakable)
{
    this->breakable = breakable;
}

bool Magic3D::PhysicsConstraint::isBreakable()
{
    return breakable;
}

void Magic3D::PhysicsConstraint::setAngularOnly(bool angular)
{
    angularOnly = angular;
}

bool Magic3D::PhysicsConstraint::isAngularOnly()
{
    return angularOnly;
}

Magic3D::XMLElement* Magic3D::PhysicsConstraint::save(XMLElement* root)
{
    XMLElement* result = NULL;
    if (root)
    {
        result = root->GetDocument()->NewElement(M3D_PHYSICS_CONSTRAINT_XML);
        root->LinkEndChild(result);

        saveInt(result,      M3D_PHYSICS_CONSTRAINT_XML_TYPE,             type);
        saveString(result,   M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_OBJECT, connectedObject);
        saveString(result,   M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_BONE,   connectedBone);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_PIVOT,            pivot);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_AXIS,             axis);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_PIVOT,  connectedPivot);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_AXIS,   connectedAxis);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_LINEAR_LOWER,     linearLowerLimit);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_LINEAR_UPPER,     linearUpperLimit);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_LOWER,    angularLowerLimit);
        saveVector3(result,  M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_UPPER,    angularUpperLimit);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_BREAKING_IMPULSE, breakingImpulse);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_LIMIT_LOW,        limitLow);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_LIMIT_HIGH,       limitHigh);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN1,      swingSpan1);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN2,      swingSpan2);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_TWIST_SPAN,       twistSpan);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_SOFTNESS,         softness);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_BIAS,             bias);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_RELAXATION,       relaxation);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_DAMPING,          damping);
        saveFloat(result,    M3D_PHYSICS_CONSTRAINT_XML_THRESH,           thresh);
        saveBool(result,     M3D_PHYSICS_CONSTRAINT_XML_BREAKABLE,        breakable);
        saveBool(result,     M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_ONLY,     angularOnly);
        saveBool(result,     M3D_PHYSICS_CONSTRAINT_XML_ENABLED,          enabled);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::PhysicsConstraint::load(XMLElement* root)
{
    if (root)
    {
        type              = (CONSTRAINT)loadInt(root,   M3D_PHYSICS_CONSTRAINT_XML_TYPE);
        pivot             = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_PIVOT);
        axis              = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_AXIS);
        connectedPivot    = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_PIVOT);
        connectedAxis     = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_AXIS);
        linearLowerLimit  = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_LINEAR_LOWER);
        linearUpperLimit  = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_LINEAR_UPPER);
        angularLowerLimit = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_LOWER);
        angularUpperLimit = loadVector3(root,  M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_UPPER);
        breakingImpulse   = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_BREAKING_IMPULSE);
        limitLow          = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_LIMIT_LOW);
        limitHigh         = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_LIMIT_HIGH);
        swingSpan1        = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN1);
        swingSpan2        = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN2);
        twistSpan         = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_TWIST_SPAN);
        softness          = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_SOFTNESS);
        bias              = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_BIAS);
        relaxation        = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_RELAXATION);
        damping           = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_DAMPING);
        thresh            = loadFloat(root,     M3D_PHYSICS_CONSTRAINT_XML_THRESH);
        breakable         = loadBool(root,     M3D_PHYSICS_CONSTRAINT_XML_BREAKABLE);
        angularOnly       = loadBool(root,     M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_ONLY);

        connectedObject = loadString(root, M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_OBJECT);
        if (connectedObject.compare(M3D_XML_NULL) == 0)
        {
            connectedObject.clear();
        }

        connectedBone = loadString(root, M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_BONE);
        if (connectedBone.compare(M3D_XML_NULL) == 0)
        {
            connectedBone.clear();
        }

        enabled = loadBool(root, M3D_PHYSICS_CONSTRAINT_XML_ENABLED);
    }

    return root;
}

void Magic3D::PhysicsConstraint::removeReference(Xml* reference)
{
    if (reference)
    {
        disconnect();
    }
}
