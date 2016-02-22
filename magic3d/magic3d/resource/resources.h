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

#ifndef MAGIC3D_RESOURCES_H
#define MAGIC3D_RESOURCES_H

#include <magic3d/xml.h>

namespace Magic3D
{
#define M3D_RESOURCE_ANIMATION "animation"
#define M3D_RESOURCE_MATERIAL  "material"
#define M3D_RESOURCE_MODEL     "model"
#define M3D_RESOURCE_OBJECT    "object"
#define M3D_RESOURCE_SHADER    "shader"
#define M3D_RESOURCE_TEXTURE   "texture"
#define M3D_RESOURCE_FONT      "font"

template <class M3D_Class>
class Resources
{
private:
    std::string* path;
protected:
    std::map<std::string, M3D_Class*> resources;
    std::map<std::string, int> names;

    std::string resourceName;

    virtual void addDefault()
    {

    }

public:
    Resources()
    {
        path = NULL;
    }

    virtual ~Resources()
    {
        clear(true);
    }

    virtual void setPath(std::string* path)
    {
        this->path = path;

        if (resources.empty())
        {
            addDefault();
        }
    }

    virtual std::string getPath()
    {
        return *path;
    }

    virtual std::string getPath(std::string path)
    {
        return *this->path + path;
    }

    virtual bool add(M3D_Class* resource)
    {
        bool result = false;
        if (resource)
        {
            bool exists = get(resource->getName());
            if (!exists)
            {
                resources[resource->getName()] = resource;
                addName(resource->getName());
            }

            result = !exists;
        }

        return result;
    }

    virtual bool remove(std::string name)
    {
        bool result = false;
        M3D_Class* resource = get(name);
        resources.erase(name);
        if (resource)
        {
            result = true;
            delete resource;
        }

        return result;
    }

    virtual bool replace(std::string name, M3D_Class* newResource)
    {
        bool result = false;
        M3D_Class* resource = get(name);

        if (resource)
        {
            result = true;
            resources[name] = newResource;
            delete resource;
        }

        return result;
    }

    virtual bool rename(std::string name, std::string newName)
    {
        bool result = false;
        M3D_Class* resource = get(newName);

        if (!resource && !newName.empty())
        {
            typename std::map<std::string, M3D_Class*>::iterator it_o;
            it_o = resources.find(name);
            if (it_o != resources.end())
            {
                resource = (*it_o).second;
                resources.erase(it_o);
                resources[newName] = resource;
                result = true;

                addName(newName);
            }
        }

        return result;
    }

    virtual M3D_Class* get(std::string name)
    {
        M3D_Class* result = NULL;
        typename std::map<std::string, M3D_Class*>::const_iterator it_o = resources.find(name);
        if (it_o != resources.end())
        {
            result = (*it_o).second;
        }
        return result;
    }

    virtual void clear(bool all = false)
    {
        names.clear();
        while (!resources.empty())
        {
            M3D_Class* resource = (*resources.begin()).second;
            resources.erase(resources.begin());
            if (resource)
            {
                delete resource;
            }
        }

        if (!all)
        {
            addDefault();
        }
    }

    virtual int count()
    {
        return resources.size();
    }

    virtual std::string getKey(int index)
    {
        typename std::map<std::string, M3D_Class*>::const_iterator it = resources.begin();
        std::advance(it, index);
        return (*it).first;
    }

    virtual M3D_Class* getValue(int index)
    {
        typename std::map<std::string, M3D_Class*>::const_iterator it = resources.begin();
        std::advance(it, index);
        return (*it).second;
    }

    virtual typename std::map<std::string, M3D_Class*>::const_iterator begin()
    {
        return resources.begin();
    }

    virtual typename std::map<std::string, M3D_Class*>::const_iterator end()
    {
        return resources.end();
    }

    virtual void addName(const std::string name)
    {
        unsigned int child = name.rfind('.');

        std::string newName = child == name.npos ? name :  name.substr(child + 1);
        if (newName.length() > 0)
        {
            unsigned int separator = newName.rfind('_');
            if (separator != newName.npos)
            {
                std::string key = newName.substr(0, separator);
                int keyValue = -1;

                if (separator < newName.size() - 1)
                {
                    std::string number = newName.substr(separator + 1);
                    if (Math::isOnlyDigits(number))
                    {
                        int count = atoi(number.c_str());
                        if (names.find(key) != names.end())
                        {
                            int v = names[key];
                            if (count > v)
                            {
                                keyValue = count;
                            }
                        }
                        else
                        {
                            keyValue = count;
                        }
                    }
                    else
                    {
                        key = newName;
                    }
                }

                if (names.find(key) != names.end())
                {
                    keyValue = keyValue >= 0 ? keyValue : names[key];
                }
                names[key] = keyValue + 1;
            }
            else
            {
                if (names.find(newName) != names.end())
                {
                    addName(newName + "_");
                }
                else
                {
                    names[newName] = 0;
                }
            }
        }
    }

    virtual std::string getNewName(const std::string name)
    {
        unsigned int child = name.rfind('.');

        std::string newName = child == name.npos ? name :  name.substr(child + 1);
        if (newName.length() > 0)
        {
            unsigned int separator = newName.rfind('_');
            if (separator != newName.npos)
            {
                std::string key = newName.substr(0, separator);
                int keyValue = -1;

                if (separator < newName.size() - 1)
                {
                    std::string number = newName.substr(separator + 1);
                    if (Math::isOnlyDigits(number))
                    {
                        int count = atoi(number.c_str());
                        if (names.find(key) != names.end())
                        {
                            int v = names[key];
                            if (count > v)
                            {
                                keyValue = v;
                            }
                        }
                    }
                    else
                    {
                        key = newName;
                    }
                }

                if (names.find(key) != names.end())
                {
                    int value = keyValue >= 0 ? keyValue : names[key];
                    char formattedNumber[7];
                    sprintf(formattedNumber, "%6.6d", value);
                    newName = key + "_" + formattedNumber;
                }
            }
            else
            {
                if (names.find(newName) != names.end())
                {
                    newName = getNewName(newName + "_");
                }
            }
        }
        else
        {
            newName = getNewName(resourceName);
        }

        return newName;
    }

};

}

#endif // MAGIC3D_RESOURCES_H
