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

#include <magic3d/script/script_magic3d.h>
#include <magic3d/magic3d.h>

const char Magic3D::ScriptMagic3D::className[] = "Magic3D";

Magic3D::ScriptClass<Magic3D::ScriptMagic3D>::ScriptEnum Magic3D::ScriptMagic3D::enums[] = {{NULL, 0}};
Magic3D::ScriptClass<Magic3D::ScriptMagic3D>::ScriptFunction Magic3D::ScriptMagic3D::functions[] = {
    ScriptClassFunction(ScriptMagic3D, getObject, "Object* getObject(string name)", "Get an object from scene by name."),
    ScriptClassFunction(ScriptMagic3D, setCamera3D, "void setCamera3D(string name)", "Set current perspective camera."),
    ScriptClassFunction(ScriptMagic3D, setCamera2D, "void setCamera2D(string name)", "Set current orthographic camera."),

    ScriptClassFunction(ScriptMagic3D, setLayerVisible, "void setLayerVisible(string name, bool visible)", "Set the visibility of a layer."),
    ScriptClassFunction(ScriptMagic3D, isLayerVisible, "bool isLayerVisible()", "Get the visibility of a layer."),

    ScriptClassFunction(ScriptMagic3D, setCursorPosition, "void setCursorPosition(int x, int y)", "Set the cursor position."),
    ScriptClassFunction(ScriptMagic3D, getCursorX, "int getCursorX()", "Get the cursor X position."),
    ScriptClassFunction(ScriptMagic3D, getCursorY, "int getCursorY()", "Get the cursor Y position."),

    ScriptClassFunction(ScriptMagic3D, setWindowWidth, "void setWindowWidth(int width)", "Set the window width."),
    ScriptClassFunction(ScriptMagic3D, getWindowWidth, "int getWindowWidth()", "Get the window width."),

    ScriptClassFunction(ScriptMagic3D, setWindowHeight, "void setWindowHeight(int height)", "Set the window height."),
    ScriptClassFunction(ScriptMagic3D, getWindowHeight, "int getWindowHeight()", "Get the window height."),

    ScriptClassFunction(ScriptMagic3D, getWindowAspectX, "void getWindowAspectX()", "Get the window horizontal aspect."),
    ScriptClassFunction(ScriptMagic3D, getWindowAspectY, "void getWindowAspectY()", "Get the window vertical aspect."),

    ScriptClassFunction(ScriptMagic3D, getTicks, "float getTicks()", "Get the current time in miliseconds."),
    ScriptClassFunction(ScriptMagic3D, getElapsedTime, "float getElapsedTime()", "Get the elapsed time since last update."),
    ScriptClassFunction(ScriptMagic3D, getElapsedTimeReal, "float getElapsedTimeReal()", "Get the elapsed time since last update without scale."),
    ScriptClassFunction(ScriptMagic3D, getTimeSinceStart, "float getTimeSinceStart()", "Get the elapsed time since the start."),
    ScriptClassFunction(ScriptMagic3D, getTimeScale, "float getTimeScale()", "Get the time scale."),
    ScriptClassFunction(ScriptMagic3D, setTimeScale, "float setTimeScale(float scale)", "Set the time scale."),

    ScriptClassFunction(ScriptMagic3D, pick, "Object* pick(string camera, float x, float y, int viewport, bool all)", "Pick an object."),
    ScriptClassFunction(ScriptMagic3D, getPosition2D, "Vector3 getPosition2D(Vector3 position, int viewport)", "Get 2D position."),
    ScriptClassFunction(ScriptMagic3D, getPosition3D, "Vector3 getPosition3D(float x, float y, float depth, int viewport)", "Get 3D position."),
    ScriptClassFunction(ScriptMagic3D, getPosition3DOnPlane, "Vector3 getPosition3DOnPlane(float x, float y, Vector3 planeNormal, Vector3 planePosition, int viewport)", "Get 3D position on a plane."),

    ScriptClassFunction(ScriptMagic3D, log, "void log(const char* text)", "Write a message in the log file."),

    ScriptClassFunction(ScriptMagic3D, loadScene, "void loadScene(string mge)", "Load scene closing current."),
    ScriptClassFunction(ScriptMagic3D, loadSceneAdditive, "loadSceneAdditive(string mge)", "Load scene without clear current."),

    ScriptClassFunction(ScriptMagic3D, loadConfig, "void loadConfig()", "Load configuration file."),
    ScriptClassFunction(ScriptMagic3D, saveConfig, "void saveConfig()", "Save configuration file."),

    ScriptClassFunction(ScriptMagic3D, isConfigEmpty, "bool isConfigEmpty()", "Return true if configuration is empty."),
    ScriptClassFunction(ScriptMagic3D, clearConfig,   "void clearConfig()", "Clear all configurations."),

    ScriptClassFunction(ScriptMagic3D, setConfigInteger, "void setConfigInteger(string key, int value)", "Set a configuration value."),
    ScriptClassFunction(ScriptMagic3D, setConfigFloat, "void setConfigFloat(string key, float value)", "Set a configuration value."),
    ScriptClassFunction(ScriptMagic3D, setConfigBoolean, "void setConfigBoolean(string key, bool value)", "Set a configuration value."),
    ScriptClassFunction(ScriptMagic3D, setConfigString, "void setConfigString(string key, string value)", "Set a configuration value."),

    ScriptClassFunction(ScriptMagic3D, getConfigInteger, "int getConfigInteger(string key)", "Get a configuration value."),
    ScriptClassFunction(ScriptMagic3D, getConfigFloat, "float getConfigFloat(string key)", "Get a configuration value."),
    ScriptClassFunction(ScriptMagic3D, getConfigBoolean, "bool getConfigBoolean(string key)", "Get a configuration value."),
    ScriptClassFunction(ScriptMagic3D, getConfigString, "string getConfigString(string key)", "Get a configuration value."),
    ScriptClassFunction(ScriptMagic3D, isConfigured, "bool isConfigured(string key)", "Return true if configuration key is found."),

    ScriptClassFunction(ScriptMagic3D, debugLine, "void debugLine(Vector3 pos1, Vector3 pos2, bool orthographic, Color color)", "Render a debug line."),

    ScriptClassFunction(ScriptMagic3D, rayCast, "Object* rayCast(Vector3 start, Vector3 end, bool orthographic)", ""),

    ScriptClassFunction(ScriptMagic3D, rotateCamera, "void rotateCamera(float x, float y, float z, bool rotate)", ""),

    ScriptClassFunction(ScriptMagic3D, setStereoscopy, "void setStereoscopy(bool stereoscopy, bool screenEffects)", ""),

    ScriptClassFunction(ScriptMagic3D, sendText, "void sendText(string text)", ""),
    ScriptClassFunction(ScriptMagic3D, sendObject, "void setStereoscopy(string objectName)", ""),
    ScriptClassFunction(ScriptMagic3D, sendInput, "void setStereoscopy(not working yet)", ""),

    {NULL, NULL, NULL, NULL}
};

