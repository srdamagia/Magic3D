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

#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

#include <magic3d/object.h>
#include <magic3d/script/script_vector3.h>
#include <magic3d/script/script_color.h>

namespace Magic3D
{
class ScriptObject : public ScriptBasic
{
private:
    Object* object;

public:
    static const char className[];
    static Magic3D::ScriptClass<ScriptObject>::ScriptEnum enums[];
    static Magic3D::ScriptClass<ScriptObject>::ScriptFunction functions[];

    ScriptObject(lua_State *lua);
    ScriptObject(Object* object);
    virtual ~ScriptObject();

    int getScript(lua_State *lua);

    int setPosition(lua_State *lua);
    int getPosition(lua_State *lua);
    int getPositionFromParent(lua_State *lua);
    int setRotation(lua_State *lua);
    int setQuaternion(lua_State *lua);
    int getRotation(lua_State *lua);
    int getRotationFromParent(lua_State *lua);
    int setScale(lua_State *lua);
    int getScale(lua_State *lua);
    int lookAt(lua_State *lua);

    int move(lua_State *lua);
    int rotate(lua_State *lua);
    int size(lua_State *lua);
    int setSize(lua_State *lua);
    int getSize(lua_State *lua);

    int setParent(lua_State *lua);
    int getParent(lua_State *lua);
    int setParentBone(lua_State *lua);
    int getParentBone(lua_State *lua);

    int getName(lua_State *lua);
    int getLayerName(lua_State *lua);

    /*getMatrix(lua_State *lua);
    setMatrix(lua_State *lua);*/

    //getBoundingBox(lua_State *lua);
    int getType(lua_State *lua);
    int getRender(lua_State *lua);

    int setEnabled(lua_State *lua);
    int isEnabled(lua_State *lua);
    int setVisible(lua_State *lua);
    int isVisible(lua_State *lua);
    int isInFrustum(lua_State *lua);

    int getDirectionFront(lua_State *lua);
    int getDirectionBack(lua_State *lua);
    int getDirectionUp(lua_State *lua);
    int getDirectionDown(lua_State *lua);
    int getDirectionLeft(lua_State *lua);
    int getDirectionRight(lua_State *lua);

    int setSelected(lua_State *lua);
    int isSelected(lua_State *lua);

    int setFlag(lua_State *lua);
    int getFlag(lua_State *lua);

    int activatePhysics(lua_State *lua);
    int isActivatedPhysics(lua_State *lua);

    int applyCentralForce(lua_State *lua);
    int applyCentralImpulse(lua_State *lua);
    int applyDamping(lua_State *lua);
    int applyForce(lua_State *lua);
    int applyGravity(lua_State *lua);
    int applyImpulse(lua_State *lua);
    int applyTorque(lua_State *lua);
    int applyTorqueImpulse(lua_State *lua);

    int setFriction(lua_State *lua);
    int getFriction(lua_State *lua);
    int setGravity(lua_State *lua);
    int getGravity(lua_State *lua);
    int setDamping(lua_State *lua);
    int getLinearDamping(lua_State *lua);
    int getAngularDamping(lua_State *lua);
    int setAngularFactor(lua_State *lua);
    int getAngularFactor(lua_State *lua);
    int setCentreOfMass(lua_State *lua);
    int getCentreOfMass(lua_State *lua);
    int setAngularVelocity(lua_State *lua);
    int getAngularVelocity(lua_State *lua);
    int setLinearVelocity(lua_State *lua);
    int getLinearVelocity(lua_State *lua);
    int setLinearFactor(lua_State *lua);
    int getLinearFactor(lua_State *lua);
    int getTotalForce(lua_State *lua);
    int getTotalTorque(lua_State *lua);
    int isPhysicsSleeping(lua_State *lua);

    int resetPhysics(lua_State *lua);

    int setMeshTexture(lua_State *lua);
    int getMeshTexture(lua_State *lua);
    int setMeshColor(lua_State *lua);
    int getMeshColor(lua_State *lua);
    int setMeshVisible(lua_State *lua);
    int isMeshVisible(lua_State *lua);

    int play(lua_State *lua);
    int playAnimation(lua_State *lua);
    int stop(lua_State *lua);
    int isPlaying(lua_State *lua);

    int playTween(lua_State *lua);
    int stopTween(lua_State *lua);

    int setCurrentAnimation(lua_State *lua);
    int getCurrentAnimation(lua_State *lua);

    int setAnimationFrame(lua_State *lua);
    int getAnimationFrame(lua_State *lua);
    int getAnimationFrameCount(lua_State *lua);

    int setText(lua_State *lua);
    int getText(lua_State *lua);
    int setTextSize(lua_State *lua);
    int getTextSize(lua_State *lua);

    int getObject(lua_State *lua);
    int getChild(lua_State *lua);
    int spawn(lua_State *lua);

    int getJoystickDirection(lua_State *lua);
    int getJoystickForce(lua_State *lua);

    int callFunction(lua_State *lua);

    int setShaderVarValue(lua_State *lua);
};
}
#endif /* SCRIPT_OBJECT_H */
