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
#include <magic3d/model/loader_dae.h>

Magic3D::LoaderDAE::LoaderDAE()
{
    upZ = false;
    createdSkeleton = false;

    loading = false;

    doc            = NULL;
    rootXML        = NULL;
    modelXML       = NULL;
    firstObjectXML = true;

    matrixUp = Matrix4::rotationX(Math::radians(-90.0f));
}

Magic3D::LoaderDAE::~LoaderDAE()
{
    nodesXML.clear();

    rootXML    = NULL;
    modelXML   = NULL;

    if (doc)
    {
        delete doc;
        doc = NULL;
    }
}

bool Magic3D::LoaderDAE::loadModel(Model* model)
{
    loading = true;
    bool result = false;

    if (model)
    {
        doc            = NULL;
        rootXML        = NULL;
        firstObjectXML = true;

        modelXML = model;

        std::string dae = ResourceManager::getModels()->getPath(modelXML->getFileName());

        if (!getModelInfo(modelXML->getFileName()))
        {
            doc = new XMLDocument();
        }
        else
        {
            model->load(getModelInfo());
        }

        bool result = false;
        if (ResourceManager::getInstance()->getPackage())
        {
            Memory mem;
            result = ResourceManager::getInstance()->unpack(dae, &mem);
            std::string str = mem.getBuffer()->str();
            result = doc->Parse(str.c_str(), str.size()) == XML_SUCCESS && result;
        }
        else
        {
            result = doc->LoadFile(dae.c_str()) == XML_SUCCESS;
        }

        if (!getModelInfo() && result)
        {
            rootXML = doc->FirstChildElement("COLLADA");
            // should always have a valid root but handle gracefully if it does
            if (rootXML)
            {
                addModelInfo(modelXML->getFileName());

                XMLElement* up = rootXML->FirstChildElement("asset")->FirstChildElement("up_axis");

                if (up && strcmp(up->GetText(), "Z_UP") == 0)
                {
                    upZ = true;
                }
                else
                {
                    upZ = false;
                }

                XMLElement* node = rootXML->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node");

                if (node)
                {
                    nodesXML.push_back(node);
                }

                firstObjectXML = true;
            }
            else
            {
                if (doc)
                {
                    delete doc;
                    doc = NULL;
                    Log::logFormat(eLOG_FAILURE, "Model \"%s\" can't load file: %s", model->getName().c_str(), dae.c_str());
                }

                rootXML    = NULL;
                modelXML   = NULL;

                loading = false;
            }
        }
        else
        {
            if (!getModelInfo())
            {
                delete doc;
                doc = NULL;
                Log::logFormat(eLOG_FAILURE, "Model \"%s\" can't load file: %s", model->getName().c_str(), dae.c_str());
            }

            modelXML   = NULL;
            loading = false;
        }
    }
    return result;
}

bool Magic3D::LoaderDAE::loadModel()
{
    bool result = false;

    if (nodesXML.size() > 0)
    {
        XMLElement* node = nodesXML.back();
        nodesXML.pop_back();
        XMLElement* next = node->NextSiblingElement("node");

        if (next)
        {
            nodesXML.push_back(next);
        }

        loadNode(rootXML, node);

        Scene::getInstance()->update();

        result = true;
    }
    else
    {
        if (modelXML)
        {
            Log::logFormat(eLOG_RENDERER, "Model \"%s\" loaded: %s", modelXML->getName().c_str(), modelXML->getFileName().c_str());
        }
        nodesXML.clear();

        rootXML    = NULL;
        modelXML   = NULL;

        if (doc)
        {
            delete doc;
            doc = NULL;
        }

        loading = false;
    }
    return result;
}

bool Magic3D::LoaderDAE::isLoading()
{
    return loading;
}