Magic3D::ScriptMagic3D::ScriptMagic3D(lua_State *lua) : ScriptBasic(className)
{
    if (lua)
    {

    }
}

Magic3D::ScriptMagic3D::~ScriptMagic3D()
{

}

int Magic3D::ScriptMagic3D::getObject(lua_State *lua)
{
    Object* object = ResourceManager::getObjects()->get(luaL_checkstring(lua, 1));

    if (object)
    {
        ScriptObject* obj = new ScriptObject(object);

        ScriptClass<ScriptObject>::push(lua, obj, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptMagic3D::setCamera3D(lua_State *lua)
{
    Object* object = ResourceManager::getObjects()->get(luaL_checkstring(lua, 1));

    if (object && object->getType() == eOBJECT_CAMERA)
    {
        Renderer::getInstance()->getCurrentViewPort()->setPerspective(static_cast<Camera*>(object));
    }
    else
    {
        Log::log(eLOG_FAILURE, "Invalid Camera!");
    }
    return 0;
}

int Magic3D::ScriptMagic3D::setCamera2D(lua_State *lua)
{
    Object* object = ResourceManager::getObjects()->get(luaL_checkstring(lua, 1));

    if (object && object->getType() == eOBJECT_CAMERA)
    {
        Renderer::getInstance()->getCurrentViewPort()->setOrthographic(static_cast<Camera*>(object));
    }
    else
    {
        Log::log(eLOG_FAILURE, "Invalid Camera!");
    }
    return 0;
}

int Magic3D::ScriptMagic3D::setLayerVisible(lua_State *lua)
{
    std::string layerName = luaL_checkstring(lua, 1);
    bool visible = lua_toboolean(lua, 2);

    Layer* layer = Scene::getInstance()->getLayer(layerName);
    if (layer)
    {
        layer->setVisible(visible);
    }
    return 0;
}

int Magic3D::ScriptMagic3D::isLayerVisible(lua_State *lua)
{
    std::string layerName = luaL_checkstring(lua, 1);

    Layer* layer = Scene::getInstance()->getLayer(layerName);
    if (layer)
    {
        lua_pushboolean(lua, layer->isVisible());
    }
    return 1;
}

int Magic3D::ScriptMagic3D::setCursorPosition(lua_State *lua)
{
    Renderer::getInstance()->getWindow()->setCursorPosition(luaL_checkinteger(lua, 1), luaL_checkinteger(lua, 2));
    return 0;
}

int Magic3D::ScriptMagic3D::getCursorX(lua_State *lua)
{
    lua_pushinteger(lua, Renderer::getInstance()->getWindow()->getCursorX());
    return 1;
}

int Magic3D::ScriptMagic3D::getCursorY(lua_State *lua)
{
    lua_pushinteger(lua, Renderer::getInstance()->getWindow()->getCursorY());
    return 1;
}

int Magic3D::ScriptMagic3D::setWindowWidth(lua_State *lua)
{
    Renderer::getInstance()->getWindow()->resize(luaL_checkinteger(lua, 1), Renderer::getInstance()->getWindow()->getHeight());
    return 0;
}

int Magic3D::ScriptMagic3D::getWindowWidth(lua_State *lua)
{
    lua_pushinteger(lua, Renderer::getInstance()->getWindow()->getWidth());
    return 1;
}

int Magic3D::ScriptMagic3D::setWindowHeight(lua_State *lua)
{
    Renderer::getInstance()->getWindow()->resize(Renderer::getInstance()->getWindow()->getWidth(), luaL_checkinteger(lua, 1));
    return 0;
}

int Magic3D::ScriptMagic3D::getWindowHeight(lua_State *lua)
{
    lua_pushinteger(lua, Renderer::getInstance()->getWindow()->getHeight());
    return 1;
}

int Magic3D::ScriptMagic3D::getWindowAspectX(lua_State *lua)
{
    lua_pushnumber(lua, Renderer::getInstance()->getWindow()->getWindowScreenAspect().getX());
    return 1;
}

int Magic3D::ScriptMagic3D::getWindowAspectY(lua_State *lua)
{
    lua_pushnumber(lua, Renderer::getInstance()->getWindow()->getWindowScreenAspect().getY());
    return 1;
}

int Magic3D::ScriptMagic3D::getTicks(lua_State *lua)
{
    lua_pushnumber(lua, Magic3D::getInstance()->getTicks());
    return 1;
}

int Magic3D::ScriptMagic3D::getElapsedTime(lua_State *lua)
{
    lua_pushnumber(lua, Magic3D::getInstance()->getElapsedTime());
    return 1;
}

int Magic3D::ScriptMagic3D::getElapsedTimeReal(lua_State *lua)
{
    lua_pushnumber(lua, Magic3D::getInstance()->getElapsedTimeReal());
    return 1;
}

int Magic3D::ScriptMagic3D::getTimeSinceStart(lua_State *lua)
{
    lua_pushnumber(lua, Magic3D::getInstance()->getTimeSinceStart());
    return 1;
}

int Magic3D::ScriptMagic3D::getTimeScale(lua_State *lua)
{
    lua_pushnumber(lua, Magic3D::getInstance()->getTimeScale());
    return 1;
}

int Magic3D::ScriptMagic3D::setTimeScale(lua_State *lua)
{
    Magic3D::getInstance()->setTimeScale(luaL_checknumber(lua, 1));
    return 0;
}

int Magic3D::ScriptMagic3D::pick(lua_State *lua)
{
    Object* object = ResourceManager::getObjects()->get(luaL_checkstring(lua, 1));

    if (object && object->getType() == eOBJECT_CAMERA)
    {
        Camera* camera = static_cast<Camera*>(object);

        Object* picked = camera->pick(luaL_checknumber(lua, 2), luaL_checknumber(lua, 3), luaL_checkinteger(lua, 4), lua_toboolean(lua, 5));

        if (picked)
        {
            ScriptObject* obj = new ScriptObject(picked);
            ScriptClass<ScriptObject>::push(lua, obj, true);
        }
        else
        {
            lua_pushnil(lua);
        }
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptMagic3D::getPosition2D(lua_State *lua)
{
    ViewPort* view = Renderer::getInstance()->getViewPort(luaL_checkinteger(lua, 2));
    if (view && view->getPerspective())
    {
        Camera* camera = view->getPerspective();
        ScriptVector3* vector = ScriptClass<ScriptVector3>::check(lua, 1);
        ScriptVector3* vec = new ScriptVector3(camera->getPosition2D(vector->getValue(), view));

        ScriptClass<ScriptVector3>::push(lua, vec, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptMagic3D::getPosition3D(lua_State *lua)
{
    ViewPort* view = Renderer::getInstance()->getViewPort(luaL_checkinteger(lua, 4));
    if (view && view->getPerspective())
    {
        Camera* camera = view->getPerspective();
        ScriptVector3* vec = new ScriptVector3(camera->getPosition3D(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3), view));

        ScriptClass<ScriptVector3>::push(lua, vec, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptMagic3D::getPosition3DOnPlane(lua_State *lua)
{
    ViewPort* view = Renderer::getInstance()->getViewPort(luaL_checkinteger(lua, 5));
    if (view && view->getPerspective())
    {
        Camera* camera = view->getPerspective();

        ScriptVector3* pN = ScriptClass<ScriptVector3>::check(lua, 3);
        ScriptVector3* pP = ScriptClass<ScriptVector3>::check(lua, 4);
        ScriptVector3* vec = new ScriptVector3(camera->getPosition3DOnPlane(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), pN->getValue(), pP->getValue(), view));

        ScriptClass<ScriptVector3>::push(lua, vec, true);
    }
    else
    {
        lua_pushnil(lua);
    }
    return 1;
}

int Magic3D::ScriptMagic3D::log(lua_State *lua)
{
    Log::log(eLOG_FAILURE, luaL_checkstring(lua, 1));
    return 0;
}

int Magic3D::ScriptMagic3D::loadScene(lua_State *lua)
{
    std::string path(luaL_checkstring(lua, 1));
    Magic3D::getInstance()->loadScene(path);
    return 0;
}

int Magic3D::ScriptMagic3D::loadSceneAdditive(lua_State *lua)
{
    std::string path(luaL_checkstring(lua, 1));
    Magic3D::getInstance()->loadSceneAdditive(path);
    return 0;
}

int Magic3D::ScriptMagic3D::isConfigEmpty(lua_State *lua)
{
    lua_pushboolean(lua, Config::getInstance()->isEmpty());
    return 1;
}

int Magic3D::ScriptMagic3D::clearConfig(lua_State *lua)
{
    if (lua)
    {

    }
    Config::getInstance()->clear();
    return 0;
}

int Magic3D::ScriptMagic3D::loadConfig(lua_State *lua)
{
    if (lua)
    {

    }
    Config::getInstance()->load();
    return 0;
}

int Magic3D::ScriptMagic3D::saveConfig(lua_State *lua)
{
    if (lua)
    {

    }
    Config::getInstance()->save();
    return 0;
}

int Magic3D::ScriptMagic3D::setConfigInteger(lua_State *lua)
{
    Config::getInstance()->setInteger(luaL_checkstring(lua, 1), luaL_checkinteger(lua, 2));
    return 0;
}

int Magic3D::ScriptMagic3D::setConfigFloat(lua_State *lua)
{
    Config::getInstance()->setFloat(luaL_checkstring(lua, 1), luaL_checknumber(lua, 2));
    return 0;
}

int Magic3D::ScriptMagic3D::setConfigBoolean(lua_State *lua)
{
    Config::getInstance()->setBoolean(luaL_checkstring(lua, 1), lua_toboolean(lua, 2));
    return 0;
}

int Magic3D::ScriptMagic3D::setConfigString(lua_State *lua)
{
    Config::getInstance()->setString(luaL_checkstring(lua, 1), luaL_checkstring(lua, 2));
    return 0;
}

int Magic3D::ScriptMagic3D::getConfigInteger(lua_State *lua)
{
    lua_pushinteger(lua, Config::getInstance()->getInteger(luaL_checkstring(lua, 1)));
    return 1;
}

int Magic3D::ScriptMagic3D::getConfigFloat(lua_State *lua)
{
    lua_pushnumber(lua, Config::getInstance()->getFloat(luaL_checkstring(lua, 1)));
    return 1;
}

int Magic3D::ScriptMagic3D::getConfigBoolean(lua_State *lua)
{
    lua_pushboolean(lua, Config::getInstance()->getBoolean(luaL_checkstring(lua, 1)));
    return 1;
}

int Magic3D::ScriptMagic3D::getConfigString(lua_State *lua)
{
    lua_pushstring(lua, Config::getInstance()->getString(luaL_checkstring(lua, 1)).c_str());
    return 1;
}

int Magic3D::ScriptMagic3D::isConfigured(lua_State *lua)
{
    lua_pushboolean(lua, Config::getInstance()->isConfigured(luaL_checkstring(lua, 1)));
    return 1;
}

int Magic3D::ScriptMagic3D::debugLine(lua_State *lua)
{
    ScriptVector3* l1 = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* l2 = ScriptClass<ScriptVector3>::check(lua, 2);
    bool   ortho = lua_toboolean(lua, 3);
    ScriptColor*   color = ScriptClass<ScriptColor>::check(lua, 4);


    Renderer::getInstance()->drawLine(l1->getValue(), l2->getValue(), ortho, color->getValue());
    return 0;
}

int Magic3D::ScriptMagic3D::rayCast(lua_State *lua)
{
    ScriptVector3* start = ScriptClass<ScriptVector3>::check(lua, 1);
    ScriptVector3* end = ScriptClass<ScriptVector3>::check(lua, 2);

    RayCastReturn ray = Physics::getInstance()->rayCast(start->getValue(), end->getValue(), lua_toboolean(lua, 3));

    if (ray.physicsObject)
    {
        ScriptObject* obj = new ScriptObject(static_cast<Object*>(ray.physicsObject));

        ScriptClass<ScriptObject>::push(lua, obj, true);
    }
    else
    {
        lua_pushnil(lua);
    }

    return 1;
}

int Magic3D::ScriptMagic3D::rotateCamera(lua_State* lua)
{
    float angleX = luaL_checknumber(lua, 1);
    float angleY = luaL_checknumber(lua, 2);
    float angleZ = luaL_checknumber(lua, 3);
    bool rotate = lua_toboolean(lua, 4);

    Quaternion ax = Quaternion::rotationX(Math::radians(angleX));
    Quaternion ay = Quaternion::rotationY(Math::radians(angleY));
    Quaternion az = Quaternion::rotationY(Math::radians(angleZ));

    Camera* camera = Renderer::getInstance()->getCurrentViewPort()->getPerspective();
    if (camera)
    {
        if (rotate)
        {
            camera->setRotation(ay * (camera->getRotation() * ax));
        }
        else
        {
            camera->setRotation(az * ay * ax);
        }
    }

    return 0;
}

int Magic3D::ScriptMagic3D::setStereoscopy(lua_State* lua)
{
    float stereoscopy = lua_toboolean(lua, 1);
    float screenEffects = lua_toboolean(lua, 2);

    Magic3D::getInstance()->setStereoscopy(stereoscopy);

    Material* material = ResourceManager::getInstance()->getMaterials()->get("screen");
    if (material)
    {
        MaterialVar_Boolean* mb = static_cast<MaterialVar_Boolean*>(material->getVar("stereoscopy"));
        if (mb)
        {
            mb->setValue(0, stereoscopy);
        }
    }

    Renderer::getInstance()->setUsingScreenEffects(screenEffects);

    return 0;
}

int Magic3D::ScriptMagic3D::sendText(lua_State* lua)
{
    Network::getInstance()->sendText(luaL_checkstring(lua, 1));
    return 0;
}

int Magic3D::ScriptMagic3D::sendObject(lua_State* lua)
{
    Object* object = ResourceManager::getObjects()->get(luaL_checkstring(lua, 1));
    if (object)
    {
        Network::getInstance()->sendObject(object);
    }
    return 0;
}

int Magic3D::ScriptMagic3D::sendInput(lua_State* lua)
{
    return 0;
}
