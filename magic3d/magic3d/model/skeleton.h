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

#ifndef MAGIC3D_SKELETON_H
#define MAGIC3D_SKELETON_H

#include <magic3d/physics/physics_object.h>

namespace Magic3D
{
#define M3D_SKELETON_DEFAULT_NAME "skeleton"

#define M3D_SKELETON_XML       "Skeleton"
#define M3D_SKELETON_XML_BONES "bones"
#define M3D_BONE_XML           "Bone"
#define M3D_BONE_XML_INDEX     "index"
#define M3D_BONE_XML_NAME      "name"
#define M3D_BONE_XML_ANIMATED  "animated"

class PhysicsObject;
class Animation;
class Bone;

class Skeleton : public Xml
{
private:
    Animation* animation;

    std::vector<Bone*> bones;

    std::string name;

    Object* object;

    void clearAll();

protected:
    Skeleton(const Skeleton& skeleton, Object* object, const std::string name);
public:
    std::vector<Matrix4> skin;

    Skeleton(Object* object, std::string name);
    virtual ~Skeleton();
    virtual Skeleton* spawn(Object* object) const;

    void copyFrom(const Skeleton* skeleton);

    virtual bool update();

    const std::string& getName() const;

    Object* getObject();

    Bone* getBone(int index);
    Bone* getBoneByName(std::string name);
    Bone* addBone(std::string name, Matrix4 matrix, int parent);
    void add(Bone* bone);
    bool remove(std::string name);
    int getBoneCount();
    void clear();

    std::vector<Bone*>* getBones();

    Animation* getAnimation();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class Bone: public PhysicsObject
{
private:
    Matrix4 matrix;
    Matrix4 matrixInverse;
    Matrix4 world;

    Vector3 scale;

    Skeleton* skeleton;

    std::vector<int> childs;

    std::string name;

    int index;
    int parent;

    bool selected;
    bool animate;
    bool needReset;

protected:
    Bone(const Bone& bone, Skeleton* skeleton);

public:
    Bone(std::string name, Skeleton* skeleton, int index);
    virtual ~Bone();
    Bone* spawnBone(Skeleton* skeleton) const;

    virtual bool update();

    const std::string& getName() const;

    void setIndex(int index);
    int getIndex();

    void setParentIndex(int parent);
    int getParentIndex();
    Bone* getParent();

    void setSelected(bool selected);
    bool isSelected();

    void setAnimated(bool animated);
    bool isAnimated();

    virtual void setPosition(Vector3 position);
    virtual Vector3 getPosition();
    virtual void setRotation(Quaternion rotation);
    virtual Quaternion getRotation();
    virtual void setScale(Vector3 scale);
    virtual Vector3 getScale();
    virtual RENDER getRender();

    void setMatrix(Matrix4 matrix);
    const Matrix4& getMatrix();
    void setMatrixInverse(Matrix4 matrix);
    const Matrix4& getMatrixInverse();
    Matrix4 getMatrixFromParent();
    Vector3 getPositionFromParent();
    Quaternion getRotationFromParent();
    Vector3 getScaleFromParent();
    Matrix4 getMatrixSkin();
    Matrix4 getObjectMatrix();
    Matrix4 getObjectAndParentMatrix();
    Matrix4 getObjectAndParentMatrixInverse();

    Skeleton* getSkeleton();
    Object* getSkeletonObject();
    std::vector<int>* getChilds();

    void setChanged(bool changed);
    bool isChanged();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};
}

#endif // MAGIC3D_SKELETON_H
