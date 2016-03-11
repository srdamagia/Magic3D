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

#ifndef MAGIC3D_CONFIG_H
#define MAGIC3D_CONFIG_H

#include <magic3d/xml.h>

namespace Magic3D
{
#define CONFIG_FILE "magic3d.config"

#define M3D_CONFIG_XML_ITEM  "item"
#define M3D_CONFIG_XML_KEY   "key"
#define M3D_CONFIG_XML_TYPE  "type"
#define M3D_CONFIG_XML_VALUE "value"

enum CONFIG_TYPE
{
    eCONFIG_INTEGER,
    eCONFIG_FLOAT,
    eCONFIG_BOOLEAN,
    eCONFIG_STRING
};

class ConfigItem
{
private:
    CONFIG_TYPE configType;
public:
    ConfigItem(CONFIG_TYPE configType);
    virtual ~ConfigItem();

    CONFIG_TYPE getType();
};

class ConfigItemInteger : public ConfigItem
{
private:
    int value;
public:
    ConfigItemInteger(int value);
    virtual ~ConfigItemInteger();

    void setValue(int value);
    int getValue();
};

class ConfigItemFloat : public ConfigItem
{
private:
    float value;
public:
    ConfigItemFloat(float value);
    virtual ~ConfigItemFloat();

    void setValue(float value);
    float getValue();
};

class ConfigItemBoolean : public ConfigItem
{
private:
    bool value;
public:
    ConfigItemBoolean(bool value);
    virtual ~ConfigItemBoolean();

    void setValue(bool value);
    bool getValue();
};

class ConfigItemString : public ConfigItem
{
private:
    std::string value;
public:
    ConfigItemString(std::string value);
    virtual ~ConfigItemString();

    void setValue(std::string value);
    const std::string& getValue();
};

class Config : public Xml
{
private:
    std::map<std::string, ConfigItem*> config;
    static Config* instance;

    Config();
    virtual ~Config();

    std::string encrypt(std::string msg);
    std::string decrypt(std::string const& msg);

public:
    static bool start();
    static bool finish();

    static Config* getInstance();

    void setInteger(std::string key, int value);
    void setFloat(std::string key, float value);
    void setBoolean(std::string key, bool value);
    void setString(std::string key, std::string value);

    int getInteger(std::string key);
    float getFloat(std::string key);
    bool getBoolean(std::string key);
    std::string getString(std::string key);

    bool isConfigured(std::string key);

    void clear();
    bool isEmpty();

    virtual bool save();
    virtual bool load();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}
#endif // MAGIC3D_CONFIG_H
