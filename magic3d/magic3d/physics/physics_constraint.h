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

#ifndef MAGIC3D_PHYSICS_CONSTRAINT_H
#define MAGIC3D_PHYSICS_CONSTRAINT_H

#include <magic3d/xml.h>
#include <magic3d/physics/physics.h>

namespace Magic3D
{
#define M3D_PHYSICS_CONSTRAINT_DEFAULT_SOFTNESS   0.9f
#define M3D_PHYSICS_CONSTRAINT_DEFAULT_BIAS       0.3f
#define M3D_PHYSICS_CONSTRAINT_DEFAULT_RELAXATION 1.0f

#define M3D_PHYSICS_CONSTRAINT_XML                  "Constraint"
#define M3D_PHYSICS_CONSTRAINT_XML_TYPE             "type"
#define M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_OBJECT "object"
#define M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_BONE   "bone"
#define M3D_PHYSICS_CONSTRAINT_XML_PIVOT            "pivot"
#define M3D_PHYSICS_CONSTRAINT_XML_AXIS             "axis"
#define M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_PIVOT  "connected_pivot"
#define M3D_PHYSICS_CONSTRAINT_XML_CONNECTED_AXIS   "connected_axis"
#define M3D_PHYSICS_CONSTRAINT_XML_LINEAR_LOWER     "linear_lower_limit"
#define M3D_PHYSICS_CONSTRAINT_XML_LINEAR_UPPER     "linear_upper_limit"
#define M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_LOWER    "angular_lower_limit"
#define M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_UPPER    "angular_upper_limit"
#define M3D_PHYSICS_CONSTRAINT_XML_BREAKING_IMPULSE "breaking_impulse"
#define M3D_PHYSICS_CONSTRAINT_XML_LIMIT_LOW        "limit_low"
#define M3D_PHYSICS_CONSTRAINT_XML_LIMIT_HIGH       "limit_high"
#define M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN1      "swing_span1"
#define M3D_PHYSICS_CONSTRAINT_XML_SWING_SPAN2      "swing_span2"
#define M3D_PHYSICS_CONSTRAINT_XML_TWIST_SPAN       "twist_span"
#define M3D_PHYSICS_CONSTRAINT_XML_SOFTNESS         "softness"
#define M3D_PHYSICS_CONSTRAINT_XML_BIAS             "bias"
#define M3D_PHYSICS_CONSTRAINT_XML_RELAXATION       "relaxation"
#define M3D_PHYSICS_CONSTRAINT_XML_DAMPING          "damping"
#define M3D_PHYSICS_CONSTRAINT_XML_THRESH           "thresh"
#define M3D_PHYSICS_CONSTRAINT_XML_BREAKABLE        "breakable"
#define M3D_PHYSICS_CONSTRAINT_XML_ANGULAR_ONLY     "angular_only"
#define M3D_PHYSICS_CONSTRAINT_XML_ENABLED          "enabled"

enum CONSTRAINT
{
    eCONSTRAINT_CUSTOM,
    eCONSTRAINT_POINT_TO_POINT,
    eCONSTRAINT_HINGE,
    eCONSTRAINT_SLIDER,
    eCONSTRAINT_CONE
};

class PhysicsObject;
class PhysicsConstraint : public Xml
{
private:
    CONSTRAINT type;
    btTypedConstraint* constraint;
    PhysicsObject* physicsObject;
    PhysicsObject* connected;

    std::string connectedObject;
    std::string connectedBone;

    Vector3 pivot;
    Vector3 axis;
    Vector3 connectedPivot;
    Vector3 connectedAxis;

    Vector3 linearLowerLimit;
    Vector3 linearUpperLimit;
    Vector3 angularLowerLimit;
    Vector3 angularUpperLimit;

    float breakingImpulse;
    float limitLow;
    float limitHigh;
    float swingSpan1;
    float swingSpan2;
    float twistSpan;
    float softness;
    float bias;
    float relaxation;
    float damping;
    float thresh;

    bool breakable;
    bool angularOnly;
    bool enabled;

    void setConnected(PhysicsObject* connected);

protected:
    PhysicsConstraint(const PhysicsConstraint& constraint);

public:
    PhysicsConstraint(CONSTRAINT type);
    virtual ~PhysicsConstraint();
    virtual PhysicsConstraint* spawn() const;

    CONSTRAINT getType();

    btTypedConstraint* getConstraint();

    void setPhysicsObject(PhysicsObject* physicsObject);
    PhysicsObject* getPhysicsObject();

    void setConnected(std::string object, std::string bone);
    const std::string& getConnectedObject();
    const std::string& getConnectedBone();
    PhysicsObject* getConnected();

    void connect();
    void disconnect();
    bool isEnabled();

    void setPivot(Vector3 pivot);
    Vector3 getPivot();
    void setAxis(Vector3 axis);
    Vector3 getAxis();
    void setConnectedPivot(Vector3 pivot);
    Vector3 getConnectedPivot();
    void setConnectedAxis(Vector3 axis);
    Vector3 getConnectedAxis();

    void setLinearLowerLimit(Vector3 limit);
    Vector3 getLinearLowerLimit();
    void setLinearUpperLimit(Vector3 limit);
    Vector3 getLinearUpperLimit();
    void setAngularLowerLimit(Vector3 limit);
    Vector3 getAngularLowerLimit();
    void setAngularUpperLimit(Vector3 limit);
    Vector3 getAngularUpperLimit();

    void setBreakingImpulse(float impulse);
    float getBreakingImpulse();

    void setHingeLimit(float low, float high, float softness = M3D_PHYSICS_CONSTRAINT_DEFAULT_SOFTNESS, float bias = M3D_PHYSICS_CONSTRAINT_DEFAULT_BIAS, float relaxation = M3D_PHYSICS_CONSTRAINT_DEFAULT_RELAXATION);
    void setConeLimit(float swingSpan1, float swingSpan2, float twistSpan, float softness = M3D_PHYSICS_CONSTRAINT_DEFAULT_SOFTNESS, float bias = M3D_PHYSICS_CONSTRAINT_DEFAULT_BIAS, float relaxation = M3D_PHYSICS_CONSTRAINT_DEFAULT_RELAXATION);
    float getLimitLow();
    float getLimitHigh();
    float getSwingSpan1();
    float getSwingSpan2();
    float getTwistSpan();
    float getSoftness();
    float getBias();
    float getRelaxation();

    void setDamping(float damping);
    float getDamping();
    void setThresh(float thresh);
    float getThresh();

    void setBreakable(bool breakable);
    bool isBreakable();
    void setAngularOnly(bool angular);
    bool isAngularOnly();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);

    virtual void removeReference(Xml* reference);
};

}

#endif // MAGIC3D_PHYSICS_CONSTRAINT_H
