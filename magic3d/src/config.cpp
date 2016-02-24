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

#include <magic3d/magic3d.h>

Magic3D::Config* Magic3D::Config::instance = NULL;

Magic3D::ConfigItem::ConfigItem(CONFIG_TYPE configType)
{
    this->configType = configType;
}

Magic3D::ConfigItem::~ConfigItem()
{

}

Magic3D::CONFIG_TYPE Magic3D::ConfigItem::getType()
{
    return configType;
}

//******************************************************************************
Magic3D::ConfigItemInteger::ConfigItemInteger(int value) : ConfigItem(eCONFIG_INTEGER)
{
    this->value = value;
}

Magic3D::ConfigItemInteger::~ConfigItemInteger()
{
}

void Magic3D::ConfigItemInteger::setValue(int value)
{
    this->value = value;
}

int Magic3D::ConfigItemInteger::getValue()
{
    return value;
}

//******************************************************************************
Magic3D::ConfigItemFloat::ConfigItemFloat(float value) : ConfigItem(eCONFIG_FLOAT)
{
    this->value  = value;
}

Magic3D::ConfigItemFloat::~ConfigItemFloat()
{
}

void Magic3D::ConfigItemFloat::setValue(float value)
{
    this->value = value;
}

float Magic3D::ConfigItemFloat::getValue()
{
    return value;
}

//******************************************************************************
Magic3D::ConfigItemBoolean::ConfigItemBoolean(bool value) : ConfigItem(eCONFIG_BOOLEAN)
{
    this->value = value;
}

Magic3D::ConfigItemBoolean::~ConfigItemBoolean()
{
}

void Magic3D::ConfigItemBoolean::setValue(bool value)
{
    this->value = value;
}

bool Magic3D::ConfigItemBoolean::getValue()
{
    return value;
}

//******************************************************************************
Magic3D::ConfigItemString::ConfigItemString(std::string value) : ConfigItem(eCONFIG_STRING)
{
    this->value = value;
}

Magic3D::ConfigItemString::~ConfigItemString()
{
}

void Magic3D::ConfigItemString::setValue(std::string value)
{
    this->value = value;
}

const std::string& Magic3D::ConfigItemString::getValue()
{
    return value;
}

//******************************************************************************
Magic3D::Config::Config()
{

}

Magic3D::Config::~Config()
{
    save();
    clear();
}

bool Magic3D::Config::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new Config();
    }

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Config sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Config not started.");
    }

    return result;
}

bool Magic3D::Config::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Config sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Config not finished.");
    }

    return !instance;
}

Magic3D::Config* Magic3D::Config::getInstance()
{
    return instance;
}

void Magic3D::Config::setInteger(std::string key, int value)
{
    std::map<std::string, ConfigItem*>::const_iterator it_c = config.find(key);
    if (it_c != config.end())
    {
        ConfigItem* item = (*it_c).second;
        if (item->getType() == eCONFIG_INTEGER)
        {
            static_cast<ConfigItemInteger*>(item)->setValue(value);
        }
        else
        {
            config.erase(key);
            delete item;
            config[key] = new ConfigItemInteger(value);
        }
    }
    else
    {
        config[key] = new ConfigItemInteger(value);
    }
}

void Magic3D::Config::setFloat(std::string key, float value)
{
    std::map<std::string, ConfigItem*>::const_iterator it_c = config.find(key);
    if (it_c != config.end())
    {
        ConfigItem* item = (*it_c).second;
        if (item->getType() == eCONFIG_FLOAT)
        {
            static_cast<ConfigItemFloat*>(item)->setValue(value);
        }
        else
        {
            config.erase(key);
            delete item;
            config[key] = new ConfigItemFloat(value);
        }
    }
    else
    {
        config[key] = new ConfigItemFloat(value);
    }
}

void Magic3D::Config::setBoolean(std::string key, bool value)
{
    std::map<std::string, ConfigItem*>::const_iterator it_c = config.find(key);
    if (it_c != config.end())
    {
        ConfigItem* item = (*it_c).second;
        if (item->getType() == eCONFIG_BOOLEAN)
        {
            static_cast<ConfigItemBoolean*>(item)->setValue(value);
        }
        else
        {
            config.erase(key);
            delete item;
            config[key] = new ConfigItemBoolean(value);
        }
    }
    else
    {
        config[key] = new ConfigItemBoolean(value);
    }
}

