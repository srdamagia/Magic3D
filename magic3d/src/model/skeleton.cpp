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

#include <magic3d/model/animation.h>
#include <magic3d/object.h>

Magic3D::Skeleton::Skeleton(const Skeleton& skeleton, Object* object, const std::string name)
{
    this->object = object;
    animation = NULL;
    copyFrom(&skeleton);
    this->name = name;
}

void Magic3D::Skeleton::copyFrom(const Skeleton* skeleton)
{
    clearAll();

    if (skeleton)
    {
        this->name = skeleton->name;

        std::vector<Bone*>::const_iterator it_b = skeleton->bones.begin();
        while (it_b != skeleton->bones.end())
        {
            Bone* bone = *it_b++;
            this->bones.push_back(bone->spawn(this));
            this->skin.push_back(Matrix4::identity());
        }

        animation = skeleton->animation ? skeleton->animation->spawn(this) : NULL;
    }
}

Magic3D::Skeleton::Skeleton(Object* object, std::string name)
{
    this->object = object;
    this->name = name;
    animation = new Animation(this);
}

Magic3D::Skeleton::~Skeleton()
{
    clearAll();
}

Magic3D::Skeleton* Magic3D::Skeleton::spawn(Object* object) const
{
    return (new Skeleton(*this, object, getName()));
}

bool Magic3D::Skeleton::update()
{
    if (getAnimation())
    {
        getAnimation()->update();
    }

    bool result = false;
    std::vector<Bone*>::const_iterator it_b = bones.begin();
    while (it_b != bones.end())
    {
        Bone* bone = *it_b++;

        result = bone->update() || result;
    }

    return result;
}

const std::string& Magic3D::Skeleton::getName() const
{
    return name;
}

Magic3D::Object* Magic3D::Skeleton::getObject()
{
    return object;
}

Magic3D::Bone* Magic3D::Skeleton::getBone(int index)
{
    Bone* result = NULL;
    if (index >=0 && index < (int)bones.size())
    {
        result = bones[index];
    }

    return result;
}

Magic3D::Bone* Magic3D::Skeleton::getBoneByName(std::string name)
{
    Bone* result = NULL;
    std::vector<Bone*>::const_iterator it_b = bones.begin();
    while (it_b != bones.end())
    {
        Bone* bone = *it_b++;

        if (bone->getName().compare(name) == 0)
        {
            result = bone;
            break;
        }
    }

    return result;
}

Magic3D::Bone* Magic3D::Skeleton::addBone(std::string name, Matrix4 matrix, int parent)
{
    Bone* bone = new Bone(name, this, bones.size());
    bone->setMatrix(matrix);
    bone->setParentIndex(parent);

    add(bone);
    return bone;
}

void Magic3D::Skeleton::add(Bone* bone)
{
    bones.push_back(bone);
    skin.push_back(Matrix4::identity());
}

bool Magic3D::Skeleton::remove(std::string name)
{
    bool result = false;
    std::vector<Bone*>::const_iterator it_b = bones.begin();

    int index = 0;
    while (it_b != bones.end())
    {
        Bone* bone = *it_b++;

        if (bone->getName().compare(name) == 0)
        {
            bones.erase(bones.begin() + index);
            skin.erase(skin.begin() + index);
            break;
        }

        index++;
    }

    return result;
}

int Magic3D::Skeleton::getBoneCount()
{
    return bones.size();
}


void Magic3D::Skeleton::clear()
{
    while (!bones.empty()) {
        Bone* bone = *bones.begin();
        bones.erase(bones.begin());
        if (bone)
        {
            delete bone;
        }
    }

    skin.clear();
}

void Magic3D::Skeleton::clearAll()
{
    if (animation)
    {
        delete animation;
        animation = NULL;
    }

    clear();
}

std::vector<Magic3D::Bone*>* Magic3D::Skeleton::getBones()
{
    return &bones;
}

Magic3D::Animation* Magic3D::Skeleton::getAnimation()
{
    return animation;
}