void Magic3D::LoaderDAE::loadNode(XMLElement* root, XMLElement* node)
{
    if (node)
    {
        /*std::string objectName;

        const char* objName = node->Attribute("id");
        if (objName)
        {
            objectName = std::string(objName);
        }
        else
        {
            objName = node->Attribute("name");
            if (objName)
            {
                objectName = std::string(objName);
            }
        }*/

        //Object* obj = ResourceManager::getObjects()->get(objectName);
        Object* obj = NULL;

        XMLElement* attribute = NULL;

        Matrix4 matrix = Matrix4::identity();
        if (loading && !obj)
        {
            Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
            Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
            Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

            attribute = node->FirstChildElement("translate");
            if (attribute)
            {
                position = parseVector3(attribute->GetText());
            }

            attribute = node->FirstChildElement("scale");
            if (attribute)
            {
                scale = parseVector3(attribute->GetText());
            }

            attribute = node->FirstChildElement("rotate");
            while (attribute)
            {
                Vector4 axisAngle = parseVector4(attribute->GetText());
                setElement(rotation, axisAngle);
                attribute = attribute->NextSiblingElement("rotate");
            }

            attribute = node->FirstChildElement("matrix");
            if (attribute)
            {
                matrix = parseMatrix4(attribute->GetText());

                matrix = transpose(matrix);

                if (upZ)
                {
                    matrix = matrixUp * matrix;
                }

                /*loading->setPosition(matrix.getTranslation());
                loading->setRotation(Quaternion(matrix.getUpper3x3()));*/
            }
            else
            {
                if (upZ)
                {
                    position = Vector3(position.getX(), position.getZ(), -position.getY());
                    rotation = Vector3(rotation.getX(), rotation.getZ(), -rotation.getY());
                    scale    = Vector3(scale.getX(),    scale.getZ(),    scale.getY());
                }

                /*loading->setPosition(position);
                loading->setRotationEuler(rotation);
                loading->setScale(scale);*/

                matrix = Matrix4(Math::quaternion(rotation), position);
                matrix = appendScale(matrix, scale);
            }
        }

        attribute = node->FirstChildElement("instance_geometry");
        if (attribute && !attribute->NextSiblingElement("instance_geometry"))
        {
            if (upZ)
            {
                matrix = matrixUp;
            }
            else
            {
                matrix = Matrix4::identity();
            }
        }

        while (attribute)
        {
            /*if (obj)
            {
                loading = static_cast<Model*>(obj);
            }
            else if (firstObjectXML)
            {
                loading = model;
            }
            firstObjectXML = false;

            if (!loading && !obj)
            {
                loading = new Model(objectName);
                ResourceManager::getObjects()->add(loading);
                Scene::getInstance()->addObject(loading);
            }*/

            if (modelXML)
            {
                if (firstObjectXML)
                {
                    modelXML->clearMeshes();
                }
                firstObjectXML = false;

                addMesh(root, NULL, attribute, attribute->Attribute("url"), matrix);
            }

            attribute = attribute->NextSiblingElement("instance_geometry");
        }

        attribute = node->FirstChildElement("instance_controller");
        while (attribute)
        {
           /* if (obj)
            {
                loading = static_cast<Model*>(obj);
            }
            else if (firstObjectXML)
            {
                loading = model;
            }
            firstObjectXML = false;

            if (!loading && !obj)
            {
                loading = new Model(objectName);
                ResourceManager::getObjects()->add(loading);
                Scene::getInstance()->addObject(loading);
            }*/

            if (modelXML)
            {
                if (firstObjectXML)
                {
                    modelXML->clearMeshes();
                }
                firstObjectXML = false;

                const char* skinName = attribute->Attribute("url");
                XMLElement* controllers = root->FirstChildElement("library_controllers")->FirstChildElement("controller");

                boneName.clear();
                bone.clear();
                while (controllers)
                {
                    if (strcmp(controllers->Attribute("id"), &skinName[1]) == 0)
                    {
                        XMLElement* skin = controllers->FirstChildElement("skin");

                        loadBoneNames(skin);

                        controllers = NULL;
                    }
                    else
                    {
                        controllers = controllers->NextSiblingElement("controller");
                    }
                }

                XMLElement* skeleton = attribute->FirstChildElement("skeleton");

                boneList.clear();

                createdSkeleton = false;

                bool hasSkeleton = false;

                XMLElement* skeletonNode = root->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node");

                while (skeleton)
                {
                    hasSkeleton = true;
                    addSkeleton(skeletonNode, skeleton->GetText(), true);

                    skeleton = skeleton->NextSiblingElement("skeleton");
                }

                if (!hasSkeleton && boneName.size() > 0)
                {
                    std::string rootBone = "#" + boneName[0];
                    addSkeleton(skeletonNode, rootBone.c_str(), false);
                }

                //loadAnimations(root, getModelInfo()->getSkeleton());

                if (upZ)
                {
                    matrix = matrixUp;
                }
                else
                {
                    matrix = Matrix4::identity();
                }

                controllers = root->FirstChildElement("library_controllers")->FirstChildElement("controller");
                while (controllers)
                {
                    if (strcmp(controllers->Attribute("id"), &skinName[1]) == 0)
                    {
                        XMLElement* skin = controllers->FirstChildElement("skin");

                        loadBindPose(skin);

                        while (skin)
                        {
                            addMesh(root, skin, attribute, skin->Attribute("source"), matrix);

                            skin = skin->NextSiblingElement("skin");
                        }

                        controllers = NULL;
                    }
                    else
                    {
                        controllers = controllers->NextSiblingElement("controller");
                    }
                }

                loadAnimations(root, getModelInfo()->getSkeleton());
            }

            attribute = attribute->NextSiblingElement("instance_controller");
        }

        if (modelXML)
        {
            if (getModelInfo()->getSkeleton())
            {
                modelXML->getSkeleton(true)->copyFrom(getModelInfo()->getSkeleton());
            }
            modelXML->updateBoundingBox();
            /*if (parent)
            {
                loading->setParent(parent);
            }*/
        }

        attribute = node->FirstChildElement("node");
        if (attribute)
        {
            nodesXML.push_back(attribute);
        }
    }
}

Magic3D::ColorRGBA Magic3D::LoaderDAE::getColor(const char* color)
{
    std::istringstream tokken (color);
    std::string value;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    if(tokken.good())
    {
        tokken >> value;
        r = atof(value.c_str());
    }
    if(tokken.good())
    {
        tokken >> value;
        g = atof(value.c_str());
    }
    if(tokken.good())
    {
        tokken >> value;
        b = atof(value.c_str());
    }
    if(tokken.good())
    {
        tokken >> value;
        a = atof(value.c_str());
    }

    return ColorRGBA(r, g, b, a);
}

void Magic3D::LoaderDAE::setElement(Vector3& vector, Vector4& axis)
{
    if (axis.getZ() != 0.0f)
    {
        vector.setZ(axis.getW());
    }
    else if (axis.getY() != 0.0f)
    {
        vector.setY(axis.getW());
    }
    else if (axis.getX() != 0.0f)
    {
        vector.setX(axis.getW());
    }
}

