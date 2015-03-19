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

#ifndef SCRIPT_H
#define SCRIPT_H

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <magic3d/type.h>
#include <magic3d/log.h>
#include <magic3d/math.h>

namespace Magic3D
{
#define M3D_SCRIPT_FILE ".script"
#define M3D_SCRIPT_MAIN "magic3d_main_script"
#define M3D_SCRIPT_TABLE "magic3d_script_table"
#define M3D_SCRIPT_OBJECT "magic3d_script_obj_"

#define M3D_SCRIPT_FUNCTION_PROTOTYPE "_prototype_"
#define M3D_SCRIPT_FUNCTION_HELP "_help_"

#define M3D_SCRIPT_FUNCTION_START "start"
#define M3D_SCRIPT_FUNCTION_FINISH "finish"

#define ScriptClassEnum(name, value) {name, value}
#define ScriptClassFunction(class, name, prototype, hint) {#name, &class::name, prototype, hint}

template<typename T> class ScriptClass {

    typedef struct { T* pT; } userdataType;

public:
    typedef int (T::*mfp)(lua_State* lua);
    typedef struct { const char* name; mfp func; const char* prototype; const char* help;} ScriptFunction;
    typedef struct { const char* name; int value;} ScriptEnum;

    static void Register(lua_State* lua)
    {
        lua_newtable(lua);
        int methods = lua_gettop(lua);

        luaL_newmetatable(lua, T::className);
        int metatable = lua_gettop(lua);

        lua_pushvalue(lua, methods);
        lua_setglobal(lua, T::className);

        lua_pushvalue(lua, methods);
        set(lua, metatable, "__metatable");

        lua_pushvalue(lua, methods);
        set(lua, metatable, "__index");

        lua_pushcfunction(lua, tostring_T);
        set(lua, metatable, "__tostring");

        lua_pushcfunction(lua, gc_T);
        set(lua, metatable, "__gc");

        lua_newtable(lua);
        lua_pushcfunction(lua, new_T);
        lua_pushvalue(lua, -1);
        set(lua, methods, "new");
        set(lua, -3, "__call");
        lua_setmetatable(lua, methods);

        for (ScriptEnum* l = T::enums; l->name; l++)
        {
            lua_pushstring(lua, l->name);
            lua_pushinteger(lua, l->value);
            lua_settable(lua, methods);
        }

        for (ScriptFunction* l = T::functions; l->name; l++)
        {
            lua_pushstring(lua, l->name);
            lua_pushlightuserdata(lua, (void*)l);
            lua_pushcclosure(lua, thunk, 1);
            lua_settable(lua, methods);

            lua_pushstring(lua, std::string(std::string(M3D_SCRIPT_FUNCTION_PROTOTYPE) + l->name).c_str());
            lua_pushstring(lua, l->prototype);
            lua_settable(lua, methods);

            lua_pushstring(lua, std::string(std::string(M3D_SCRIPT_FUNCTION_HELP) + l->name).c_str());
            lua_pushstring(lua, l->help);
            lua_settable(lua, methods);
        }

        lua_pop(lua, 2);
    }

    static int call(lua_State* lua, const char *method, int nargs = 0, int nresults = LUA_MULTRET, int errfunc = 0)
    {
        int base = lua_gettop(lua) - nargs;
        if (!luaL_checkudata(lua, base, T::className))
        {
            lua_settop(lua, base-1);
            lua_pushfstring(lua, "not a valid %s userdata", T::className);
            return -1;
        }

        lua_pushstring(lua, method);
        lua_gettable(lua, base);
        if (lua_isnil(lua, -1))
        {
            lua_settop(lua, base-1);
            lua_pushfstring(lua, "%s missing method '%s'", T::className, method);
            return -1;
        }
        lua_insert(lua, base);

        int status = lua_pcall(lua, 1+nargs, nresults, errfunc);
        if (status)
        {
            const char* msg = lua_tostring(lua, -1);
            if (msg == NULL)
            {
                msg = "(error with no message)";
            }
            lua_pushfstring(lua, "%s:%s status = %d\n%s", T::className, method, status, msg);
            lua_remove(lua, base);
            return -1;
        }
        return lua_gettop(lua) - base + 1;
    }

    static int push(lua_State* lua, T* obj, bool gc = false)
    {
        if (!obj)
        {
            lua_pushnil(lua); return 0;
        }
        luaL_getmetatable(lua, T::className);
        if (lua_isnil(lua, -1))
        {
            luaL_error(lua, "%s missing metatable", T::className);
        }
        int mt = lua_gettop(lua);
        subtable(lua, mt, "userdata", "v");
        userdataType* ud = static_cast<userdataType*>(pushuserdata(lua, obj, sizeof(userdataType)));
        if (ud)
        {
            ud->pT = obj;
            lua_pushvalue(lua, mt);
            lua_setmetatable(lua, -2);
            if (gc == false)
            {
                lua_checkstack(lua, 3);
                subtable(lua, mt, "do not trash", "k");
                lua_pushvalue(lua, -2);
                lua_pushboolean(lua, 1);
                lua_settable(lua, -3);
                lua_pop(lua, 1);
            }
        }
        lua_replace(lua, mt);
        lua_settop(lua, mt);
        return mt;
    }


    static T* check(lua_State* lua, int narg) {
        userdataType* ud = static_cast<userdataType*>(luaL_checkudata(lua, narg, T::className));
        if(!ud)
        {
            Log::logFormat(eLOG_FAILURE, "Script check error narg: %d - %s", narg, T::className);
        }
        return ud->pT;  // pointer to T object
    }

private:
    ScriptClass();

    static int thunk(lua_State* lua)
    {
        T* obj = check(lua, 1);
        lua_remove(lua, 1);

        ScriptFunction* l = static_cast<ScriptFunction*>(lua_touserdata(lua, lua_upvalueindex(1)));
        return (obj->*(l->func))(lua);
    }

    static int new_T(lua_State* lua) {
        lua_remove(lua, 1);
        T *obj = new T(lua);
        push(lua, obj, true);
        return 1;
    }

    static int gc_T(lua_State* lua)
    {
        if (luaL_getmetafield(lua, 1, "do not trash"))
        {
            lua_pushvalue(lua, 1);
            lua_gettable(lua, -2);
            if (!lua_isnil(lua, -1))
            {
                return 0;
            }
        }
        userdataType* ud = static_cast<userdataType*>(lua_touserdata(lua, 1));
        T *obj = ud->pT;
        if (obj)
        {
            delete obj;
        }
        return 0;
    }

    static int tostring_T (lua_State* lua)
    {
        char buff[32];
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(lua, 1));
        T* obj = ud->pT;
        sprintf(buff, "%p", (void*)obj);
        lua_pushfstring(lua, "%s (%s)", obj->toString().c_str(), buff);
        return 1;
    }

