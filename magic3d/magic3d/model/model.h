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

#ifndef MAGIC3D_MODEL_H
#define MAGIC3D_MODEL_H

#include <magic3d/object.h>
#include <magic3d/model/animation.h>
#include <magic3d/model/loader.h>

namespace Magic3D
{

#define MODEL_MESH_NAME "model"

#define M3D_MODEL_XML      "Model"
#define M3D_MODEL_XML_FILE "file"

class Model : public Object
{
private:
    Skeleton* skeleton;
    Loader* loader;

    std::string fileName;

    Box box;

    bool uniqueUpdate;

protected:
    Model(const Model& model, std::string name);

public:
    Model(std::string name);
    virtual ~Model();
    virtual void* spawn(std::string name) const;

    virtual bool update();

    void setFileName(std::string name);
    const std::string& getFileName();

    virtual const Box& getBoundingBox();
    virtual void updateBoundingBox();

    Skeleton* getSkeleton(bool empty = false);

    virtual bool load();
    virtual bool load(ModelInfo* modelInfo);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_MODEL_H