void Magic3D::LoaderDAE::addMesh(XMLElement* root, XMLElement* skin, XMLElement* mesh, const char* meshName, Matrix4& matrix)
{
    if (root && mesh)
    {
        XMLElement* geometry = root->FirstChildElement("library_geometries")->FirstChildElement("geometry");

        while (geometry)
        {
            if (strcmp(geometry->Attribute("id"), &meshName[1]) == 0)
            {
                const char* meshRealName = geometry->Attribute("name");
                if (!meshRealName)
                {
                    meshRealName = &meshName[1];
                }
                geometry = geometry->FirstChildElement("mesh");

                if (geometry)
                {
                    position.clear();
                    normal.clear();
                    uv[0].clear();
                    uv[1].clear();
                    color.clear();

                    bool hasVertex = false;
                    bool hasNormal = false;
                    bool hasUV1    = false;
                    bool hasUV2    = false;
                    bool hasColor  = false;

                    int vertices = 0;
                    XMLElement* poly = geometry->FirstChildElement("polylist");
                    if (!poly)
                    {
                        poly = geometry->FirstChildElement("polygons");
                        vertices = 3;
                    }
                    if (!poly)
                    {
                        poly = geometry->FirstChildElement("triangles");
                        vertices = 3;
                    }

                    int group = 0;
                    while (poly)
                    {
                        XMLElement* input = poly->FirstChildElement("input");

                        while (input)
                        {
                            const char* semantic = "semantic";
                            const char* source = "source";

                            if (!hasVertex && strcmp("VERTEX", input->Attribute(semantic)) == 0)
                            {
                                hasVertex = true;
                                loadVertex(geometry);

                                loadWeight(skin);
                            }
                            else if (!hasNormal && strcmp("NORMAL", input->Attribute(semantic)) == 0)
                            {
                                hasNormal = true;
                                loadNormal(geometry, input->Attribute(source));
                            }
                            else if (strcmp("TEXCOORD", input->Attribute(semantic)) == 0)
                            {
                                int index = input->Attribute("set") ? atoi(input->Attribute("set")) : 0;

                                switch (index)
                                {
                                    case 0: hasUV1 = true; break;
                                    case 1: hasUV2 = true; break;
                                }

                                if (index < M3D_MULTI_TEXTURE_COUNT)
                                {
                                    loadUV(geometry, input->Attribute(source), index);
                                }
                            }
                            else if (!hasColor && strcmp("COLOR", input->Attribute(semantic)) == 0)
                            {
                                hasColor = true;
                                loadColor(geometry, input->Attribute(source));
                            }

                            input = input->NextSiblingElement("input");
                        }

                        XMLElement* data = poly->FirstChildElement("p");

                        if (data)
                        {
                            int faces = atoi(poly->Attribute("count"));

                            std::stringstream sstm;
                            sstm << std::string(meshRealName);
                            if (group > 0)
                            {
                                sstm  << group;
                            }
                            group++;

                            MeshData* modelMeshData = new MeshData(eMESH_TRIANGLES, sstm.str());

                            std::stringstream info;
                            while (data)
                            {
                                info << data->GetText() << " ";
                                data = data->NextSiblingElement("p");
                            }
                            std::string value;

                            XMLElement* vcount = poly->FirstChildElement("vcount");
                            std::stringstream tokken;

                            if (vcount)
                            {
                                tokken << vcount->GetText();
                            }

                            for (int f = 0; f < faces; f++)
                            {
                                if (vcount && tokken.good())
                                {
                                    tokken >> value;
                                    vertices = atoi(value.c_str());
                                }

                                Vertex3D* vertex = new Vertex3D[vertices];
                                for (int i = 0; i < vertices; i++)
                                {
                                    if (info.good())
                                    {
                                        if (hasVertex)
                                        {
                                            info >> value;
                                            int index = atoi(value.c_str());

                                            vertex[i].position = (matrix * Vector4(this->position[index].position, 1.0f)).getXYZ();

                                            for (int s = 0; s < M3D_MULTI_SKIN_COUNT; s++)
                                            {
                                                vertex[i].bones.setElem(s, this->position[index].bone[s]);
                                                vertex[i].weights.setElem(s, this->position[index].weight[s]);
                                            }

                                            if (!hasNormal && (int)this->normal.size() > index)
                                            {
                                                vertex[i].normal = (matrix * Vector4(this->normal[index], 0.0f)).getXYZ();
                                            }
                                        }
                                        if (hasNormal)
                                        {
                                            info >> value;
                                            int index = atoi(value.c_str());

                                            if ((int)this->normal.size() > index)
                                            {
                                                vertex[i].normal = (matrix * Vector4(this->normal[index], 0.0f)).getXYZ();
                                            }
                                        }
                                        if (hasUV1 || hasUV2)
                                        {
                                            info >> value;
                                            int index = atoi(value.c_str());

                                            if (hasUV1)
                                            {
                                                vertex[i].uv[0] = this->uv[0][index];
                                                vertex[i].uv[1] = this->uv[0][index];
                                            }
                                            if (hasUV2)
                                            {
                                                vertex[i].uv[1] = this->uv[1][index];
                                                if (!hasUV1)
                                                {
                                                    vertex[i].uv[0] = this->uv[1][index];
                                                }
                                            }
                                        }
                                        if (hasColor)
                                        {
                                            info >> value;
                                            int index = atoi(value.c_str());

                                            vertex[i].color = ColorRGBA(this->color[index], 1.0f);
                                        }
                                    }
                                }

                                if (vertices > 0)
                                {
                                    int index = modelMeshData->getVertices()->size();

                                    for (int v = 0; v < vertices; v++)
                                    {
                                        modelMeshData->addVertex(vertex[v]);
                                        if (getModelInfo()->getSkeleton())
                                        {
                                            modelMeshData->setSkinned(true);
                                        }
                                    }

                                    int total = vertices - 2;

                                    for (int v = 0; v < total; v++)
                                    {
                                        modelMeshData->addTriangle(TriangleIndexes(index + 0, index + v + 1, index + v + 2));
                                    }
                                }

                                if (vcount)
                                {
                                    vertices = 0;
                                }

                                delete[] vertex;
                            }


                            if (modelMeshData->getIndexes()->size() > 0)
                            {
                                modelMeshData->createVbo();
                                addMaterial(root, mesh, poly->Attribute("material"), modelMeshData);
                            }
                        }


                        XMLElement* next = poly->NextSiblingElement("polylist");
                        vertices = 0;
                        if (!next)
                        {
                            next = poly->NextSiblingElement("polygons");
                            vertices = 3;
                        }
                        if (!next)
                        {
                            next = poly->NextSiblingElement("triangles");
                            vertices = 3;
                        }

                        poly = next;
                    }
                }

                geometry = NULL;
            }
            else
            {
                geometry = geometry->NextSiblingElement("geometry");
            }
        }
    }
}