void Magic3D::Config::setString(std::string key, std::string value)
{
    std::map<std::string, ConfigItem*>::const_iterator it_c = config.find(key);
    if (it_c != config.end())
    {
        ConfigItem* item = (*it_c).second;
        if (item->getType() == eCONFIG_STRING)
        {
            static_cast<ConfigItemString*>(item)->setValue(value);
        }
        else
        {
            config.erase(key);
            delete item;
            config[key] = new ConfigItemString(value);
        }
    }
    else
    {
        config[key] = new ConfigItemString(value);
    }
}

int Magic3D::Config::getInteger(std::string key)
{
    int result = 0;
    if (config.find(key) != config.end())
    {
        ConfigItem* item = config[key];
        switch (item->getType())
        {
            case eCONFIG_INTEGER: result = static_cast<ConfigItemInteger*>(item)->getValue(); break;
            case eCONFIG_FLOAT: result = (int)floorf(static_cast<ConfigItemFloat*>(item)->getValue() + 0.5f); break;
            case eCONFIG_BOOLEAN: result = static_cast<ConfigItemBoolean*>(item)->getValue() ? 1 : 0; break;
            case eCONFIG_STRING: result = atoi(static_cast<ConfigItemString*>(item)->getValue().c_str()); break;
        }
    }
    return result;
}

float Magic3D::Config::getFloat(std::string key)
{
    float result = 0.0f;
    if (config.find(key) != config.end())
    {
        ConfigItem* item = config[key];
        switch (item->getType())
        {
            case eCONFIG_INTEGER: result = (float)static_cast<ConfigItemInteger*>(item)->getValue(); break;
            case eCONFIG_FLOAT: result = static_cast<ConfigItemFloat*>(item)->getValue(); break;
            case eCONFIG_BOOLEAN: result = static_cast<ConfigItemBoolean*>(item)->getValue() ? 1.0f : 0.0f; break;
            case eCONFIG_STRING: result = atof(static_cast<ConfigItemString*>(item)->getValue().c_str()); break;
        }
    }
    return result;
}

bool Magic3D::Config::getBoolean(std::string key)
{
    bool result = false;
    if (config.find(key) != config.end())
    {
        ConfigItem* item = config[key];
        switch (item->getType())
        {
            case eCONFIG_INTEGER: result = static_cast<ConfigItemInteger*>(item)->getValue() > 0 ? true : false; break;
            case eCONFIG_FLOAT: result = static_cast<ConfigItemFloat*>(item)->getValue() > 0.0f ? true : false; break;
            case eCONFIG_BOOLEAN: result = static_cast<ConfigItemBoolean*>(item)->getValue(); break;
            case eCONFIG_STRING: result = static_cast<ConfigItemString*>(item)->getValue().compare("1") ? true : false; break;
        }
    }
    return result;
}

std::string Magic3D::Config::getString(std::string key)
{
    std::stringstream result;
    if (config.find(key) != config.end())
    {
        ConfigItem* item = config[key];
        switch (item->getType())
        {
            case eCONFIG_INTEGER: result << static_cast<ConfigItemInteger*>(item)->getValue(); break;
            case eCONFIG_FLOAT: result << static_cast<ConfigItemFloat*>(item)->getValue(); break;
            case eCONFIG_BOOLEAN: result << static_cast<ConfigItemBoolean*>(item)->getValue(); break;
            case eCONFIG_STRING: result << static_cast<ConfigItemString*>(item)->getValue(); break;
        }
    }
    return result.str();
}


bool Magic3D::Config::isConfigured(std::string key)
{
    bool result = false;
    if (config.find(key) != config.end())
    {
        result = true;
    }
    return result;
}

void Magic3D::Config::clear()
{
    while (!config.empty())
    {
        ConfigItem* item = (*config.begin()).second;
        config.erase(config.begin());
        if (item)
        {
            delete item;
        }
    }
}

bool Magic3D::Config::isEmpty()
{
    return config.empty();
}

std::string Magic3D::Config::encrypt(std::string msg)
{
    const std::string key = MAGIC3D_PASSWORD;

    std::string::size_type size = msg.size();
    std::string::size_type keySize = key.size();
    for (std::string::size_type i = 0; i < size; ++i)
    {
        msg[i] ^= key[i % keySize];
    }
    return msg;
}

std::string Magic3D::Config::decrypt(std::string const& msg)
{
    return encrypt(msg);
}

