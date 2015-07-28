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

#ifndef MAGIC3D_PHYSICS_H
#define MAGIC3D_PHYSICS_H

#include <magic3d/math.h>

namespace Magic3D
{
#define M3D_PHYSICS_WORLDS 2

class PhysicsRenderer2D;
class PhysicsObject;
class PhysicsConstraint;

enum PHYSICS_SHAPE
{
    ePHYSICS_SHAPE_BOX,
    ePHYSICS_SHAPE_SPHERE,
    ePHYSICS_SHAPE_CAPSULE,
    ePHYSICS_SHAPE_CYLINDER,
    ePHYSICS_SHAPE_CONE,
    ePHYSICS_SHAPE_CONVEXHULL,
    ePHYSICS_SHAPE_TRIANGLEMESH
};

enum PHYSICS_GROUP
{
    ePHYSICS_GROUP_0 = BIT(0),
    ePHYSICS_GROUP_1 = BIT(1),
    ePHYSICS_GROUP_2 = BIT(2),
    ePHYSICS_GROUP_3 = BIT(3),
    ePHYSICS_GROUP_4 = BIT(4),
    ePHYSICS_GROUP_5 = BIT(5),
    ePHYSICS_GROUP_6 = BIT(6),
    ePHYSICS_GROUP_7 = BIT(7),
    ePHYSICS_GROUP_8 = BIT(8),
    ePHYSICS_GROUP_9 = BIT(9)
};

class PhysicsCollision
{
public:
    virtual ~PhysicsCollision() = 0;
    virtual void collide(PhysicsObject* physicsObject, Vector3 selfPosition, Vector3 objectPosition, Vector3 objectNormal) = 0;
    virtual PhysicsCollision* spawn() const = 0;
};

struct RayCastReturn
{
    Vector3 point;
    Vector3 normal;
    PhysicsObject* physicsObject;
};

class Physics
{
private:
    static Physics* instance;

    PhysicsRenderer2D* renderer2D;
    btDefaultCollisionConfiguration*     collisionConfiguration[2];
    btCollisionDispatcher*               dispatcher[2];
    btBroadphaseInterface*               overlappingPairCache[2];
    btSequentialImpulseConstraintSolver* solver[2];
    btDiscreteDynamicsWorld*             dynamicsWorld[2];

    bool playing;

    int bodiesCount;
    int constraintsCount;

    Physics();
    virtual ~Physics();

    void createWorld();
    void destroyWorld();

public:
    static bool start();
    static bool finish();

    virtual bool update();

    static Physics* getInstance();

    void render();
    void play(bool create = true);
    void stop(bool destroy = true);
    bool isPlaying();

    int getBodiesCount();
    int getConstraintsCount();

    void add(PhysicsObject* physicsObject);
    void remove(PhysicsObject* physicsObject);
    void reset(PhysicsObject* physicsObject);

    void addConstraint(PhysicsConstraint* constraint);
    void removeConstraint(PhysicsConstraint* constraint);

    float* mapShape(PhysicsObject* physicsObject);
    void unmapShape(PhysicsObject* physicsObject);

    void setGravity(const Vector3& gravity);
    Vector3 getGravity();

    RayCastReturn rayCast(Vector3 start, Vector3 end, bool orthographic);
};

}
#endif // MAGIC3D_PHYSICS_H