Magic3D::XMLElement* Magic3D::Skeleton::save(XMLElement* root)
{
    if (root)
    {
        if (getAnimation())
        {
            getAnimation()->save(root);
        }

        XMLElement* skel = root->GetDocument()->NewElement(M3D_SKELETON_XML);
        skel->SetAttribute(M3D_SKELETON_XML_BONES, getBoneCount());
        root->LinkEndChild(skel);

        std::vector<Bone*>::const_iterator it_b = bones.begin();

        while (it_b != bones.end())
        {
            Bone* bone = *it_b++;

            bone->save(skel);
        }
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Skeleton::load(XMLElement* root)
{
    if (root)
    {
        if (getAnimation())
        {
            getAnimation()->load(root);
        }

        XMLElement* skel = root->FirstChildElement(M3D_SKELETON_XML);

        if (skel)
        {
            XMLElement* b = skel->FirstChildElement(M3D_BONE_XML);

            while (b)
            {
                int tmpIndex = loadInt(b, M3D_BONE_XML_INDEX);

                Bone* bone = getBone(tmpIndex);
                if (bone)
                {
                    bone->load(b);
                }
                b = b->NextSiblingElement();
            }
        }
    }

    return root;
}

//******************************************************************************
Magic3D::Bone::Bone(const Bone& bone, Skeleton* skeleton) : PhysicsObject(bone)
{
    this->matrix = bone.matrix;
    this->matrixInverse = bone.matrixInverse;
    this->world = bone.world;
    this->scale = bone.scale;

    this->skeleton = skeleton;

    this->childs = bone.childs;

    this->name = bone.name;

    this->index = bone.index;
    this->parent = bone.parent;

    this->selected = false;
    this->needTransform = bone.needTransform;
    this->animate = bone.animate;

    this->needReset = true;
}

Magic3D::Bone::Bone(std::string name, Skeleton* skeleton, int index) : PhysicsObject(eOBJECT_BONE)
{
    this->name = name;
    this->skeleton = skeleton;

    this->index = index;

    matrix        = Matrix4::identity();
    matrixInverse = Matrix4::identity();
    world         = Matrix4::identity();
    scale         = Vector3(1.0f, 1.0f, 1.0f);
    parent        = -1;
    selected      = false;
    needTransform = true;
    animate       = true;

    needReset     = false;
}

Magic3D::Bone::~Bone()
{

}

Magic3D::Bone* Magic3D::Bone::spawn(Skeleton* skeleton) const
{
    return (new Bone(*this, skeleton));
}

bool Magic3D::Bone::update()
{
    bool result = false;
    bool body = getRigidBody() != NULL;
    bool physics = Physics::getInstance()->isPlaying();

    updateTweens();
    if (needTransform || (getParent() && getParent()->isChanged()) || body)
    {
        result = true;

        if (physics && !needReset && !needTransform && body && !isAnimated())
        {
            btTransform transform;
            getRigidBody()->getMotionState()->getWorldTransform(transform);
            btQuaternion q = transform.getRotation();
            btVector3    v = transform.getOrigin();

            Quaternion rotation = Quaternion(q.getX(), q.getY(), q.getZ(), q.getW());
            Vector3 position = Vector3(v.getX(), v.getY(), v.getZ());

            matrix = getObjectAndParentMatrixInverse() * Matrix4(rotation, position) * Matrix4(Matrix3::identity(), -getShapePosition());
        }

        world = getMatrix();
        if (getParent())
        {
            world = getParent()->getMatrixFromParent() * world;
        }

        if (index < skeleton->getBoneCount())
        {
            skeleton->skin[index] = world * matrixInverse;
        }

        if ((needTransform || needReset) && body)
        {
            Physics::getInstance()->reset(this);
            needReset = false;
        }

        needTransform = false;

        std::vector<int>::const_iterator it_c = childs.begin();
        while (it_c != childs.end())
        {
            Bone* child = skeleton->getBone(*it_c++);
            child->setChanged(!physics || !child->getRigidBody() || child->isAnimated());
        }
    }

    return result;
}

const std::string& Magic3D::Bone::getName() const
{
    return name;
}

void Magic3D::Bone::setIndex(int index)
{
    this->index = index;
}

int Magic3D::Bone::getIndex()
{
    return index;
}

void Magic3D::Bone::setParentIndex(int parent)
{
    this->parent = parent;
}

int Magic3D::Bone::getParentIndex()
{
    return parent;
}

Magic3D::Bone* Magic3D::Bone::getParent()
{
    return skeleton->getBone(parent);
}

void Magic3D::Bone::setSelected(bool selected)
{
    this->selected = selected;
}

bool Magic3D::Bone::isSelected()
{
    return selected;
}

void Magic3D::Bone::setAnimated(bool animated)
{
    this->animate = animated;
}

bool Magic3D::Bone::isAnimated()
{
    return animate;
}

void Magic3D::Bone::setPosition(Vector3 position)
{
    Matrix4 tmp = getObjectAndParentMatrixInverse() * Matrix4(Matrix3::identity(), position);
    if (getRigidBody())
    {
        tmp *= Matrix4(Matrix3::identity(), -getShapePosition());
    }
    matrix.setTranslation(tmp.getTranslation());
    needTransform = true;
}

Magic3D::Vector3 Magic3D::Bone::getPosition()
{
    return getRigidBody() ? (getObjectAndParentMatrix() * Matrix4(Matrix3::identity(), getShapePosition())).getTranslation() : getObjectAndParentMatrix().getTranslation();
}

void Magic3D::Bone::setRotation(Quaternion rotation)
{
    Matrix4 tmp = getObjectAndParentMatrixInverse() * Matrix4(rotation, Vector3(0.0f, 0.0f, 0.0f));
    matrix.setUpper3x3(tmp.getUpper3x3());
    matrix = appendScale(matrix, getScale());

    needTransform = true;
}

Magic3D::Quaternion Magic3D::Bone::getRotation()
{
    Matrix3 mRot = getObjectAndParentMatrix().getUpper3x3();

    mRot.setCol0(normalize(mRot.getCol0()));
    mRot.setCol1(normalize(mRot.getCol1()));
    mRot.setCol2(normalize(mRot.getCol2()));

    return Quaternion(mRot);
}

void Magic3D::Bone::setScale(Vector3 scale)
{
    Vector3 tmpPos = matrix.getTranslation();
    Matrix3 mRot = matrix.getUpper3x3();

    mRot.setCol0(normalize(mRot.getCol0()));
    mRot.setCol1(normalize(mRot.getCol1()));
    mRot.setCol2(normalize(mRot.getCol2()));

    matrix = Matrix4(mRot, tmpPos);
    matrix = appendScale(matrix, scale);
    this->scale = scale;
    needTransform = true;
}

Magic3D::Vector3 Magic3D::Bone::getScale()
{
    return scale;
}

Magic3D::RENDER Magic3D::Bone::getRender()
{
    return Magic3D::eRENDER_3D;
}

void Magic3D::Bone::setMatrix(Matrix4 matrix)
{
    this->matrix = matrix;

    Matrix3 m = this->matrix.getUpper3x3();
    scale.setX(length(m.getCol0()));
    scale.setY(length(m.getCol1()));
    scale.setZ(length(m.getCol2()));

    needTransform = true;
}

const Magic3D::Matrix4& Magic3D::Bone::getMatrix()
{
    return matrix;
}

void Magic3D::Bone::setMatrixInverse(Matrix4 matrix)
{
    matrixInverse = matrix;
}

const Magic3D::Matrix4& Magic3D::Bone::getMatrixInverse()
{
    return matrixInverse;
}

Magic3D::Matrix4 Magic3D::Bone::getMatrixFromParent()
{
    return world;
}

Magic3D::Vector3 Magic3D::Bone::getPositionFromParent()
{
    return skeleton ? skeleton->skin[index].getTranslation() : Vector3(0.0f, 0.0f, 0.0f);
}

Magic3D::Quaternion Magic3D::Bone::getRotationFromParent()
{
    return skeleton ? Quaternion(skeleton->skin[index].getUpper3x3()) : Quaternion::identity();
}

Magic3D::Vector3 Magic3D::Bone::getScaleFromParent()
{
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

    if (skeleton)
    {
        Matrix3 m = skeleton->skin[index].getUpper3x3();
        scale.setX(length(m.getCol0()));
        scale.setY(length(m.getCol1()));
        scale.setZ(length(m.getCol2()));
    }

    return scale;
}

Magic3D::Matrix4 Magic3D::Bone::getMatrixSkin()
{
    return skeleton ? skeleton->skin[index] : Matrix4::identity();
}

Magic3D::Matrix4 Magic3D::Bone::getObjectMatrix()
{
    return getSkeletonObject() ? getSkeletonObject()->getMatrixFromParent() : Matrix4::identity();
}

Magic3D::Matrix4 Magic3D::Bone::getObjectAndParentMatrix()
{
    return getObjectMatrix() * getMatrixFromParent();
}

Magic3D::Matrix4 Magic3D::Bone::getObjectAndParentMatrixInverse()
{
    return inverse(getParent() ? getObjectMatrix() * getParent()->getMatrixFromParent() : getObjectMatrix());
}

Magic3D::Skeleton* Magic3D::Bone::getSkeleton()
{
    return skeleton;
}

Magic3D::Object* Magic3D::Bone::getSkeletonObject()
{
    return skeleton ? skeleton->getObject() : NULL;
}

std::vector<int>* Magic3D::Bone::getChilds()
{
    return &childs;
}

void Magic3D::Bone::setChanged(bool changed)
{
    this->needTransform = changed;
}

bool Magic3D::Bone::isChanged()
{
    return needTransform;
}

Magic3D::XMLElement* Magic3D::Bone::save(XMLElement* root)
{
    if (root)
    {
        XMLElement* bone = root->GetDocument()->NewElement(M3D_BONE_XML);
        root->LinkEndChild(bone);

        saveInt(bone,    M3D_BONE_XML_INDEX,    index);
        saveString(bone, M3D_BONE_XML_NAME,     name);
        saveBool(bone,   M3D_BONE_XML_ANIMATED, animate);

        PhysicsObject::save(bone);
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Bone::load(XMLElement* root)
{
    if (root)
    {
        std::string tmpName = loadString(root, M3D_BONE_XML_NAME);

        if (tmpName.compare(name) == 0)
        {
            animate = loadBool(root, M3D_BONE_XML_ANIMATED);
            PhysicsObject::load(root);
            needReset = true;
        }
    }

    return root;
}
