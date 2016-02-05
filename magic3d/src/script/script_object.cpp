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

#include <magic3d/script/script_object.h>
#include <magic3d/scene.h>
#include <magic3d/tween/tween.h>

const char Magic3D::ScriptObject::className[] = "Object";

Magic3D::ScriptClass<Magic3D::ScriptObject>::ScriptEnum Magic3D::ScriptObject::enums[] = {
    ScriptClassEnum("eOBJECT_CUSTOM",     Magic3D::eOBJECT_CUSTOM),
    ScriptClassEnum("eOBJECT_CAMERA",     Magic3D::eOBJECT_CAMERA),
    ScriptClassEnum("eOBJECT_SPRITE",     Magic3D::eOBJECT_SPRITE),
    ScriptClassEnum("eOBJECT_MODEL",      Magic3D::eOBJECT_MODEL),
    ScriptClassEnum("eOBJECT_LIGHT",      Magic3D::eOBJECT_LIGHT),
    ScriptClassEnum("eOBJECT_PARTICLES",  Magic3D::eOBJECT_PARTICLES),
    ScriptClassEnum("eOBJECT_TERRAIN",    Magic3D::eOBJECT_TERRAIN),
    ScriptClassEnum("eOBJECT_GUI_LABEL",  Magic3D::eOBJECT_GUI_LABEL),
    ScriptClassEnum("eOBJECT_GUI_WINDOW", Magic3D::eOBJECT_GUI_WINDOW),
    ScriptClassEnum("eOBJECT_GUI_BUTTON", Magic3D::eOBJECT_GUI_BUTTON),
    ScriptClassEnum("eOBJECT_GUI_SLIDER", Magic3D::eOBJECT_GUI_SLIDER),
    ScriptClassEnum("eOBJECT_SOUND",      Magic3D::eOBJECT_SOUND),
    ScriptClassEnum("eOBJECT_WATER",      Magic3D::eOBJECT_WATER),
    ScriptClassEnum("eOBJECT_VEGETATION", Magic3D::eOBJECT_VEGETATION),
    ScriptClassEnum("eOBJECT_CLOTH",      Magic3D::eOBJECT_CLOTH),
    ScriptClassEnum("eOBJECT_JOYSTICK",   Magic3D::eOBJECT_JOYSTICK),
    ScriptClassEnum("eOBJECT_BONE",       Magic3D::eOBJECT_BONE),

    ScriptClassEnum("eRENDER_2D", Magic3D::eRENDER_2D),
    ScriptClassEnum("eRENDER_3D", Magic3D::eRENDER_3D),
    {NULL, 0}
};

