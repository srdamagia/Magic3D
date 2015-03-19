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

#ifndef MAGIC3D_LOADER_DAE_H
#define MAGIC3D_LOADER_DAE_H

#include <magic3d/model/model.h>
#include <magic3d/model/skeleton.h>

namespace Magic3D
{

struct VertexSkin
{
    VertexSkin() {position = Vector3(0.0f, 0.0f, 0.0f); for (int i = 0; i < M3D_MULTI_SKIN_COUNT; i++) {weight[i] = 0.0f; bone[i] = -1;}}
    int bone[M3D_MULTI_SKIN_COUNT];
    Vector3 position;
    float weight[M3D_MULTI_SKIN_COUNT];
};

class LoaderDAE : public Loader
{
private:
    std::map<std::string, int> boneList;

    std::vector<VertexSkin> position;
    std::vector<Vector3> normal;
    std::vector<Texture2D> uv[M3D_MULTI_TEXTURE_COUNT];
    std::vector<ColorRGB> color;
    std::vector<Bone*> bone;
    std::vector<std::string> boneName;
    std::vector<float> weight;

    std::list<XMLElement*> nodesXML;

    XMLDocument* doc;
    XMLElement* rootXML;

    Model* modelXML;

    Matrix4 matrixUp;

    bool upZ;
    bool createdSkeleton;
    bool loading;
    bool firstObjectXML;

    void loadNode(XMLElement* root, XMLElement* node);
    ColorRGBA getColor(const char* color);
    void setElement(Vector3& vector, Vector4& axis);

    void addMesh(XMLElement* root, XMLElement* skin, XMLElement* mesh, const char* meshName, Matrix4& matrix);
    bool addSkeleton(XMLElement* root, const char* rootBone, bool compareRoot);
    void loadBoneNames(XMLElement* root);
    void addBone(XMLElement* root, Skeleton* skeleton, int parent, const Matrix4& matrixParent);
    void addMaterial(XMLElement* root, XMLElement* mesh, const char* material, MeshData* modelMeshData);
    void addTexture(XMLElement* root, XMLElement* effect, Material* material, int texID, const char* texture);

    void loadBindPose(XMLElement* root);
    void loadWeight(XMLElement* root);
    void loadAnimations(XMLElement* root, Skeleton* skeleton);
    void loadAnimationInput(XMLElement* root, const char* source, std::vector<float>& time);
    void loadAnimationOutput(XMLElement* root, const char* source, std::vector<Matrix4>& matrix);

    void loadVertex(XMLElement* root);

    void loadVertexPosition(XMLElement* root, const char* position);

    void loadNormal(XMLElement* root, const char* normal);
    void loadUV(XMLElement* root, const char* uv, int uvIndex);
    void loadColor(XMLElement* root, const char* color);

public:
    LoaderDAE();
    virtual ~LoaderDAE();

    virtual bool load(Model* model);
    virtual bool load();
    virtual bool isLoading();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};
}

#endif // MAGIC3D_LOADER_DAE_H
