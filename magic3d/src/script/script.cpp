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
#include <magic3d/script/script_input.h>
#include <magic3d/magic3d.h>
#include <magic3d/package.h>

Magic3D::Script* Magic3D::Script::instance = NULL;
lua_State* Magic3D::Script::lua = NULL;
int Magic3D::Script::scriptObjects = 0;

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {NULL, NULL}
};

bool SortHelp(const Magic3D::ScriptHelpItems& lo, const Magic3D::ScriptHelpItems& ro)
{
    return strcasecmp(lo.name.c_str(), ro.name.c_str()) < 0;
}

bool SortHelpItem(const Magic3D::ScriptHelpItem& lo, const Magic3D::ScriptHelpItem& ro)
{
    return lo.func.substr(lo.func.find_first_of(' ') + 1).compare(ro.func.substr(ro.func.find_first_of(' ') + 1)) < 0;
}

Magic3D::Script::Script()
{
    lua = NULL;
    m3d = new ScriptMagic3D(lua);

    playing = false;
    hasErrors = false;
    elapsed = 0.0f;
    fps = 1.0f / 60.0f;

    loadingObjects = false;

    Input::getInstance()->addEventListener(new Script_Input_Keyboard());
    Input::getInstance()->addEventListener(new Script_Input_Mouse());
    Input::getInstance()->addEventListener(new Script_Input_Joystick());
    Input::getInstance()->addEventListener(new Script_Input_Touch());
    Input::getInstance()->addEventListener(new Script_Input_Accelerometer());
    Input::getInstance()->addEventListener(new Script_Input_Gyroscope());

    resetInputFunctions();
}

Magic3D::Script::~Script()
{
    stop();

    if (m3d)
    {
        delete m3d;
        m3d = NULL;
    }
}

bool Magic3D::Script::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new Script();
    }

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Script sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Script not started.");
    }

    return result;
}

bool Magic3D::Script::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Script sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Script not finished.");
    }

    return !instance;
}

Magic3D::Script* Magic3D::Script::getInstance()
{
    return instance;
}

void Magic3D::Script::play(bool startScript)
{
    Log::log(eLOG_SUCCESS, "play");
    if (isPlaying())
    {
        stop(startScript);
    }

    hasErrors = false;
    lua = luaL_newstate();

    const luaL_Reg *lib;
    /* call open functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++)
    {
        luaL_requiref(lua, lib->name, lib->func, 1);
        lua_pop(lua, 1);  /* remove lib */
    }

    std::string m3dtable = M3D_SCRIPT_TABLE;
    m3dtable += " = {__index = _G};";
    if (luaL_loadstring(lua, m3dtable.c_str()) == 0)
    {
        lua_pcall(lua, 0, 0, 0);
        //Log::log(eLOG_PLAINTEXT, "%s created", M3D_SCRIPT_TABLE);
    }
    else
    {
        log("Play()");
    }

    ScriptClass<ScriptMagic3D>::Register(lua);
    ScriptClass<ScriptVector3>::Register(lua);
    ScriptClass<ScriptColor>::Register(lua);
    ScriptClass<ScriptObject>::Register(lua);

    ScriptClass<ScriptMagic3D>::push(lua, m3d);
    lua_setglobal(lua, "magic3d");

    if (ResourceManager::getInstance()->getPackage())
    {
        ResourceManager::getInstance()->getPackage()->open();
    }
    playing = reload();

    if (playing && startScript)
    {
        //executeAllScripts(M3D_SCRIPT_FUNCTION_START, false);

        execute(M3D_SCRIPT_FUNCTION_START, 0);
        playing = loadObjects();
        if (!playing)
        {
            stop(false);
        }
    }
    if (ResourceManager::getInstance()->getPackage())
    {
        ResourceManager::getInstance()->getPackage()->close();
    }
    elapsed = 0.0f;
}

void Magic3D::Script::stop(bool finishScript)
{
    Log::log(eLOG_FAILURE, "stop");
    if (lua)
    {
        if (playing && finishScript)
        {
            executeAllScripts(M3D_SCRIPT_FUNCTION_FINISH, true);
        }
        if (playing)
        {
            std::map<std::string, Object*>::iterator it_o = temp.begin();

            while (it_o != temp.end())
            {
                //std::string name = (*it).first;
                Object* object = (*it_o++).second;

                object->killChildren();
                if (object->getLayer())
                {
                    Scene::getInstance()->removeObject(object->getLayer(), object);
                }
                else if (object->getOctree())
                {
                    object->getOctree()->remove(object);
                }
                ResourceManager::getObjects()->remove(object->getName());
            }

            temp.clear();
        }

        lua_gc(lua, LUA_GCCOLLECT, 0);
        lua_close(lua);
    }
    lua = NULL;

    resetInputFunctions();

    playing = false;
}