bool Magic3D::Config::save()
{
    bool result = false;
    if (!ResourceManager::getInstance()->getUserPath().empty())
    {
        XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
        doc.LinkEndChild( decl );

        XMLElement* root = doc.NewElement( "MGE" );
        doc.LinkEndChild( root );

        XMLComment* comment = doc.NewComment(" MGE Configuration ");
        root->LinkEndChild( comment );

        save(root);

        //result = doc.SaveFile((ResourceManager::getInstance()->getUserPath() + CONFIG_FILE).c_str()) == XML_SUCCESS;

        std::ofstream configFile;
        configFile.open ((ResourceManager::getInstance()->getUserPath() + CONFIG_FILE).c_str(), std::ios::out | std::ios::binary);

        XMLPrinter printer;
        doc.Print(&printer);
        std::string mStr = encrypt(printer.CStr());
        configFile.write(mStr.c_str(), mStr.size());
        configFile.close();
    }
    return result;
}

bool Magic3D::Config::load()
{
    XMLDocument* doc = new XMLDocument();

    std::ifstream configFile;
    configFile.open ((ResourceManager::getInstance()->getUserPath() + CONFIG_FILE).c_str(), std::ios::binary);

    std::stringstream mStr;
    mStr << configFile.rdbuf();
    configFile.close();

    //bool result = doc->LoadFile((ResourceManager::getInstance()->getUserPath() + CONFIG_FILE).c_str()) == XML_SUCCESS;
    bool result = doc->Parse(decrypt(mStr.str()).c_str()) == XML_SUCCESS;
    if (result)
    {
        XMLElement* pElem = doc->FirstChildElement();

        // should always have a valid root but handle gracefully if it does
        if (pElem)
        {
            load(pElem);
        }
    }

    if (doc)
    {
        delete doc;
        doc = NULL;
    }

    return result;
}

Magic3D::XMLElement* Magic3D::Config::save(XMLElement* root)
{
    if (root)
    {
        std::map<std::string, ConfigItem*>::const_iterator it_c = config.begin();
        while (it_c != config.end())
        {
            XMLElement* xmlItem = root->GetDocument()->NewElement(M3D_CONFIG_XML_ITEM);
            root->LinkEndChild(xmlItem);

            xmlItem->SetAttribute(M3D_CONFIG_XML_KEY, (*it_c).first.c_str());            

            ConfigItem* item = (*it_c).second;
            xmlItem->SetAttribute(M3D_CONFIG_XML_TYPE, item->getType());
            switch (item->getType())
            {
                case eCONFIG_INTEGER: xmlItem->SetAttribute(M3D_CONFIG_XML_VALUE, static_cast<ConfigItemInteger*>(item)->getValue()); break;
                case eCONFIG_FLOAT:   xmlItem->SetAttribute(M3D_CONFIG_XML_VALUE, static_cast<ConfigItemFloat*>(item)->getValue()); break;
                case eCONFIG_BOOLEAN: xmlItem->SetAttribute(M3D_CONFIG_XML_VALUE, static_cast<ConfigItemBoolean*>(item)->getValue()); break;
                case eCONFIG_STRING:  xmlItem->SetAttribute(M3D_CONFIG_XML_VALUE, static_cast<ConfigItemString*>(item)->getValue().c_str()); break;
            }

            ++it_c;
        }
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Config::load(XMLElement* root)
{
    if (root)
    {
        clear();
        XMLElement* xmlItem = root->FirstChildElement();
        while (xmlItem != NULL)
        {
            std::string key = xmlItem->Attribute(M3D_CONFIG_XML_KEY);
            if (!key.empty())
            {
                int ctype = eCONFIG_INTEGER;
                xmlItem->QueryIntAttribute(M3D_CONFIG_XML_TYPE, &ctype);
                ConfigItem* item = NULL;
                switch (ctype)
                {
                    case eCONFIG_INTEGER:
                    {
                        int value = 0;
                        xmlItem->QueryIntAttribute(M3D_CONFIG_XML_VALUE, &value);
                        item = new ConfigItemInteger(value);
                        break;
                    }
                    case eCONFIG_FLOAT:
                    {
                        float value = 0.0f;
                        xmlItem->QueryFloatAttribute(M3D_CONFIG_XML_VALUE, &value);
                        item = new ConfigItemFloat(value);
                        break;
                    }
                    case eCONFIG_BOOLEAN:
                    {
                        bool value = false;
                        xmlItem->QueryBoolAttribute(M3D_CONFIG_XML_VALUE, &value);
                        item = new ConfigItemBoolean(value);
                        break;
                    }
                    case eCONFIG_STRING:
                    {
                        std::string value = xmlItem->Attribute(M3D_CONFIG_XML_VALUE);
                        item = new ConfigItemString(value);
                        break;
                    }
                }

                if (item)
                {
                    config[key] = item;
                }
            }

            xmlItem = xmlItem->NextSiblingElement();
        }
    }
    return root;
}