bool Magic3D::LoaderDAE::addSkeleton(XMLElement* root, const char* rootBone, bool compareRoot)
{
    bool result = false;

    if (root && modelXML)
    {
        XMLElement* node = root;

        while (node)
        {
            XMLElement* subNode = node->FirstChildElement("node");
            const char* name = node->Attribute("id");

            bool found = strcmp(name, &rootBone[1]) == 0;

            while (subNode || found)
            {
                const char* joint = subNode->Attribute("type");
                bool isJoint = joint && strcmp(joint, "JOINT") == 0;

                if (isJoint && (!compareRoot || ((strcmp(subNode->Attribute("id"), &rootBone[1]) == 0 || found))))
                {
                    Skeleton* skeleton = getModelInfo()->getSkeleton();

                    if (!skeleton)
                    {
                        skeleton = new Skeleton(NULL, name);
                        getModelInfo()->setSkeleton(skeleton);
                    }

                    addBone(node, skeleton, -1, Matrix4::identity());

                    result = true;
                }

                if (!result && addSkeleton(subNode, rootBone, compareRoot))
                {
                    subNode = NULL;
                    result = true;
                }
                else
                {
                    subNode = subNode->NextSiblingElement("node");
                }

                found = false;
            }

            if (result)
            {
                node = NULL;
            }
            else
            {
                node = node->NextSiblingElement("node");
            }
        }
    }

    return result;
}

void Magic3D::LoaderDAE::loadBoneNames(XMLElement* root)
{
    XMLElement* input = root->FirstChildElement("joints")->FirstChildElement("input");

    const char* source = "source";

    while (input)
    {
        if (strcmp("JOINT", input->Attribute("semantic")) == 0)
        {
            const char* name = input->Attribute(source);

            XMLElement* s = root->FirstChildElement(source);

            while (s)
            {
                if (strcmp(s->Attribute("id"), &name[1]) == 0)
                {
                    XMLElement* param = s->FirstChildElement("technique_common")->FirstChildElement("accessor")->FirstChildElement("param");

                    if (param)
                    {
                        std::string arrayname = std::string(param->Attribute("type")) + "_array";
                        arrayname[0] = std::toupper(arrayname[0]);

                        XMLElement* array = s->FirstChildElement(arrayname.c_str());

                        if (array)
                        {
                            std::istringstream data (array->GetText());
                            std::string value;

                            while (data.good())
                            {
                                data >> value;
                                boneName.push_back(std::string(value));
                                bone.push_back(NULL);
                            }
                        }
                    }

                    s = NULL;
                }
                else
                {
                    s = s->NextSiblingElement(source);
                }
            }

            input = NULL;
        }
        else
        {
            input = input->NextSiblingElement("input");
        }
    }
}

void Magic3D::LoaderDAE::addBone(XMLElement* root, Skeleton* skeleton, int parent, const Matrix4& matrixParent)
{
    if (root && skeleton)
    {
        const char* boneSid = root->Attribute("sid");
        if (!boneSid)
        {
            boneSid = root->Attribute("name");
        }

        int boneIndex = 0;
        bool isBoneInList = false;
        if (boneSid)
        {
            std::vector<std::string>::const_iterator it_s = boneName.begin();
            while (it_s != boneName.end())
            {
                std::string b_name = *it_s++;

                if (b_name.compare(boneSid) == 0)
                {
                    isBoneInList = true;
                    break;
                }

                boneIndex++;
            }
        }

        Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 scale = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 rotation = Vector3(1.0f, 1.0f, 1.0f);
        Matrix4 matrix = Matrix4::identity();

        XMLElement* attribute = root->FirstChildElement("translate");
        if (attribute)
        {
            position = parseVector3(attribute->GetText());
        }

        attribute = root->FirstChildElement("scale");
        if (attribute)
        {
            scale = parseVector3(attribute->GetText());
        }

        bool j = false;
        attribute = root->FirstChildElement("rotate");
        while (attribute)
        {
            const char* sid = attribute->Attribute("sid");
            if (sid && sid[0] == 'j')
            {
                j = true;
            }
            if (!sid || (j && sid[0] == 'j') || (!j && sid[0] == 'r'))
            {
                Vector4 axisAngle = parseVector4(attribute->GetText());
                setElement(rotation, axisAngle);
            }
            attribute = attribute->NextSiblingElement("rotate");
        }

        attribute = root->FirstChildElement("matrix");
        if (attribute)
        {
            matrix = parseMatrix4(attribute->GetText());

            matrix = transpose(matrix);
        }
        else
        {
            Quaternion quat = Math::quaternion(rotation);
            matrix = Matrix4(quat, position);
            appendScale(matrix, scale);
        }

        Bone* bone = NULL;
        int skeletonIndex = -1;

        if (isBoneInList)
        {
            std::string id = std::string(root->Attribute("id"));

            std::map<std::string, int>::iterator it = boneList.find(id);

            if (it == boneList.end())
            {
                bone = new Bone(std::string(boneSid), skeleton, skeleton->getBoneCount());

                Matrix4 m = matrixParent * matrix;

                if (upZ && parent < 0)
                {
                    m = matrixUp * m;
                }
                bone->setMatrix(m);

                matrix = Matrix4::identity();

                if (parent >= 0)
                {
                    bone->setParentIndex(parent);
                    skeleton->getBone(parent)->getChilds()->push_back(skeleton->getBoneCount());
                }

                skeleton->add(bone);
                skeletonIndex = skeleton->getBoneCount() - 1;

                if (!isBoneInList)
                {
                    this->bone.push_back(bone);
                    boneList[id] = this->bone.size() - 1;
                }
                else
                {
                    this->bone[boneIndex] = bone;
                    boneList[id] = boneIndex;
                }
            }
        }
        else
        {
            matrix = matrixParent * matrix;
        }

        XMLElement* node = root->FirstChildElement("node");

        while (node)
        {
            addBone(node, skeleton, bone ? skeletonIndex : parent, matrix);

            node = node->NextSiblingElement("node");
        }
    }
}

