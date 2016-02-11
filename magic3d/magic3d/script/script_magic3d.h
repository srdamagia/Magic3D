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

#ifndef SCRIPT_MAGIC3D_H
#define SCRIPT_MAGIC3D_H

#include <magic3d/script/script_object.h>

namespace Magic3D
{
class ScriptMagic3D : public ScriptBasic
{
public:
    static const char className[];
    static Magic3D::ScriptClass<ScriptMagic3D>::ScriptEnum enums[];
    static Magic3D::ScriptClass<ScriptMagic3D>::ScriptFunction functions[];

    ScriptMagic3D(lua_State *lua);
    virtual ~ScriptMagic3D();

    int getObject(lua_State *lua);
    int setCamera3D(lua_State *lua);
    int setCamera2D(lua_State *lua);

    int setLayerVisible(lua_State *lua);
    int isLayerVisible(lua_State *lua);

    int setCursorPosition(lua_State *lua);
    int getCursorX(lua_State *lua);
    int getCursorY(lua_State *lua);

    int setWindowWidth(lua_State *lua);
    int getWindowWidth(lua_State *lua);

    int setWindowHeight(lua_State *lua);
    int getWindowHeight(lua_State *lua);

    int getWindowAspectX(lua_State *lua);
    int getWindowAspectY(lua_State *lua);

    int getTicks(lua_State *lua);
    int getElapsedTime(lua_State *lua);
    int getElapsedTimeReal(lua_State *lua);
    int getTimeSinceStart(lua_State *lua);
    int getTimeScale(lua_State *lua);
    int setTimeScale(lua_State *lua);

    int pick(lua_State *lua);
    int getPosition2D(lua_State *lua);
    int getPosition3D(lua_State *lua);
    int getPosition3DOnPlane(lua_State *lua);

    int log(lua_State *lua);

    int loadScene(lua_State *lua);
    int loadSceneAdditive(lua_State *lua);

    int loadConfig(lua_State *lua);
    int saveConfig(lua_State *lua);

    int isConfigEmpty(lua_State *lua);
    int clearConfig(lua_State *lua);

    int setConfigInteger(lua_State *lua);
    int setConfigFloat(lua_State *lua);
    int setConfigBoolean(lua_State *lua);
    int setConfigString(lua_State *lua);

    int getConfigInteger(lua_State *lua);
    int getConfigFloat(lua_State *lua);
    int getConfigBoolean(lua_State *lua);
    int getConfigString(lua_State *lua);
    int isConfigured(lua_State *lua);

    int debugLine(lua_State *lua);

    int rayCast(lua_State *lua);

    int rotateCamera(lua_State* lua);

    int setStereoscopy(lua_State* lua);
    
    int sendText(lua_State* lua);
    int sendObject(lua_State* lua);
    int sendInput(lua_State* lua);
};
}
#endif /* SCRIPT_MAGIC3D_H */