    static void set(lua_State* lua, int table_index, const char* key)
    {
        lua_pushstring(lua, key);
        lua_insert(lua, -2);
        lua_settable(lua, table_index);
    }

    static void weaktable(lua_State* lua, const char* mode)
    {
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        lua_setmetatable(lua, -2);
        lua_pushliteral(lua, "__mode");
        lua_pushstring(lua, mode);
        lua_settable(lua, -3);
    }

    static void subtable(lua_State* lua, int tindex, const char* name, const char* mode)
    {
        lua_pushstring(lua, name);
        lua_gettable(lua, tindex);
        if (lua_isnil(lua, -1))
        {
            lua_pop(lua, 1);
            lua_checkstack(lua, 3);
            weaktable(lua, mode);
            lua_pushstring(lua, name);
            lua_pushvalue(lua, -2);
            lua_settable(lua, tindex);
        }
    }

    static void* pushuserdata(lua_State* lua, void* key, size_t size)
    {
        void *ud = 0;
        lua_pushlightuserdata(lua, key);
        lua_gettable(lua, -2);
        if (lua_isnil(lua, -1))
        {
            lua_pop(lua, 1);
            lua_checkstack(lua, 3);
            ud = lua_newuserdata(lua, size);
            lua_pushlightuserdata(lua, key);
            lua_pushvalue(lua, -2);
            lua_settable(lua, -4);
        }
        return ud;
    }
};

struct ScriptHelpItem
{
    std::string func;
    std::string prototype;
    std::string help;
};

struct ScriptHelpItems
{
    std::string name;
    std::vector<ScriptHelpItem> items;
};

class PhysicsObject;
class Object;
struct ScriptInfo
{
    Object* object;
    std::string script;
};

class ScriptMagic3D;
class Script
{
private:
    static Script* instance;

    static lua_State* lua;    
    ScriptMagic3D* m3d;

    std::map<std::string, ScriptInfo> scripts;
    std::map<std::string, Object*> temp;
    std::map<std::string, Object*> objects;

    float elapsed;
    float fps;
    bool playing;
    bool hasErrors;
    bool loadingObjects;

    Script();
    ~Script();

    bool reload();
    void resetInputFunctions();
    bool hasFunction(std::string functionName);
    bool load(std::string script);
    bool loadObject(std::string name);
    bool loadObjects();
    bool collectObject(const std::string& name);

    void add(std::string name, std::string script, Object* object);
    void remove(std::string name);
    void log(std::string text);

    void helpItems(std::vector<ScriptHelpItem>* list, const char* table);

    bool executeAllScripts(std::string functionName, bool mainEnd);
public:
    static int scriptObjects;
    bool on_keyboard_down;
    bool on_keyboard_pressed;
    bool on_keyboard_up;

    bool on_mouse_down;
    bool on_mouse_pressed;
    bool on_mouse_up;
    bool on_mouse_move;
    bool on_mouse_wheel;

    bool on_joystick_down;
    bool on_joystick_pressed;
    bool on_joystick_up;

    bool on_touch_down;
    bool on_touch_pressed;
    bool on_touch_up;
    bool on_touch_move;

    bool on_accelerometer;
    bool on_gyroscope;

    static bool start();
    static bool finish();

    void play(bool startScript = true);
    void stop(bool finishScript = true);
    bool isPlaying();
    void clear();
    int getStack();
    void dumpStack();
    void update();
    int getMemoryUsed();

    void add(Object* object);
    void remove(Object* object);
    void setMain(std::string script);
    void removeMain();

    bool startCall(const std::string& functionName);
    bool startCall(const std::string& functionName, const std::string& objectName);
    bool call(const std::string& functionName, int params);
    bool call(const std::string& functionName, int params, const std::string& objectName);

    bool execute(const std::string& functionName, int params, const std::string& objectName = "");

    void help(std::vector<ScriptHelpItems>* list);

    static Script* getInstance();

    void pushInteger(int value);
    void pushNumber(float value);
    void pushString(std::string value);
    void pushBool(bool value);
    void pushNil();

    int checkInteger(int index);
    float checkNumber(int index);
    std::string checkString(int index);
    bool checkBool(int index);
    bool checkNil(int index);

    void call_collision(PhysicsObject* object, PhysicsObject* collider, Vector3 selfPosition, Vector3 objectPosition, Vector3 objectNormal);

    void addToTemp(Object* object);
};
}

#endif /* SCRIPT_H */