void Magic3D::LoaderDAE::loadBindPose(XMLElement* root)
{
    if (root)
    {
        XMLElement* bsm = root->FirstChildElement("bind_shape_matrix");

        Matrix4 bindMatrix = parseMatrix4(bsm->GetText());
        bindMatrix = transpose(bindMatrix);

        XMLElement* input = root->FirstChildElement("joints")->FirstChildElement("input");

        const char* source = "source";

        while (input)
        {
            if (strcmp("INV_BIND_MATRIX", input->Attribute("semantic")) == 0)
            {
                const char* name = input->Attribute(source);

                XMLElement* s = root->FirstChildElement(source);

                while (s)
                {
                    if (strcmp(s->Attribute("id"), &name[1]) == 0)
                    {
                        XMLElement* array = s->FirstChildElement("float_array");

                        int count = atoi(array->Attribute("count")) / 16;

                        if ((int)bone.size() >= count)
                        {
                            std::istringstream data (array->GetText());
                            std::string value;

                            for (int c = 0; c < count; c++)
                            {
                                int size = 16;
                                float matrix[size];
                                for (int m = 0; m < size; m++)
                                {
                                    if (data.good())
                                    {
                                        data >> value;
                                        matrix[m] = atof(value.c_str());
                                    }
                                }

                                Matrix4 bind;
                                Renderer::getInstance()->arrayToMatrix(matrix, bind);

                                bind = transpose(bind) * bindMatrix;

                                if (upZ)
                                {
                                    bind = bind * Matrix4::rotationX(Math::radians(90.0f));
                                }

                                if (bone[c])
                                {
                                    bone[c]->setMatrixInverse(bind);
                                }
                            }
                        }

                        s = NULL;
                    }
                    else
                    {
                        s = s->NextSiblingElement(source);
                    }
                }

                input = NULL;
            }
            else
            {
                input = input->NextSiblingElement("input");
            }
        }
    }
}

void Magic3D::LoaderDAE::loadWeight(XMLElement* root)
{
    if (root)
    {
        XMLElement* weights = root->FirstChildElement("vertex_weights");
        XMLElement* input = weights->FirstChildElement("input");

        const char* source = "source";

        while (input)
        {
            if (strcmp("WEIGHT", input->Attribute("semantic")) == 0)
            {
                const char* name = input->Attribute(source);

                XMLElement* s = root->FirstChildElement(source);

                while (s)
                {
                    if (strcmp(s->Attribute("id"), &name[1]) == 0)
                    {
                        XMLElement* array = s->FirstChildElement("float_array");
                        XMLElement* vcount = weights->FirstChildElement("vcount");
                        XMLElement* v = weights->FirstChildElement("v");

                        std::istringstream data (array->GetText());
                        std::istringstream dataVCount (vcount->GetText());
                        std::istringstream dataV (v->GetText());

                        std::string value;

                        int count = atoi(array->Attribute("count"));

                        for (int c = 0; c < count; c++)
                        {
                            if (data.good())
                            {
                                data >> value;

                                weight.push_back(atof(value.c_str()));
                            }
                        }

                        int vertices = 0;
                        while(dataVCount.good())
                        {
                            dataVCount >> value;

                            int vskin = atoi(value.c_str());

                            if (dataV.good())
                            {
                                for (int i = 0; i < vskin; i++)
                                {

                                    dataV >> value;
                                    int j = atoi(value.c_str());

                                    dataV >> value;
                                    int w = atoi(value.c_str());

                                    if (i < M3D_MULTI_SKIN_COUNT)
                                    {
                                        if (w < (int)weight.size())
                                        {
                                            position[vertices].weight[i] = weight[w];
                                        }
                                        if (j < (int)bone.size())
                                        {
                                            position[vertices].bone[i] = j;
                                        }
                                    }
                                }
                            }

                            vertices++;
                        }

                        s = NULL;
                    }
                    else
                    {
                        s = s->NextSiblingElement(source);
                    }
                }

                input = NULL;
            }
            else
            {
                input = input->NextSiblingElement("input");
            }
        }
    }
}