Magic3D::ScriptClass<Magic3D::ScriptObject>::ScriptFunction Magic3D::ScriptObject::functions[] = {
    ScriptClassFunction(Magic3D::ScriptObject, getScript, "void getScript()", "Script used by the object."),

    ScriptClassFunction(Magic3D::ScriptObject, setPosition, "void setPosition(Vector3 position)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getPosition, "Vector3 getPosition()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setRotation, "void setRotation(Vector3 rotation)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setQuaternion, "void setQuaternion(float x, float y, float z, float w)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getRotation, "Vector3 getRotation()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setScale,    "void setScale(Vector3 scale)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getScale,    "Vector3 getScale()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, lookAt,      "void lookAt(Vector3 position, Vector3 up, float factor)", "Rotate to look at a position."),

    ScriptClassFunction(Magic3D::ScriptObject, move,    "void move(Vector3 factor)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, rotate,  "void rotate(Vector3 rotation)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, size,    "void size(Vector3 scale)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setSize, "void setSize(float x, float y)", "Set the Sprite size."),
    ScriptClassFunction(Magic3D::ScriptObject, getSize, "Vector3 getSize()", "Get the Sprite size."),

    ScriptClassFunction(Magic3D::ScriptObject, getParent,     "Object* getParent()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setParent,     "void setParent(Object* parent)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getParentBone, "string getParentBone()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setParentBone, "void setParentBone(string bone)", ""),

    ScriptClassFunction(Magic3D::ScriptObject, getName,      "string getName()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getLayerName, "string getLayerName()", ""),

    /*getMatrix(lua_State *lua);
    setMatrix(lua_State *lua);*/

    //getBoundingBox(lua_State *lua);

    ScriptClassFunction(Magic3D::ScriptObject, getType, "int getType()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getRender, "int getRender()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setEnabled, "void setEnabled(bool enabled)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isEnabled, "bool isEnabled()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setVisible, "void setVisible(bool visible)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isVisible, "bool isVisible()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isInFrustum, "bool isInFrustum()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, getDirectionFront, "Vector3 getDirectionFront()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getDirectionBack, "Vector3 getDirectionBack()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getDirectionUp, "Vector3 getDirectionUp()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getDirectionDown, "Vector3 getDirectionDown()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getDirectionLeft, "Vector3 getDirectionLeft()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getDirectionRight, "Vector3 getDirectionRight()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setSelected, "void setSelected(bool selected)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isSelected, "bool isSelected()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setFlag, "void setFlag(int flag)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getFlag, "int getFlag()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, activatePhysics, "void activatePhysics(bool activate)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isActivatedPhysics, "bool isActivatedPhysics()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, applyCentralForce, "void applyCentralForce(Vector3 force)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyCentralImpulse, "void applyCentralImpulse(Vector3 impulse)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyDamping, "void applyDamping(float damping)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyForce, "void applyForce(Vector3 force, Vector3 position)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyGravity, "void applyGravity()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyImpulse, "void applyImpulse(Vector3 impulse, Vector3 position)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyTorque, "void applyTorque(Vector3 torque)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, applyTorqueImpulse, "void applyTorqueImpulse(Vector3 torque)", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setFriction, "void setFriction(float friction)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getFriction, "float getFriction()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setGravity, "void setGravity(Vector3 gravity)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getGravity, "Vector3 getGravity()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setDamping, "void setDamping(float linear, float angular)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getLinearDamping, "float getLinearDamping()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getAngularDamping, "float getAngularDamping()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setAngularFactor, "void setAngularFactor(Vector3 factor)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getAngularFactor, "Vector3 getAngularFactor()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setCentreOfMass, "void setCentreOfMass(Vector3 position)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getCentreOfMass, "Vector3 getCentreOfMass()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setAngularVelocity, "void setAngularVelocity(Vector3 velocity)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getAngularVelocity, "Vector3 getAngularVelocity()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setLinearVelocity, "void setLinearVelocity(Vector3 velocity)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getLinearVelocity, "Vector3 getLinearVelocity()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setLinearFactor, "void setLinearFactor(Vector3 factor)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getLinearFactor, "Vector3 getLinearFactor()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getTotalForce, "Vector3 getTotalForce()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getTotalTorque, "Vector3 getTotalTorque()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isPhysicsSleeping, "void isPhysicsSleeping()", ""),    
    ScriptClassFunction(Magic3D::ScriptObject, resetPhysics, "void resetPhysics()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setMeshTexture, "void setMeshTexture(string mesh, string texture)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getMeshTexture, "string getMeshTexture(string mesh)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setMeshColor, "void setMeshColor(string mesh, Color color)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getMeshColor, "Color getMeshColor(string mesh)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setMeshVisible, "void setMeshVisible(string mesh, bool visible)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isMeshVisible, "bool isMeshVisible(string mesh)", ""),

    ScriptClassFunction(Magic3D::ScriptObject, play, "void play(bool reverse)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, playAnimation, "void playAnimation(string animation, float interpolation, bool reverse)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, stop, "void stop()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, isPlaying, "bool isPlaying()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, playTween, "void playTween(int index)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, stopTween, "void stopTween(int index)", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setCurrentAnimation, "void setCurrentAnimation(string animation)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getCurrentAnimation, "string getCurrentAnimation()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setAnimationFrame, "void setAnimationFrame(int frame)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getAnimationFrame, "int getAnimationFrame()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getAnimationFrameCount, "int getAnimationFrameCount()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setText, "void setText(string text)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getText, "string getText()", ""),
    ScriptClassFunction(Magic3D::ScriptObject, setTextSize, "void setTextSize(float size)", ""),
    ScriptClassFunction(Magic3D::ScriptObject, getTextSize, "float getTextSize()", ""),

    ScriptClassFunction(Magic3D::ScriptObject, getObject,    "Object* getObject(string name)", "Get an object from scene by name."),
    ScriptClassFunction(Magic3D::ScriptObject, getChild,     "Object* getChild(string name)", "Get an object child by name."),
    ScriptClassFunction(Magic3D::ScriptObject, spawn,        "Object* spawn(string name, string layerName, bool keepChildrenLayer)", "Spawn object in layer with new name."),

    ScriptClassFunction(Magic3D::ScriptObject, getJoystickDirection, "Vector3 getJoystickDirection()", "Get the joystick direction."),
    ScriptClassFunction(Magic3D::ScriptObject, getJoystickForce, "float getJoystickForce()", "Get the joystick force."),

    ScriptClassFunction(Magic3D::ScriptObject, callFunction, "void callFunction(string function, int nargs)", ""),

    ScriptClassFunction(Magic3D::ScriptObject, setShaderVarValue, "void setShaderVarValue(int meshIndex, string variable, type x, type y, type z, type w)", ""),

    {NULL, NULL, NULL, NULL}
};

Magic3D::ScriptObject::ScriptObject(lua_State *lua) : ScriptBasic(className)
{
    if (lua)
    {

    }
}

Magic3D::ScriptObject::ScriptObject(Object *object) : ScriptBasic(className)
{
    this->object = object;
}

Magic3D::ScriptObject::~ScriptObject()
{
    object = NULL;
}

int Magic3D::ScriptObject::getScript(lua_State *lua)
{
    std::string name = M3D_SCRIPT_OBJECT + object->getName();
    lua_getglobal(lua, name.c_str());
    if (lua_isnil(lua, -1))
    {
        lua_remove(lua, -1);
        lua_pushnil(lua);
        luaL_error(lua, "%s missing table", name.c_str());
    }
    return 1;
}

int Magic3D::ScriptObject::setPosition(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPosition(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getPosition(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPosition());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setRotation(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setRotationEuler(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::setQuaternion(lua_State *lua)
{
    float x = luaL_checknumber(lua, 1);
    float y = luaL_checknumber(lua, 2);
    float z = luaL_checknumber(lua, 3);
    float w = luaL_checknumber(lua, 4);

    object->setRotation(Quaternion(x, y, z, w));
    return 0;
}

int Magic3D::ScriptObject::getRotation(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getRotationEuler());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setScale(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setRotationEuler(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getScale(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getScale());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::lookAt(lua_State *lua)
{
    ScriptVector3* position = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* up = ScriptClass<ScriptVector3>::check(lua, 2);

    object->lookAt(position->getValue(), up->getValue(), luaL_checknumber(lua, 3));
    return 0;
}

int Magic3D::ScriptObject::move(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->move(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::rotate(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->rotate(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::size(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->size(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::setSize(lua_State *lua)
{
    switch (object->getType())
    {
        case eOBJECT_GUI_BUTTON:
        case eOBJECT_GUI_LABEL:
        case eOBJECT_GUI_SLIDER:
        case eOBJECT_GUI_WINDOW:
        case eOBJECT_SPRITE:
        {
            Sprite* sprite = static_cast<Sprite*>(object);
            sprite->setSize(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
            break;
        }
        default: break;
    }
    return 0;
}

int Magic3D::ScriptObject::getSize(lua_State *lua)
{
    int result = 0;
    switch (object->getType())
    {
        case eOBJECT_GUI_BUTTON:
        case eOBJECT_GUI_LABEL:
        case eOBJECT_GUI_SLIDER:
        case eOBJECT_GUI_WINDOW:
        case eOBJECT_SPRITE:
        {
            Sprite* sprite = static_cast<Sprite*>(object);
            ScriptVector3* vec = new ScriptVector3(Vector3(sprite->getWidth(), sprite->getHeight(), 0.0f));

            ScriptClass<ScriptVector3>::push(lua, vec, true);
            result = 1;
            break;
        }
        default: break;
    }
    return result;
}

int Magic3D::ScriptObject::setParent(lua_State *lua)
{
    ScriptObject* object = ScriptClass<ScriptObject>::check(lua, 1);

    if (object)
    {
        this->object->setParent(object->object);
    }
    return 0;
}

int Magic3D::ScriptObject::getParent(lua_State *lua)
{
    ScriptObject* object = NULL;

    if (this->object->getParent())
    {
        object = new ScriptObject(this->object->getParent());
        ScriptClass<ScriptObject>::push(lua, object, true);
    }
    else
    {
        lua_pushnil(lua);
    }

    return 1;
}

int Magic3D::ScriptObject::setParentBone(lua_State *lua)
{
    if (object->getType() == eOBJECT_MODEL)
    {
        Model* model = static_cast<Model*>(object);
        model->setParentBone(model->getSkeleton()->getBoneByName(luaL_checkstring(lua, 1)));
    }
    return 0;
}

int Magic3D::ScriptObject::getParentBone(lua_State *lua)
{
    if (object->getParentBone())
    {
        lua_pushstring(lua, object->getParentBone()->getName().c_str());
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::getName(lua_State *lua)
{
    lua_pushstring(lua, this->object->getName().c_str());

    return 1;
}

int Magic3D::ScriptObject::getLayerName(lua_State *lua)
{
    if (this->object->getLayer())
    {
        lua_pushstring(lua, this->object->getLayer()->getName().c_str());
    }
    else
    {
        lua_pushnil(lua);
    }

    return 1;
}

/*getMatrix(lua_State *lua);
setMatrix(lua_State *lua);*/

//getBoundingBox(lua_State *lua);

int Magic3D::ScriptObject::getType(lua_State *lua)
{
    lua_pushinteger(lua, object->getType());
    return 1;
}

int Magic3D::ScriptObject::getRender(lua_State *lua)
{
    lua_pushinteger(lua, object->getRender());
    return 1;
}

int Magic3D::ScriptObject::setEnabled(lua_State *lua)
{
    object->setEnabled(lua_toboolean(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::isEnabled(lua_State *lua)
{
    lua_pushboolean(lua, object->isEnabled());
    return 1;
}

int Magic3D::ScriptObject::setVisible(lua_State *lua)
{
    object->setVisible(lua_toboolean(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::isVisible(lua_State *lua)
{
    lua_pushboolean(lua, object->isVisible());
    return 1;
}

int Magic3D::ScriptObject::isInFrustum(lua_State *lua)
{
    lua_pushboolean(lua, object->isInFrustum());
    return 1;
}

int Magic3D::ScriptObject::getDirectionFront(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionFront());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getDirectionBack(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionBack());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getDirectionUp(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionUp());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getDirectionDown(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionDown());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getDirectionLeft(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionLeft());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getDirectionRight(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getDirectionRight());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setSelected(lua_State *lua)
{
    object->setSelected(lua_toboolean(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::isSelected(lua_State *lua)
{
    lua_pushboolean(lua, object->isSelected());
    return 1;
}

int Magic3D::ScriptObject::setFlag(lua_State *lua)
{
    object->setFlag(luaL_checkinteger(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::getFlag(lua_State *lua)
{
    lua_pushinteger(lua, object->getFlag());
    return 1;
}

int Magic3D::ScriptObject::activatePhysics(lua_State *lua)
{
    object->activatePhysics(lua_toboolean(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::isActivatedPhysics(lua_State *lua)
{
    lua_pushboolean(lua, object->isActivatedPhysics());
    return 1;
}

int Magic3D::ScriptObject::applyCentralForce(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->applyPhysicsCentralForce(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::applyCentralImpulse(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->applyPhysicsCentralImpulse(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::applyDamping(lua_State *lua)
{
    object->applyPhysicsDamping(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::applyForce(lua_State *lua)
{
    ScriptVector3* force = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* position = ScriptClass<ScriptVector3>::check(lua, 2);

    object->applyPhysicsForce(force->getValue(), position->getValue());
    return 0;
}

int Magic3D::ScriptObject::applyGravity(lua_State *lua)
{
    object->applyPhysicsGravity();
    return 0;
}

int Magic3D::ScriptObject::applyImpulse(lua_State *lua)
{
    ScriptVector3* impulse = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* position = ScriptClass<ScriptVector3>::check(lua, 2);

    object->applyPhysicsImpulse(impulse->getValue(), position->getValue());
    return 0;
}

int Magic3D::ScriptObject::applyTorque(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->applyPhysicsTorque(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::applyTorqueImpulse(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->applyPhysicsTorqueImpulse(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::setFriction(lua_State *lua)
{
    object->setPhysicsFriction(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::getFriction(lua_State *lua)
{
    lua_pushnumber(lua, object->getPhysicsFriction());
    return 1;
}

int Magic3D::ScriptObject::setGravity(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsGravity(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getGravity(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsGravity());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setDamping(lua_State *lua)
{
    object->setPhysicsDamping(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
    return 0;
}

int Magic3D::ScriptObject::getLinearDamping(lua_State *lua)
{
    lua_pushnumber(lua, object->getPhysicsLinearDamping());
    return 1;
}

int Magic3D::ScriptObject::getAngularDamping(lua_State *lua)
{
    lua_pushnumber(lua, object->getPhysicsAngularDamping());
    return 1;
}

int Magic3D::ScriptObject::setAngularFactor(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsAngularFactor(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getAngularFactor(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsAngularFactor());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setCentreOfMass(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsCentreOfMass(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getCentreOfMass(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsCentreOfMass());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setAngularVelocity(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsAngularVelocity(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getAngularVelocity(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsAngularVelocity());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setLinearVelocity(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsLinearVelocity(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getLinearVelocity(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsLinearVelocity());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::setLinearFactor(lua_State *lua)
{
    ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);

    object->setPhysicsLinearFactor(vector->getValue());
    return 0;
}

int Magic3D::ScriptObject::getLinearFactor(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsLinearFactor());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getTotalForce(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsTotalForce());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::getTotalTorque(lua_State *lua)
{
    ScriptVector3* vec = new ScriptVector3(object->getPhysicsTotalTorque());

    ScriptClass<ScriptVector3>::push(lua, vec, true);
    return 1;
}

int Magic3D::ScriptObject::isPhysicsSleeping(lua_State *lua)
{
    lua_pushboolean(lua, object->isPhysicsSleeping());
    return 1;
}

int Magic3D::ScriptObject::resetPhysics(lua_State *lua)
{
    if (lua)
    {

    }
    object->resetPhysics();
    return 0;
}

int Magic3D::ScriptObject::setMeshTexture(lua_State *lua)
{
    object->setMeshTexture(luaL_checkstring(lua, 1), luaL_checkstring(lua, 2));
    return 0;
}

int Magic3D::ScriptObject::getMeshTexture(lua_State *lua)
{
    lua_pushstring(lua, object->getMeshTexture(luaL_checkstring(lua, 1)).c_str());
    return 1;
}

int Magic3D::ScriptObject::setMeshColor(lua_State *lua)
{
    ScriptColor* color = ScriptClass<ScriptColor>::check(lua, 2);

    object->setMeshColor(luaL_checkstring(lua, 1), color->getValue());
    return 0;
}

int Magic3D::ScriptObject::getMeshColor(lua_State *lua)
{
    ScriptColor* color = new ScriptColor(object->getMeshColor(luaL_checkstring(lua, 1)));

    ScriptClass<ScriptColor>::push(lua, color, true);
    return 1;
}

int Magic3D::ScriptObject::setMeshVisible(lua_State *lua)
{
    object->setMeshVisible(luaL_checkstring(lua, 1), lua_toboolean(lua, 2));
    return 0;
}

int Magic3D::ScriptObject::isMeshVisible(lua_State *lua)
{
    lua_pushboolean(lua, object->isMeshVisible(luaL_checkstring(lua, 1)));
    return 1;
}

int Magic3D::ScriptObject::play(lua_State *lua)
{
    object->play(lua_toboolean(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::playAnimation(lua_State *lua)
{
    object->playAnimation(luaL_checkstring(lua, 1), luaL_checknumber(lua, 2), lua_toboolean(lua, 3));
    return 0;
}

int Magic3D::ScriptObject::stop(lua_State *lua)
{
    if (lua)
    {

    }
    object->stop();
    return 0;
}

int Magic3D::ScriptObject::isPlaying(lua_State *lua)
{
    lua_pushboolean(lua, object->isPlaying());
    return 1;
}

int Magic3D::ScriptObject::playTween(lua_State *lua)
{
    Tween* tween = object->getTween(luaL_checkinteger(lua, 1));
    if (tween)
    {
        tween->play();
    }
}

int Magic3D::ScriptObject::stopTween(lua_State *lua)
{
    Tween* tween = object->getTween(luaL_checkinteger(lua, 1));
    if (tween)
    {
        tween->stop();
    }
}

int Magic3D::ScriptObject::setCurrentAnimation(lua_State *lua)
{
    object->setCurrentAnimation(luaL_checkstring(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::getCurrentAnimation(lua_State *lua)
{
    lua_pushstring(lua, object->getCurrentAnimation().c_str());
    return 1;
}

int Magic3D::ScriptObject::setAnimationFrame(lua_State *lua)
{
    object->setAnimationFrame(luaL_checkinteger(lua, 1));
    return 0;
}

int Magic3D::ScriptObject::getAnimationFrame(lua_State *lua)
{
    lua_pushinteger(lua, object->getAnimationFrame());
    return 1;
}

int Magic3D::ScriptObject::getAnimationFrameCount(lua_State *lua)
{
    lua_pushinteger(lua, object->getAnimationFrameCount());
    return 1;
}

int Magic3D::ScriptObject::setText(lua_State *lua)
{
    if (object->getType() == eOBJECT_GUI_LABEL)
    {
        GUILabel* label = static_cast<GUILabel*>(object);
        label->setText(luaL_checkstring(lua, 1));
    }
    return 0;
}

int Magic3D::ScriptObject::getText(lua_State *lua)
{
    if (object->getType() == eOBJECT_GUI_LABEL)
    {
        GUILabel* label = static_cast<GUILabel*>(object);
        lua_pushstring(lua, label->getText().c_str());
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::setTextSize(lua_State *lua)
{
    if (object->getType() == eOBJECT_GUI_LABEL)
    {
        GUILabel* label = static_cast<GUILabel*>(object);
        label->setTextSize(luaL_checknumber(lua, 1));
    }
    return 0;
}

int Magic3D::ScriptObject::getTextSize(lua_State *lua)
{
    if (object->getType() == eOBJECT_GUI_LABEL)
    {
        GUILabel* label = static_cast<GUILabel*>(object);
        lua_pushnumber(lua, label->getTextSize());
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::getObject(lua_State *lua)
{
    Object* findObj = object->getObject(luaL_checkstring(lua, 1));

    if (findObj)
    {
        ScriptObject* obj = new ScriptObject(findObj);

        ScriptClass<ScriptObject>::push(lua, obj, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::getChild(lua_State *lua)
{
    Object* findObj = object->getChild(luaL_checkstring(lua, 1));

    if (findObj)
    {
        ScriptObject* obj = new ScriptObject(findObj);

        ScriptClass<ScriptObject>::push(lua, obj, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::spawn(lua_State *lua)
{
    Object* findObj = object->spawn(luaL_checkstring(lua, 1), luaL_checkstring(lua, 2), lua_toboolean(lua, 3));

    if (findObj)
    {
        Script::getInstance()->addToTemp(findObj);
        ScriptObject* obj = new ScriptObject(findObj);

        ScriptClass<ScriptObject>::push(lua, obj, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptObject::getJoystickDirection(lua_State *lua)
{
    if (object->getType() == eOBJECT_JOYSTICK)
    {
        Joystick* joystick = static_cast<Joystick*>(object);
        ScriptVector3* vec = new ScriptVector3(joystick->getDirection());

        ScriptClass<ScriptVector3>::push(lua, vec, true);
    }
    else
    {
        ScriptVector3* vec = new ScriptVector3(Vector3(0.0f, 0.0f, 0.0f));

        ScriptClass<ScriptVector3>::push(lua, vec, true);
    }
    return 1;
}

int Magic3D::ScriptObject::getJoystickForce(lua_State *lua)
{
    if (object->getType() == eOBJECT_JOYSTICK)
    {
        Joystick* joystick = static_cast<Joystick*>(object);
        lua_pushnumber(lua, joystick->getForce());
    }
    else
    {
        lua_pushnumber(lua, 0.0f);
    }
    return 1;
}

int Magic3D::ScriptObject::callFunction(lua_State *lua)
{
    std::string function = luaL_checkstring(lua, 1);
    Script::getInstance()->execute(function, luaL_checkinteger(lua, 2), object->getName());
    return 0;
}

int Magic3D::ScriptObject::setShaderVarValue(lua_State *lua)
{
    int index = luaL_checkinteger(lua, 1);
    std::string variable = luaL_checkstring(lua, 2);

    if (index >= 0 && index < (int)object->getMeshes()->size())
    {
        MaterialVar* v = object->getMeshes()->at(index)->getMaterials()->at(0)->getVar(variable);
        if (v)
        {
            switch (v->getType())
            {
                case eSHADER_VAR_BOOL:      static_cast<MaterialVar_Boolean*>(v)->setValue(0, lua_toboolean(lua, 3)); break;
                case eSHADER_VAR_BOOL_VEC2:
                {
                    MaterialVar_Boolean* mv = static_cast<MaterialVar_Boolean*>(v);
                    mv->setValue(0, lua_toboolean(lua, 3));
                    mv->setValue(1, lua_toboolean(lua, 4));
                    break;
                }
                case eSHADER_VAR_BOOL_VEC3:
                {
                    MaterialVar_Boolean* mv = static_cast<MaterialVar_Boolean*>(v);
                    mv->setValue(0, lua_toboolean(lua, 3));
                    mv->setValue(1, lua_toboolean(lua, 4));
                    mv->setValue(2, lua_toboolean(lua, 5));
                    break;
                }
                case eSHADER_VAR_BOOL_VEC4:
                {
                    MaterialVar_Boolean* mv = static_cast<MaterialVar_Boolean*>(v);
                    mv->setValue(0, lua_toboolean(lua, 3));
                    mv->setValue(1, lua_toboolean(lua, 4));
                    mv->setValue(2, lua_toboolean(lua, 5));
                    mv->setValue(3, lua_toboolean(lua, 6));
                    break;
                }

                case eSHADER_VAR_INT:      static_cast<MaterialVar_Integer*>(v)->setValue(0, luaL_checkinteger(lua, 3)); break;
                case eSHADER_VAR_INT_VEC2:
                {
                    MaterialVar_Integer* mv = static_cast<MaterialVar_Integer*>(v);
                    mv->setValue(0, luaL_checkinteger(lua, 3));
                    mv->setValue(1, luaL_checkinteger(lua, 4));
                    break;
                }
                case eSHADER_VAR_INT_VEC3:
                {
                    MaterialVar_Integer* mv = static_cast<MaterialVar_Integer*>(v);
                    mv->setValue(0, luaL_checkinteger(lua, 3));
                    mv->setValue(1, luaL_checkinteger(lua, 4));
                    mv->setValue(2, luaL_checkinteger(lua, 5));
                    break;
                }
                case eSHADER_VAR_INT_VEC4:
                {
                    MaterialVar_Integer* mv = static_cast<MaterialVar_Integer*>(v);
                    mv->setValue(0, luaL_checkinteger(lua, 3));
                    mv->setValue(1, luaL_checkinteger(lua, 4));
                    mv->setValue(2, luaL_checkinteger(lua, 5));
                    mv->setValue(3, luaL_checkinteger(lua, 6));
                    break;
                }

                case eSHADER_VAR_FLOAT:      static_cast<MaterialVar_Float*>(v)->setValue(0, luaL_checknumber(lua, 3)); break;
                case eSHADER_VAR_FLOAT_VEC2:
                {
                    MaterialVar_Float* mv = static_cast<MaterialVar_Float*>(v);
                    mv->setValue(0, luaL_checknumber(lua, 3));
                    mv->setValue(1, luaL_checknumber(lua, 4));
                    break;
                }
                case eSHADER_VAR_FLOAT_VEC3:
                {
                    MaterialVar_Float* mv = static_cast<MaterialVar_Float*>(v);
                    mv->setValue(0, luaL_checknumber(lua, 3));
                    mv->setValue(1, luaL_checknumber(lua, 4));
                    mv->setValue(2, luaL_checknumber(lua, 5));
                    break;
                }
                case eSHADER_VAR_FLOAT_VEC4:
                {
                    MaterialVar_Float* mv = static_cast<MaterialVar_Float*>(v);
                    mv->setValue(0, luaL_checknumber(lua, 3));
                    mv->setValue(1, luaL_checknumber(lua, 4));
                    mv->setValue(2, luaL_checknumber(lua, 5));
                    mv->setValue(3, luaL_checknumber(lua, 6));
                    break;
                }

                default: break;
            }
        }
    }

    return 0;
}