bool Magic3D::Script::isPlaying()
{
    return playing;
}

void Magic3D::Script::clear()
{
    Log::log(eLOG_SUCCESS, "clear");
    scripts.clear();
    temp.clear();
    objects.clear();
}

int Magic3D::Script::getStack()
{
    return lua_checkstack(lua, 1);
}

void Magic3D::Script::dumpStack()
{
    int i;
    int top = lua_gettop(lua);

    Log::logFormat(eLOG_FAILURE, "total in stack %d",top);

    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(lua, i);
        switch (t)
        {
            case LUA_TSTRING:  /* strings */
                Log::logFormat(eLOG_FAILURE, "string: '%s'", lua_tostring(lua, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                Log::logFormat(eLOG_FAILURE, "boolean %s",lua_toboolean(lua, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                Log::logFormat(eLOG_FAILURE, "number: %g", lua_tonumber(lua, i));
                break;
            default:  /* other values */
                Log::logFormat(eLOG_FAILURE, "%s", lua_typename(lua, t));
                break;
        }
        Log::log(eLOG_FAILURE, "  ");  /* put a separator */
    }
    Log::log(eLOG_FAILURE, "");  /* end the listing */
}

void Magic3D::Script::update()
{
    if (playing && !hasErrors)
    {
        lua_gc(lua, LUA_GCCOLLECT, 0);
    }
}

int Magic3D::Script::getMemoryUsed()
{
    int result = 0;
    if (playing && !hasErrors)
    {
        result = lua_gc(lua, LUA_GCCOUNT, 0);
    }

    return result;
}

void Magic3D::Script::add(Object* object)
{
    if (object)
    {
        objects[object->getName()] = object;
    }
}

void Magic3D::Script::remove(Object* object)
{
    if (object)
    {
        remove(object->getName());

        if (isPlaying())
        {
            execute(M3D_SCRIPT_FUNCTION_FINISH, 0, object->getName());
            collectObject(object->getName());
        }
    }
}

void Magic3D::Script::setMain(std::string script)
{
    add(M3D_SCRIPT_MAIN, script + ".main", NULL);
}

void Magic3D::Script::removeMain()
{
    remove(M3D_SCRIPT_MAIN);
}

void Magic3D::Script::add(std::string name, std::string script, Object* object)
{
    Log::log(eLOG_SUCCESS, name.c_str());
    std::string ending = M3D_SCRIPT_FILE;

    std::string path = ResourceManager::getInstance()->getScriptPath(script);
    if (path.compare (path.length() - ending.length(), ending.length(), ending) != 0)
    {
        path += ending;
    }

    ScriptInfo info;
    info.object = object;
    info.script = path;

    scripts[name] = info;
}

void Magic3D::Script::remove(std::string name)
{
    //Log::log(eLOG_FAILURE, name.c_str());
    scripts.erase(name);
}

bool Magic3D::Script::startCall(const std::string& functionName)
{
    return startCall(functionName, "");
}

bool Magic3D::Script::startCall(const std::string& functionName, const std::string& objectName)
{
    Magic3D::getInstance()->setTimeReference();
    bool result = false;
    if (isPlaying() && !hasErrors)
    {
        if (!objectName.empty())
        {
            std::string alias(M3D_SCRIPT_OBJECT);
            alias += objectName;

            lua_getglobal(lua, alias.c_str());
            lua_getfield(lua, -1, functionName.c_str());
        }
        else
        {
            lua_getglobal(lua, functionName.c_str());
        }

        result = lua_isfunction(lua, -1);

        if (!result)
        {
            lua_pop(lua, 1);
            if (!objectName.empty())
            {
                lua_pop(lua, 1);
            }
            log(std::string("NOT a function: ") + functionName);
            Scene::getInstance()->addScriptMS(Magic3D::getInstance()->getTimeSinceReference());
        }
    }
    return result;
}

bool Magic3D::Script::call(const std::string& functionName, int params)
{
    return call(functionName, params, "");
}

bool Magic3D::Script::call(const std::string& functionName, int params, const std::string& objectName)
{
    bool result = !hasErrors;

    if (!hasErrors)
    {
        if (lua_pcall(lua, params, LUA_MULTRET, 0) != 0)
        {
            std::string l;
            if (!objectName.empty())
            {
                l += "Object: ";
                l += objectName;
                l += " - ";
            }
            l += "Function: ";
            l += functionName;

            log(l);

            result = false;
        }

        if (!objectName.empty())
        {
            lua_pop(lua, 1);
        }
    }

    Scene::getInstance()->addScriptMS(Magic3D::getInstance()->getTimeSinceReference());

    if (!loadingObjects)
    {
        if (!loadObjects())
        {
            stop(false);
        }
    }

    return result;
}

bool Magic3D::Script::execute(const std::string& functionName, int params, const std::string& objectName)
{
    bool result = false;

    if (startCall(functionName, objectName))
    {
        result = call(functionName, params, objectName);
    }

    return result;
}

void Magic3D::Script::resetInputFunctions()
{
    on_keyboard_down    = false;
    on_keyboard_pressed = false;
    on_keyboard_up      = false;

    on_mouse_down       = false;
    on_mouse_pressed    = false;
    on_mouse_up         = false;
    on_mouse_move       = false;
    on_mouse_wheel      = false;

    on_joystick_down    = false;
    on_joystick_pressed = false;
    on_joystick_up      = false;

    on_touch_down       = false;
    on_touch_pressed    = false;
    on_touch_up         = false;
    on_touch_move       = false;

    on_accelerometer    = false;
    on_gyroscope        = false;
}

bool Magic3D::Script::hasFunction(std::string functionName)
{
    lua_getglobal(lua, functionName.c_str());
    bool result = lua_isfunction(lua, -1);
    lua_pop(lua, 1);

    return result;
}

bool Magic3D::Script::load(std::string script)
{
    bool result = true;

    bool ready = false;
    if (ResourceManager::getInstance()->getPackage())
    {
        Memory mem;
        ready = ResourceManager::getInstance()->unpack(script, &mem);
        std::stringstream str;
        str << mem.getBuffer();

        ready = luaL_dostring (lua, str.str().c_str()) == 0;
    }
    else
    {
        ready = luaL_dofile (lua, script.c_str()) == 0;
    }

    if (!ready)
    {
        hasErrors = true;
        result = false;
        log(std::string("Load: ") + script);
    }
    else
    {
        on_keyboard_down    = hasFunction(M3D_SCRIPT_KEYBOARD_DOWN);
        on_keyboard_pressed = hasFunction(M3D_SCRIPT_KEYBOARD_PRESSED);
        on_keyboard_up      = hasFunction(M3D_SCRIPT_KEYBOARD_UP);

        on_mouse_down       = hasFunction(M3D_SCRIPT_MOUSE_DOWN);
        on_mouse_pressed    = hasFunction(M3D_SCRIPT_MOUSE_PRESSED);
        on_mouse_up         = hasFunction(M3D_SCRIPT_MOUSE_UP);
        on_mouse_move       = hasFunction(M3D_SCRIPT_MOUSE_MOVE);
        on_mouse_wheel      = hasFunction(M3D_SCRIPT_MOUSE_WHEEL);

        on_joystick_down    = hasFunction(M3D_SCRIPT_JOYSTICK_DOWN);
        on_joystick_pressed = hasFunction(M3D_SCRIPT_JOYSTICK_PRESSED);
        on_joystick_up      = hasFunction(M3D_SCRIPT_JOYSTICK_UP);

        on_touch_down       = hasFunction(M3D_SCRIPT_TOUCH_DOWN);
        on_touch_pressed    = hasFunction(M3D_SCRIPT_TOUCH_PRESSED);
        on_touch_up         = hasFunction(M3D_SCRIPT_TOUCH_UP);
        on_touch_move       = hasFunction(M3D_SCRIPT_TOUCH_MOVE);

        on_accelerometer    = hasFunction(M3D_SCRIPT_ACCELEROMETER);
        on_gyroscope        = hasFunction(M3D_SCRIPT_GYROSCOPE);
    }
    return result;
}

bool Magic3D::Script::loadObject(std::string name)
{
    bool result = true;

    ScriptInfo& info = scripts[name];

    std::string alias(M3D_SCRIPT_OBJECT);
    alias += name;

    std::stringstream script;

    script << alias;
    script << " = {}; ";

    script << "setmetatable(";
    script << alias;
    script << ", ";
    script << M3D_SCRIPT_TABLE;
    script << "); ";
    script << "do local _ENV = ";
    script << alias;
    script << "; ";

    script << "local this = magic3d:getObject(\"";
    script << name;
    script << "\"); ";

    if (ResourceManager::getInstance()->getPackage())
    {
        Memory mem;
        ResourceManager::getInstance()->unpack(info.script, &mem);
        script << mem.getBuffer();
    }
    else
    {
        std::ifstream t(info.script.c_str());
        script << t.rdbuf();
    }

    script << " end;";

    Log::logFormat(eLOG_FAILURE, "name: %s - script: %s", alias.c_str(), info.script.c_str());
    if (luaL_loadstring(lua, script.str().c_str()) != 0)
    {
        hasErrors = true;
        result = false;
        log(std::string("Load Object error: ") + name);
    }

    lua_pcall(lua, 0, 0, 0);

    return result;
}

bool Magic3D::Script::loadObjects()
{
    bool needClear = !objects.empty();

    bool loaded = !needClear;

    while (playing && !objects.empty())
    {
        Object* object = (*objects.begin()).second;

        add(object->getName(), object->getScript(), object);
        loaded = loadObject(object->getName());
        if (loaded)
        {
            loadingObjects = true;
            execute(M3D_SCRIPT_FUNCTION_START, 0, object->getName());
            loadingObjects = false;
        }
        objects.erase(objects.begin());

        if (!loaded)
        {
            break;
        }
    }

    if (needClear)
    {
        objects.clear();
    }

    return loaded;
}

bool Magic3D::Script::collectObject(const std::string& name)
{
    bool result = true;

    std::string alias(M3D_SCRIPT_OBJECT);
    alias += name;

    std::stringstream script;

    script << alias;
    script << " = nil; ";

    if (luaL_loadstring(lua, script.str().c_str()) != 0)
    {
        result = false;
        log(std::string("Collect Object: ") + name);
    }

    lua_pcall(lua, 0, 0, 0);

    return result;
}

bool Magic3D::Script::reload()
{
    bool result = true;

    std::string main(M3D_SCRIPT_MAIN);
    std::map<std::string, ScriptInfo>::iterator it = scripts.begin();

    while (it != scripts.end())
    {
        std::string name = (*it).first;
        ScriptInfo& info = (*it++).second;

        if (name.compare(main) == 0)
        {
            result = load(info.script);
        }
        else
        {
            result = loadObject(name);
        }

        if (!result)
        {
            break;
        }
    }    

    return result;
}

bool Magic3D::Script::executeAllScripts(std::string functionName, bool mainEnd)
{
    bool result = true;

    std::string main(M3D_SCRIPT_MAIN);
    std::map<std::string, ScriptInfo>::iterator it = scripts.begin();

    if (!mainEnd)
    {
        execute(functionName, 0);
    }

    while (it != scripts.end())
    {
        std::string name = (*it).first;
        ScriptInfo& info = (*it++).second;

        if (name.compare(main) != 0)
        {
            result = execute(functionName, 0, info.object->getName());
        }

        if (!result)
        {
            break;
        }
    }

    if (mainEnd)
    {
        execute(functionName, 0);
    }

    return result;
}

void Magic3D::Script::log(std::string text)
{
    const char format[] = "Script Error: %s - %s";
    const char* error = lua_tostring(lua, -1);
    if (error)
    {
        int size = strlen(format) + text.size() + strlen(error) + 1;
        char formatedError[size];
        sprintf(formatedError, format, error, text.c_str());
        Log::log(eLOG_FAILURE, formatedError);
    }
}

void Magic3D::Script::helpItems(std::vector<ScriptHelpItem>* list, const char* table)
{
    std::map<std::string, ScriptHelpItem> tmpList;

    lua_getglobal( lua, table );
    int t = lua_gettop(lua);

    lua_pushnil( lua );
    while( lua_next( lua, t ) ) {
        int type = lua_type( lua, -1 );
        if (strcmp(lua_typename( lua, type), "function") == 0)
        {
            std::string name = std::string(lua_tostring( lua, -2));

            if (tmpList.find(name) == tmpList.end())
            {
                ScriptHelpItem item;
                item.prototype = "";
                item.help = "";

                tmpList[std::string(name)] = item;
            }
        }
        else if (strcmp(lua_typename( lua, type), "string") == 0)
        {
            std::string name = std::string(lua_tostring( lua, -2));
            std::string prototype(M3D_SCRIPT_FUNCTION_PROTOTYPE);
            std::string help(M3D_SCRIPT_FUNCTION_HELP);
            std::string tmp;
            bool isPrototype = false;
            if (name.compare (0, prototype.size(), prototype) == 0)
            {
                isPrototype = true;
                tmp = prototype;
            }
            if (name.compare (0, help.size(), help) == 0)
            {
                isPrototype = false;
                tmp = help;
            }
            if (!tmp.empty())
            {
                std::string func = name.substr(tmp.size(), name.size() - tmp.size());
                std::string value = lua_tostring( lua, -1);

                ScriptHelpItem item;
                if (isPrototype)
                {
                    item.prototype = value;
                    item.help = "";
                }
                else
                {
                    item.prototype = "";
                    item.help = value;
                }

                if (tmpList.find(func) != tmpList.end())
                {
                    item = tmpList[func];
                    if (isPrototype)
                    {
                        item.prototype = value;
                    }
                    else
                    {
                        item.help = value;
                    }
                }
                tmpList[func] = item;
            }
        }
        lua_pop( lua, 1 );
    }
    lua_pop( lua, 1 );

    std::map<std::string, ScriptHelpItem>::iterator it = tmpList.begin();

    while (it != tmpList.end())
    {
        std::string func = (*it).first;
        ScriptHelpItem item = (*it++).second;
        item.func = func;

        list->push_back(item);
    }

    std::sort(list->begin(), list->end(), SortHelpItem);
}

void Magic3D::Script::help(std::vector<ScriptHelpItems>* list)
{
    lua_getglobal( lua, "_G" );
    int t = lua_gettop(lua);

    lua_pushnil( lua );
    while( lua_next( lua, t ) ) {
        const char* tableName = lua_tostring( lua, -2);
        const char* typeName = lua_typename( lua, lua_type( lua, -1 ) );
        lua_pop( lua, 1 );
        if (strcmp(typeName, "table") == 0 /*&& strcmp(tableName, "_G") != 0*/ && strcmp(tableName, M3D_SCRIPT_TABLE) != 0)
        {
            ScriptHelpItems items;

            items.name = tableName;
            int length = strlen(M3D_SCRIPT_OBJECT);
            if (items.name.compare(0, strlen(M3D_SCRIPT_OBJECT), M3D_SCRIPT_OBJECT) == 0)
            {
                items.name = items.name.substr(length, items.name.size() - length) + " (Object*)";
            }
            if (items.name.compare(ScriptMagic3D::className) == 0)
            {
                items.name = "magic3d";
            }
            if (items.name.compare("_G") == 0)
            {
                items.name = "global";
            }

            helpItems(&items.items, tableName);
            list->push_back(items);
        }
    }
    lua_pop( lua, 1 );

    std::sort(list->begin(), list->end(), SortHelp);
}

void Magic3D::Script::pushInteger(int value)
{
    lua_pushinteger(lua, value);
}

void Magic3D::Script::pushNumber(float value)
{
    lua_pushnumber(lua, value);
}

void Magic3D::Script::pushString(std::string value)
{
    lua_pushstring(lua, value.c_str());
}

void Magic3D::Script::pushBool(bool value)
{
    lua_pushboolean(lua, value);
}

void Magic3D::Script::pushNil()
{
    lua_pushnil(lua);
}

int Magic3D::Script::checkInteger(int index)
{
    return luaL_checkinteger(lua, index);
}

float Magic3D::Script::checkNumber(int index)
{
    return luaL_checknumber(lua, index);
}

std::string Magic3D::Script::checkString(int index)
{
    return std::string(luaL_checkstring(lua, index));
}

bool Magic3D::Script::checkBool(int index)
{
    return lua_toboolean(lua, index);
}

bool Magic3D::Script::checkNil(int index)
{
    return lua_isnoneornil(lua, index);
}

void Magic3D::Script::call_collision(PhysicsObject* object, PhysicsObject* collider, Vector3 selfPosition, Vector3 objectPosition, Vector3 objectNormal)
{
    if (!isPlaying() || hasErrors)
    {
        return;
    }
    Magic3D::getInstance()->setTimeReference();
    std::string function_collide("collide");

    if (startCall(function_collide, object->getAsObject()->getName()))
    {
        ScriptObject* obj = new ScriptObject(collider->getAsObject());
        ScriptClass<ScriptObject>::push(lua, obj, true);

        ScriptVector3* vec = new ScriptVector3(selfPosition);
        ScriptClass<ScriptVector3>::push(lua, vec, true);

        vec = new ScriptVector3(objectPosition);
        ScriptClass<ScriptVector3>::push(lua, vec, true);

        vec = new ScriptVector3(objectNormal);
        ScriptClass<ScriptVector3>::push(lua, vec, true);

        call(function_collide, 4, object->getAsObject()->getName());
    }
    Scene::getInstance()->addScriptMS(Magic3D::getInstance()->getTimeSinceReference());
}

void Magic3D::Script::addToTemp(Object* object)
{
    temp[object->getName()] = object;
}