void Magic3D::LoaderDAE::loadAnimations(XMLElement* root, Skeleton* skeleton)
{
    if (root && skeleton)
    {
        XMLElement* node = root->FirstChildElement("library_animations");

        if (node)
        {
            node = node->FirstChildElement("animation");
        }

        while (node)
        {
            XMLElement* channel = node->FirstChildElement("channel");
            if (!channel)
            {
                channel = node->FirstChildElement("animation")->FirstChildElement("channel");
            }
            while (channel)
            {
                std::vector<float> time;
                std::vector<Matrix4> matrix;

                const char* source = channel->Attribute("source");
                std::string target = std::string(channel->Attribute("target"));

                target = target.substr(0, target.find_first_of('/'));

                XMLElement* sampler = node->FirstChildElement("sampler");
                if (!sampler)
                {
                    sampler = node->FirstChildElement("animation")->FirstChildElement("sampler");
                }
                while (sampler)
                {
                    if (strcmp(sampler->Attribute("id"), &source[1]) == 0)
                    {
                        XMLElement* input = sampler->FirstChildElement("input");
                        while (input)
                        {
                            const char* s = input->Attribute("source");

                            if (strcmp("INPUT", input->Attribute("semantic")) == 0)
                            {
                                loadAnimationInput(node, s, time);
                            }
                            else if (strcmp("OUTPUT", input->Attribute("semantic")) == 0)
                            {
                                loadAnimationOutput(node, s, matrix);
                            }

                            input = input->NextSiblingElement("input");
                        }
                    }

                    sampler = sampler->NextSiblingElement("sampler");
                }

                Matrix4 m;

                int keys = time.size();

                if (keys == (int)matrix.size())
                {
                    for (int i = 0; i < keys; i++)
                    {
                        float t = time[i];
                        m = matrix[i];

                        if (bone.size() > 0 && boneList.size() > 0)
                        {
                            std::map<std::string, int>::iterator it;
                            it = boneList.find(target);

                            if (it != boneList.end())
                            {
                                int b = it->second;

                                if (upZ && skeleton->getBone(b) && !skeleton->getBone(b)->getParent())
                                {
                                    m = matrixUp * m;
                                }

                                if (skeleton->getBone(b))
                                {
                                    skeleton->getAnimation()->addBone(t, b, m);
                                }
                            }
                        }
                    }
                }

                channel = channel->NextSiblingElement("channel");
            }

            node = node->NextSiblingElement("animation");
        }

        skeleton->getAnimation()->setCurrentFrame(0);
    }

    if (skeleton && skeleton->getAnimation() && !skeleton->getAnimation()->getSequences())
    {
        bool created = false;
        skeleton->getAnimation()->setSequences(ResourceManager::getAnimations()->load(modelXML->getFileName(), created));
    }
}

void Magic3D::LoaderDAE::loadAnimationInput(XMLElement* root, const char* source, std::vector<float>& time)
{
    XMLElement* input = root->FirstChildElement("source");
    if (!input)
    {
        input = root->FirstChildElement("animation")->FirstChildElement("source");
    }
    while (input)
    {
        if (strcmp(&source[1], input->Attribute("id")) == 0)
        {

            XMLElement* array = input->FirstChildElement("float_array");

            std::istringstream tokken (array->GetText());
            std::string value;

            while (tokken.good())
            {
                tokken >> value;
                time.push_back(atof(value.c_str()));
            }

            input = NULL;
        }
        else
        {
            input = input->NextSiblingElement("source");
        }
    }
}

void Magic3D::LoaderDAE::loadAnimationOutput(XMLElement* root, const char* source, std::vector<Matrix4>& matrix)
{
    if (root && source)
    {
        XMLElement* input = root->FirstChildElement("source");
        if (!input)
        {
            input = root->FirstChildElement("animation")->FirstChildElement("source");
        }
        while (input)
        {
            if (strcmp(&source[1], input->Attribute("id")) == 0)
            {

                XMLElement* array = input->FirstChildElement("float_array");

                int count = atoi(array->Attribute("count")) / 16;

                std::istringstream data (array->GetText());
                std::string value;

                for (int c = 0; c < count; c++)
                {
                    int size = 16;
                    float mat[size];
                    for (int m = 0; m < size; m++)
                    {
                        if (data.good())
                        {
                            data >> value;
                            mat[m] = atof(value.c_str());
                        }
                    }

                    Matrix4 bind;
                    Renderer::getInstance()->arrayToMatrix(mat, bind);

                    bind = transpose(bind);

                    matrix.push_back(bind);
                }


                input = NULL;
            }
            else
            {
                input = input->NextSiblingElement("source");
            }
        }
    }
}

void Magic3D::LoaderDAE::addMaterial(XMLElement* root, XMLElement* mesh, const char* material, MeshData* modelMeshData)
{
    XMLElement* mat = mesh->FirstChildElement("bind_material");

    Material* materialXML = NULL;
    bool doubleSide = false;

    if (mat)
    {
        mat = mat->FirstChildElement("technique_common")->FirstChildElement("instance_material");
    }

    while (mat)
    {
        if (strcmp(mat->Attribute("symbol"), material) == 0)
        {
            const char* target = mat->Attribute("target");
            XMLElement* libMat = root->FirstChildElement("library_materials")->FirstChildElement("material");

            while (libMat)
            {
                const char* matName = libMat->Attribute("name");
                if (!matName)
                {
                    matName = libMat->Attribute("id");
                }

                std::string lower = modelXML->getFileName();
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                lower = std::string(matName);
                lower = lower.substr(0, lower.rfind("-material"));                
                //modelXML->getFileName().substr(0, lower.rfind(".dae")) + "_" +
                bool created = false;
                materialXML = ResourceManager::getMaterials()->load(lower, created);

                if (strcmp(libMat->Attribute("id"), &target[1]) == 0)
                {
                    libMat = libMat->FirstChildElement("instance_effect");

                    if (libMat)
                    {
                        target = libMat->Attribute("url");

                        XMLElement* effect = root->FirstChildElement("library_effects")->FirstChildElement("effect");

                        while (effect)
                        {
                            if (strcmp(effect->Attribute("id"), &target[1]) == 0)
                            {

                                XMLElement* common = effect->FirstChildElement("profile_COMMON");
                                XMLElement* attribute = common ? common->FirstChildElement("technique") : NULL;
                                XMLElement* extra = common ? common->FirstChildElement("extra") : NULL;

                                while (created && attribute)
                                {
                                    target = attribute->Attribute("sid");
                                    if (strcmp(target, "common") == 0 || strcmp(target, "COMMON") == 0)
                                    {
                                        XMLElement* info = attribute->FirstChildElement("phong");

                                        if (!info)
                                        {
                                            info = attribute->FirstChildElement("lambert");
                                        }

                                        if (info)
                                        {
                                            /*attribute = info->FirstChildElement("ambient");
                                            if (attribute)
                                            {
                                                attribute = attribute->FirstChildElement("color");
                                                if (attribute)
                                                {
                                                    m->setColorAmbient(getColor(attribute->GetText()));
                                                }
                                            }*/
                                            materialXML->setColorAmbient(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

                                            attribute = info->FirstChildElement("diffuse");
                                            if (attribute)
                                            {
                                                XMLElement* tmp = attribute->FirstChildElement("color");
                                                if (tmp)
                                                {
                                                    materialXML->setColorDiffuse(getColor(tmp->GetText()));
                                                }

                                                tmp = attribute->FirstChildElement("texture");
                                                if (tmp)
                                                {
                                                    materialXML->setColorDiffuse(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
                                                    addTexture(root, effect, materialXML, 0, tmp->Attribute("texture"));
                                                }
                                            }

                                            attribute = info->FirstChildElement("emission");
                                            if (attribute)
                                            {
                                                attribute = attribute->FirstChildElement("color");
                                                if (attribute)
                                                {
                                                    materialXML->setColorEmission(getColor(attribute->GetText()));
                                                }
                                            }

                                            attribute = info->FirstChildElement("specular");
                                            if (attribute)
                                            {
                                                XMLElement* tmp = attribute->FirstChildElement("color");
                                                if (tmp)
                                                {
                                                    materialXML->setColorSpecular(getColor(tmp->GetText()));
                                                }

                                                tmp = attribute->FirstChildElement("texture");
                                                if (tmp)
                                                {
                                                    addTexture(root, effect, materialXML, 1, tmp->Attribute("texture"));
                                                }
                                            }

                                            attribute = info->FirstChildElement("shininess");
                                            if (attribute)
                                            {
                                                attribute = attribute->FirstChildElement("float");
                                                if (attribute)
                                                {
                                                    materialXML->setShininess(atof(attribute->GetText()));
                                                }
                                            }
                                        }

                                        attribute = NULL;
                                    }
                                    else
                                    {
                                        attribute = attribute->NextSiblingElement("technique");
                                    }
                                }

                                if (extra)
                                {
                                    extra = extra->FirstChildElement("technique");

                                    while (extra)
                                    {
                                        if (strcmp(extra->Attribute("profile"), "GOOGLEEARTH") == 0)
                                        {
                                            XMLElement* info = extra->FirstChildElement("show_double_sided");

                                            if (!info)
                                            {
                                                info = extra->FirstChildElement("double_sided");
                                            }

                                            if (info)
                                            {
                                                doubleSide = atoi(info->GetText());
                                            }

                                            extra = NULL;
                                        }
                                        else
                                        {
                                            extra  = extra->NextSiblingElement("technique");
                                        }
                                    }
                                }

                                effect = NULL;
                            }
                            else
                            {
                                effect = effect->NextSiblingElement("effect");
                            }
                        }
                    }

                    if (created && materialXML)
                    {
                        materialXML->save();
                    }
                    libMat = NULL;
                }
                else
                {
                    libMat = libMat->NextSiblingElement("material");
                }
            }

            mat = NULL;
        }
        else
        {
            mat = mat->NextSiblingElement("instance_material");
        }
    }

    if (!materialXML)
    {
        materialXML = ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL);
    }

    Mesh* meshXML = new Mesh(modelMeshData, doubleSide);
    meshXML->addMaterial(materialXML);
    modelXML->addMesh(meshXML);
    getModelInfo()->addMesh(modelMeshData, materialXML, doubleSide);
}

void Magic3D::LoaderDAE::addTexture(XMLElement* root, XMLElement* effect, Material* material, int texID, const char* texture)
{
    if (root && effect)
    {
        XMLElement* attribute = effect->FirstChildElement("profile_COMMON");
        XMLElement* param = attribute->FirstChildElement("newparam");

        while (param)
        {
            if (strcmp(param->Attribute("sid"), texture) == 0)
            {
                XMLElement* img = param->FirstChildElement("sampler2D")->FirstChildElement("source");

                param = attribute->FirstChildElement("newparam");

                while (param)
                {
                    if (strcmp(param->Attribute("sid"), img->GetText()) == 0)
                    {
                        const char* init_from = param->FirstChildElement("surface")->FirstChildElement("init_from")->GetText();

                        img = root->FirstChildElement("library_images")->FirstChildElement("image");

                        while (img)
                        {
                            if (strcmp(img->Attribute("id"), init_from) == 0)
                            {
                                init_from = img->FirstChildElement("init_from")->GetText();

                                const char* texName = img->Attribute("name");
                                std::string name;

                                if (!texName)
                                {
                                    texName = img->Attribute("id");
                                }
                                name = std::string(texName);
                                name = name.substr(0, name.rfind("_"));

                                bool created = false;
                                material->setTexture(texID, ResourceManager::getInstance()->addTexture(eTEXTURE_DIFFUSE, name, std::string(init_from), true, false, created));

                                img = NULL;
                            }
                            else
                            {
                                img = img->NextSiblingElement("image");
                            }
                        }

                        param = NULL;
                    }
                    else
                    {
                        param = param->NextSiblingElement("newparam");
                    }
                }

                param = NULL;
            }
            else
            {
                param = param->NextSiblingElement("newparam");
            }
        }
    }
}

void Magic3D::LoaderDAE::loadVertex(XMLElement* root)
{
    if (root)
    {
        XMLElement* vertices = root->FirstChildElement("vertices");
        while (vertices)
        {

            const char* semantic = "semantic";
            const char* source = "source";

            XMLElement* input = vertices->FirstChildElement("input");
            while (input)
            {
                if (strcmp("POSITION", input->Attribute(semantic)) == 0)
                {
                    loadVertexPosition(root, input->Attribute(source));
                }

                if (strcmp("NORMAL", input->Attribute(semantic)) == 0)
                {
                    loadNormal(root, input->Attribute(source));
                }

                input = input->NextSiblingElement("input");
            }

            vertices = vertices->NextSiblingElement("vertices");
        }
    }
}

void Magic3D::LoaderDAE::loadVertexPosition(XMLElement* root, const char* position)
{
    XMLElement* source = root->FirstChildElement("source");

    while (source)
    {
        if (strcmp(source->Attribute("id"), &position[1]) == 0)
        {
            XMLElement* data = source->FirstChildElement("float_array");
            if (data)
            {
                std::istringstream tokken (data->GetText());
                std::string value;

                while(tokken.good())
                {
                    tokken >> value;
                    float x = atof(value.c_str());
                    float y = 0.0f;
                    float z = 0.0f;

                    if (tokken.good())
                    {
                        tokken >> value;
                        y = atof(value.c_str());
                    }

                    if (tokken.good())
                    {
                        tokken >> value;
                        z = atof(value.c_str());
                    }

                    VertexSkin v;

                    /*if (upZ)
                    {
                        v.position = Vector3(x, z, -y);
                    }
                    else*/
                    {
                        v.position = Vector3(x, y, z);
                    }

                    this->position.push_back(v);
                }
            }
            source = NULL;
        }
        else
        {
            source = source->NextSiblingElement("source");
        }
    }
}

void Magic3D::LoaderDAE::loadNormal(XMLElement* root, const char* normal)
{
    XMLElement* source = root->FirstChildElement("source");

    while (source)
    {
        if (strcmp(source->Attribute("id"), &normal[1]) == 0)
        {
            XMLElement* data = source->FirstChildElement("float_array");
            if (data)
            {
                std::istringstream tokken (data->GetText());
                std::string value;

                while(tokken.good())
                {
                    tokken >> value;
                    float x = atof(value.c_str());
                    float y = 0.0f;
                    float z = 0.0f;

                    if (tokken.good())
                    {
                        tokken >> value;
                        y = atof(value.c_str());
                    }

                    if (tokken.good())
                    {
                        tokken >> value;
                        z = atof(value.c_str());
                    }

                    /*if (upZ)
                    {
                        this->normal.push_back(Vector3(x, z, -y));
                    }
                    else*/
                    {
                        this->normal.push_back(Vector3(x, y, z));
                    }
                }
            }
            source = NULL;
        }
        else
        {
            source = source->NextSiblingElement("source");
        }
    }
}

void Magic3D::LoaderDAE::loadUV(XMLElement* root, const char* uv, int uvIndex)
{
    XMLElement* source = root->FirstChildElement("source");

    while (source)
    {
        if (strcmp(source->Attribute("id"), &uv[1]) == 0)
        {
            XMLElement* data = source->FirstChildElement("float_array");
            if (data)
            {
                std::istringstream tokken (data->GetText());
                std::string value;

                XMLElement* acessor = source->FirstChildElement("technique_common")->FirstChildElement("accessor");
                int stride = atoi(acessor->Attribute("stride"));

                while(tokken.good())
                {
                    tokken >> value;
                    float u = atof(value.c_str());
                    float v = 0.0f;

                    if (tokken.good())
                    {
                        tokken >> value;
                        v = atof(value.c_str());
                    }

                    int s = 2;
                    while (s < stride)
                    {
                        if (tokken.good())
                        {
                            tokken >> value;
                        }
                        s++;
                    }

                    this->uv[uvIndex].push_back(Texture2D(u, v));
                }
            }
            source = NULL;
        }
        else
        {
            source = source->NextSiblingElement("source");
        }
    }
}

void Magic3D::LoaderDAE::loadColor(XMLElement* root, const char* color)
{
    XMLElement* source = root->FirstChildElement("source");

    while (source)
    {
        if (strcmp(source->Attribute("id"), &color[1]) == 0)
        {
            XMLElement* data = source->FirstChildElement("float_array");
            if (data)
            {
                std::istringstream tokken (data->GetText());
                std::string value;

                while(tokken.good())
                {
                    tokken >> value;
                    float r = atof(value.c_str());
                    float g = 0.0f;
                    float b = 0.0f;

                    if (tokken.good())
                    {
                        tokken >> value;
                        g = atof(value.c_str());
                    }

                    if (tokken.good())
                    {
                        tokken >> value;
                        b = atof(value.c_str());
                    }

                    this->color.push_back(ColorRGB(r, g, b));
                }
            }
            source = NULL;
        }
        else
        {
            source = source->NextSiblingElement("source");
        }
    }
}

Magic3D::XMLElement* Magic3D::LoaderDAE::save(XMLElement* root)
{
    if (root)
    {

    }
    return root;
}

Magic3D::XMLElement* Magic3D::LoaderDAE::load(XMLElement* root)
{
    if (root)
    {

    }
    return root;
}
