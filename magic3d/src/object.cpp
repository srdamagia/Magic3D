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

Magic3D::Object::Object(const Object& object, std::string name) : PhysicsObject(object)
{
    this->parent = object.parent;
    this->parentBone = object.parentBone;
    this->parentPosition = object.parentPosition;
    this->parentRotation = object.parentRotation;
    this->parentScale = object.parentScale;

    this->layer = NULL;
    this->octree = NULL;

    this->name = name;
    this->script = object.script;
    this->scripted = false;

    this->matrix = object.matrix;

    this->box = object.box;

    this->rotation = object.rotation;

    this->position = object.position;
    this->scale = object.scale;
    this->axis = object.axis;
    this->euler = object.euler;

    this->upward = object.upward;
    this->forward = object.forward;
    this->rightward = object.rightward;

    this->angle = object.angle;

    this->zOrderFactor = object.zOrderFactor;

    this->collisionMesh = object.collisionMesh;

    this->flag = object.flag;
    this->billboard = object.billboard;

    this->enabled = object.enabled;
    this->visible = object.visible;
    this->frustum = object.frustum;
    this->frustumEffect = object.frustumEffect;
    this->pick = object.pick;
    this->zOrder = object.zOrder;
    this->shadowed = object.shadowed;
    this->reflective = object.reflective;
    this->glowed = object.glowed;

    this->AI = object.AI ? object.AI->spawn() : NULL;

    this->selected = false;
    this->grid = false;
    this->uniqueUpdate = false;

    std::vector<Mesh*>::const_iterator it_m = object.meshes.begin();

    while (it_m != object.meshes.end())
    {
        Mesh* mesh = *it_m++;
        this->meshes.push_back(mesh->spawn());
    }

    if (object.scripted)
    {
        setScripted(true);
    }

}

Magic3D::Object::Object(OBJECT type, RENDER render, std::string name) : PhysicsObject(type, render)
{
    init(name);
}

void Magic3D::Object::init(std::string name)
{
    this->name = name;
    this->script = name;

    this->layer = NULL;
    this->octree = NULL;

    flag             = 0;
    billboard        = eBILLBOARD_NONE;
    parent           = NULL;
    parentBone       = NULL;
    parentPosition   = true;
    parentRotation   = true;
    parentScale      = true;
    collisionMesh    = -1;

    enabled          = true;
    visible          = true;
    shadowed         = false;
    reflective       = false;
    glowed           = false;
    selected         = false;
    grid             = false;
    frustum          = false;
    frustumEffect    = false;
    pick             = false;
    zOrder           = false;
    zOrderFactor     = 0.0f;

    position         = Vector3(0.0f, 0.0f, 0.0f);
    scale            = Vector3(1.0f, 1.0f, 1.0f);
    euler            = Vector3(0.0f, 0.0f, 0.0f);

    upward           = Vector3::yAxis();
    forward          = Vector3::zAxis();
    rightward        = Vector3::xAxis();

    box              = Box(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

    rotation         = Quaternion::identity();
    matrix           = Matrix4::identity();

    uniqueUpdate     = false;
    scripted         = false;

    AI               = NULL;
}

Magic3D::Object::~Object()
{
    std::vector<Object*>::const_iterator it_o = children.begin();
    while (it_o != children.end())
    {
        (*it_o++)->setParent(NULL, false);
    }
    children.clear();

    std::vector<ObjectInstance*>::const_iterator it_i = instances.begin();
    while (it_i != instances.end())
    {
        (*it_i++)->setInstance(NULL, false);
    }
    instances.clear();

    if (getParent())
    {
        setParent(NULL);
    }

    if (Script::getInstance())
    {
        if (isScripted())
        {
            setScripted(false);
        }
    }

    if (AI)
    {
        delete AI;
        AI = NULL;
    }

    clearMeshes();
}

Magic3D::Object* Magic3D::Object::spawn(std::string name) const
{
    return new Object(*this, name);
}

Magic3D::Object* Magic3D::Object::spawn(std::string name, std::string layerName, bool keepChildrenLayer)
{
    Object* result = NULL;
    Layer* layer = Scene::getInstance()->getLayer(layerName);

    if (!layer)
    {
        layer = Scene::getInstance()->getLayer(M3D_MAIN_LAYER_NAME);
    }

    result = getObject(name);
    if (!result && layer)
    {
        bool allchildren = true;

        std::vector<Object*>::const_iterator it_o = children.begin();
        while (it_o != children.end())
        {
            Object* child = *it_o++;

            std::string childName = child->getName();
            unsigned int separator = childName.rfind('.');
            childName = name + "." + (separator == childName.npos ? childName : childName.substr(separator + 1));

            if (getObject(childName))
            {
                allchildren = false;
                break;
            }
        }

        if (allchildren)
        {
            result = spawn(name);
            ResourceManager::getObjects()->add(result);
            Scene::getInstance()->addObject(layer, result);

            it_o = children.begin();
            while (it_o != children.end())
            {
                Object* child = *it_o++;

                std::string childName = child->getName();
                unsigned int separator = childName.rfind('.');
                childName = name + "." + (separator == childName.npos ? childName : childName.substr(separator + 1));

                Object* spawned = child->spawn(childName, keepChildrenLayer && child->getLayer() ? child->getLayer()->getName() : layerName, keepChildrenLayer);
                if (spawned != child)
                {
                    spawned->setParent(result);
                    if (child->getParentBone() && result->getType() == eOBJECT_MODEL)
                    {
                        Bone* bone = static_cast<Model*>(result)->getSkeleton()->getBoneByName(child->getParentBone()->getName());
                        spawned->setParentBone(bone);
                    }
                }
            }
        }
        else
        {
            result = this;
        }
    }

    if (result && getRigidBody())
    {
        Physics::getInstance()->add(result);
    }

    return result;
}

void Magic3D::Object::killChildren()
{
    //std::vector<Object*>::const_iterator it_o = children.begin();
    //while (it_o != children.end())
    while (!children.empty())
    {
        Object* child = *children.begin();
        child->killChildren();
        Scene::getInstance()->removeObject(child->getLayer(), child);
        ResourceManager::getObjects()->remove(child->name);
    }
}

bool Magic3D::Object::update()
{
    if (Scene::getInstance()->getUniqueUpdateFlag() != uniqueUpdate)
    {
        uniqueUpdate = Scene::getInstance()->getUniqueUpdateFlag();

        bool needTransformOld = needTransform;

        if (isEnabled())
        {
            if (isScripted())
            {
                std::string function_AI("AI");
                Script::getInstance()->execute(function_AI, 0, getName());
            }

            if (AI)
            {
                AI->AI();
            }

            updateTweens();
        }

        if (needTransform || (getRigidBody() && Physics::getInstance()->isPlaying() &&  getRigidBody()->getActivationState() != ISLAND_SLEEPING) || (getParent() && getParent()->isInFrustum()))
        {
            if (needTransform && !needTransformOld)
            {
                resetPhysics();
            }
            needTransform = false;
            updateMatrix();
        }
    }

    return true;
}

bool Magic3D::Object::updateMeshes()
{
    bool result = true;
    bool update = false;

    std::vector<Mesh*>::const_iterator it_m = meshes.begin();
    while (it_m != meshes.end())
    {
        Mesh* mesh = *it_m++;

        update = mesh->update(this);
        result = result && update;
    }

    return result;
}

const std::string& Magic3D::Object::getName()
{
    return name;
}

bool Magic3D::Object::setName(std::string name)
{
    bool result = ResourceManager::getObjects()->rename(this->name, name);

    if (result)
    {
        if (this->name.compare(script) == 0)
        {
            script = name;
        }
        this->name = name;
    }

    return result;
}

const std::string& Magic3D::Object::getScript()
{
    return script;
}

void Magic3D::Object::setScript(std::string script)
{
    this->script = script;

    setScripted(true);
}

void Magic3D::Object::setZOrder(bool zOrder)
{
    this->zOrder = zOrder;
}

bool Magic3D::Object::isZOrder()
{
    return zOrder;
}

void Magic3D::Object::setZOrderFactor(float factor)
{
    zOrderFactor = factor;
}

float Magic3D::Object::getZOrderFactor()
{
    return zOrderFactor;
}

void Magic3D::Object::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

bool Magic3D::Object::isEnabled()
{
    return enabled;
}

void Magic3D::Object::setVisible(bool visible)
{
    this->visible = visible;
}

bool Magic3D::Object::isVisible()
{
    return visible;
}

void Magic3D::Object::setShadowed(bool shadowed)
{
    this->shadowed = shadowed;
}

bool Magic3D::Object::isShadowed()
{
    return shadowed;
}

void Magic3D::Object::setReflective(bool reflective)
{
    this->reflective = reflective;
}

bool Magic3D::Object::isReflective()
{
    return reflective;
}

void Magic3D::Object::setGlowed(bool glowed)
{
    this->glowed = glowed;
}

bool Magic3D::Object::isGlowed()
{
    return glowed;
}

void Magic3D::Object::setInFrustum(bool frustum)
{
    this->frustum = frustum;
}

bool Magic3D::Object::isInFrustum()
{
    return frustum;
}

bool Magic3D::Object::isInParentFrustum()
{
    bool result = frustum;
    if (!result && getParent())
    {
        result = getParent()->isInParentFrustum();
    }
    return result;
}

void Magic3D::Object::setInEffectFrustum(bool frustum)
{
    this->frustumEffect = frustum;
}

bool Magic3D::Object::isInEffectFrustum()
{
    return frustumEffect;
}

void Magic3D::Object::setPickable(bool pick)
{
    this->pick = pick;
}

bool Magic3D::Object::isPickable()
{
    return pick;
}

const std::vector<Magic3D::Object*>* Magic3D::Object::getChildren()
{
    return &children;
}

void Magic3D::Object::removeChild(Object* child)
{
    std::vector<Object*>::iterator it_o = children.begin();
    while (it_o != children.end())
    {
        Object* c = *it_o;
        if (c == child)
        {
            children.erase(it_o);
            break;
        }

        it_o++;
    }
}

void Magic3D::Object::addInstance(ObjectInstance* instance)
{
    instances.push_back(instance);
}

void Magic3D::Object::removeInstance(ObjectInstance* instance)
{
    std::vector<ObjectInstance*>::iterator it_o = instances.begin();
    while (it_o != instances.end())
    {
        ObjectInstance* c = *it_o;
        if (c == instance)
        {
            instances.erase(it_o);
            break;
        }

        it_o++;
    }
}

void Magic3D::Object::setParent(Object* parent)
{
    setParent(parent, true);
}

void Magic3D::Object::setParent(Object* parent, bool removeChild)
{
    if ((parent && parent != this && parent->getParent() != this) || !parent)
    {
        if (this->parent && removeChild)
        {
            this->parent->removeChild(this);
        }
        this->parent = parent;
        if (this->parent)
        {
            this->parent->children.push_back(this);
        }
        needTransform = true;
    }
}

Magic3D::Object* Magic3D::Object::getParent()
{
    return parent;
}

void Magic3D::Object::setParentBone(Bone* bone)
{
    parentBone = bone;
}

Magic3D::Bone* Magic3D::Object::getParentBone()
{
    return parentBone;
}

void Magic3D::Object::setParentRelation(bool position, bool rotation, bool scale)
{
    parentPosition = position;
    parentRotation = rotation;
    parentScale = scale;
}

bool Magic3D::Object::isParentPosition()
{
    return parentPosition;
}

bool Magic3D::Object::isParentRotation()
{
    return parentRotation;
}

bool Magic3D::Object::isParentScale()
{
    return parentScale;
}

void Magic3D::Object::setLayer(Layer* layer)
{
    this->layer = layer;
}

Magic3D::Layer* Magic3D::Object::getLayer()
{
    return layer;
}

void Magic3D::Object::setOctree(Octree* octree)
{
    this->octree = octree;
}

Magic3D::Octree* Magic3D::Object::getOctree()
{
    return octree;
}

void Magic3D::Object::setPosition(Vector3 position)
{
    this->position = position;
    needTransform = true;
}

Magic3D::Vector3 Magic3D::Object::getPosition()
{
    return position;
}

Magic3D::Vector3 Magic3D::Object::getPositionFromParent()
{
    Vector3 position = getPosition();
    if (getParent() && parentPosition)
    {
        Matrix4 m = getParent()->getMatrixFromParent();

        if (getParentBone())
        {
            m = m * getParentBone()->getMatrixFromParent();
        }
        position = (m * Vector4(position, 1.0f)).getXYZ();
    }

    return position;
}

void Magic3D::Object::setScale(Vector3 scale)
{
    this->scale = scale;
    needTransform = true;
}

Magic3D::Vector3 Magic3D::Object::getScale()
{
    return scale;
}

Magic3D::Vector3 Magic3D::Object::getScaleFromParent()
{
    Vector3 scale = getScale();
    if (getParent() && parentScale)
    {
        Vector3 v = getParent()->getScaleFromParent();

        if (getParentBone())
        {
            v = mulPerElem(v, getParentBone()->getScaleFromParent());
        }
        scale = mulPerElem(v, scale);
    }

    return scale;
}

void Magic3D::Object::setRotationEuler(Vector3 rotation)
{
    euler = rotation;
    this->rotation = Math::quaternion(euler);

    needUpdateEuler = false;
    needTransform = true;
}

void Magic3D::Object::setRotationEuler(float angle, Vector3 axis)
{
    this->rotation = Quaternion::rotation(angle, axis);
    needUpdateEuler = true;
    needTransform = true;
}

const Magic3D::Vector3& Magic3D::Object::getRotationEuler()
{
    if (needUpdateEuler)
    {
        needUpdateEuler = false;
        euler = Math::euler(rotation);
    }
    return euler;
}

void Magic3D::Object::setRotation(Quaternion rotation)
{
    this->rotation = rotation;
    needUpdateEuler = true;
    needTransform = true;
}

Magic3D::Quaternion Magic3D::Object::getRotation()
{
    return rotation;
}

Magic3D::Quaternion Magic3D::Object::getRotationFromParent()
{
    Quaternion rot = getRotation();
    if (getParent() && parentRotation)
    {
        Quaternion q = getParent()->getRotationFromParent();

        if (getParentBone())
        {
            q = q * getParentBone()->getRotationFromParent();
        }
        rot = q * rot;
    }
    return rot;
}

Magic3D::Vector3 Magic3D::Object::getRotationEuler(float& angle)
{
    updateAxisAngleFromRotation();
    angle = this->angle;
    return axis;
}

void Magic3D::Object::lookAt(Vector3 position, Vector3 up, float factor)
{
    Matrix4 m4EyeFrame;
    Vector3 v3X, v3Y, v3Z;
    Vector3 eyePos = getPositionFromParent();
    v3Y = normalize( up );
    v3Z = normalize( ( position - eyePos ) );
    v3X = normalize( cross( v3Y, v3Z ) );
    v3Y = cross( v3Z, v3X );
    m4EyeFrame = Matrix4( Vector4( v3X ), Vector4( v3Y ), Vector4( v3Z ), Vector4( eyePos ) );

    Quaternion parent = getParent() ? Math::inverse(getParent()->getRotationFromParent()) : Quaternion::identity();
    Quaternion m = slerp(factor, getRotationFromParent(), Quaternion(m4EyeFrame.getUpper3x3()));

    setRotation(parent * m);
}

void Magic3D::Object::rotate(Vector3 rotation)
{
    setRotation(getRotation() * Math::quaternion(rotation));
}

const Magic3D::Vector3& Magic3D::Object::getDirectionUp()
{
    return upward;
}

Magic3D::Vector3 Magic3D::Object::getDirectionDown()
{
    return -upward;
}

const Magic3D::Vector3& Magic3D::Object::getDirectionFront()
{
    return forward;
}

Magic3D::Vector3 Magic3D::Object::getDirectionBack()
{
    return -forward;
}

Magic3D::Vector3 Magic3D::Object::getDirectionLeft()
{
    return -rightward;
}

const Magic3D::Vector3& Magic3D::Object::getDirectionRight()
{
    return rightward;
}

void Magic3D::Object::addMesh(Mesh* mesh)
{
    if (mesh)
    {
        meshes.push_back(mesh);
    }
}

Magic3D::Mesh* Magic3D::Object::getMesh(std::string name)
{
    Mesh* result = NULL;
    std::vector<Mesh*>::const_iterator it_m = meshes.begin();

    while (it_m != meshes.end())
    {
        Mesh* mesh = *it_m++;
        if (mesh->getData() && mesh->getData()->getName().compare(name) == 0)
        {
            result = mesh;
            break;
        }
    }

    return result;
}

int Magic3D::Object::getMeshIndex(std::string name)
{
    int result = -1;
    std::vector<Mesh*>::const_iterator it_m = meshes.begin();

    int index = 0;
    while (it_m != meshes.end())
    {
        Mesh* mesh = *it_m++;
        if (mesh->getData() && mesh->getData()->getName().compare(name) == 0)
        {
            result = index;
            break;
        }

        index++;
    }

    return result;
}

void Magic3D::Object::clearMeshes()
{
    while (!meshes.empty()) {
        Mesh* mesh = *meshes.begin();
        meshes.erase(meshes.begin());
        if (mesh)
        {
            delete mesh;
        }
    }
}

std::vector<Magic3D::Mesh*>* Magic3D::Object::getMeshes()
{
    return &meshes;
}

void Magic3D::Object::updateAxisAngleFromRotation()
{
    float temp_angle; // temp angle
    float scale;      // temp vars

    temp_angle = std::acos(rotation.getW());

    scale = (float)sqrt(Math::sqr(rotation.getX()) + Math::sqr(rotation.getY()) + Math::sqr(rotation.getZ()));

    if (Math::eq(0.0f, scale)) // angle is 0 or 360 so just simply set axis to 0,0,1 with angle 0
    {
        angle = 0.0f;
        axis.setX(0.0f);
        axis.setY(0.0f);
        axis.setZ(1.0f); // any axis will do
    }
    else
    {
        angle = (float)(temp_angle * 2.0f); // angle in degrees
        Vector3 vector = Vector3(rotation.getX() / scale, rotation.getY() / scale, rotation.getZ() / scale);
        axis = normalize(vector);
    }
}

const Magic3D::Box& Magic3D::Object::getBoundingBox()
{
    return box;
}

Magic3D::Box Magic3D::Object::getBoundingBoxAlignedAxis()
{
    Box b = getBoundingBox();
    Vector3 min = b.corners[0];
    Vector3 max = b.corners[1];

    Matrix4 matrix = getMatrixFromParent();
    Matrix3 dir = matrix.getUpper3x3();
    Vector3 position = matrix.getTranslation();

    Vector3 dimensions = Vector3(b.getWidth(), b.getHeight(), b.getDepth()) * 0.5f;

    Vector3 corners[6];
    corners[0] = position + dir.getCol0() * dimensions.getX();
    corners[1] = position + dir.getCol1() * dimensions.getY();
    corners[2] = position + dir.getCol2() * dimensions.getZ();

    corners[3] = position + dir.getCol0() * -dimensions.getX();
    corners[4] = position + dir.getCol1() * -dimensions.getY();
    corners[5] = position + dir.getCol2() * -dimensions.getZ();

    min = corners[0];
    max = corners[0];

    for (int i = 1; i < 6; i++)
    {
        min.setX(Math::min(min.getX(), corners[i].getX()));
        min.setY(Math::min(min.getY(), corners[i].getY()));
        min.setZ(Math::min(min.getZ(), corners[i].getZ()));

        max.setX(Math::max(max.getX(), corners[i].getX()));
        max.setY(Math::max(max.getY(), corners[i].getY()));
        max.setZ(Math::max(max.getZ(), corners[i].getZ()));
    }

    b.corners[0] = min;
    b.corners[1] = max;

    return b;
}

void Magic3D::Object::updateBoundingBox()
{
    updateBoundingBox(true);
}

void Magic3D::Object::updateBoundingBox(bool updateMesh)
{
    Vector3 min(0.0f, 0.0f, 0.0f);
    Vector3 max(0.0f, 0.0f, 0.0f);

    bool first = true;
    std::vector<Mesh*>::const_iterator it_m = meshes.begin();
    while (it_m != meshes.end())
    {
        Mesh* mesh = *it_m++;

        if (mesh && mesh->getData())
        {
            if (!mesh->isVisible())
            {
                continue;
            }

            if (updateMesh)
            {
                mesh->update(this);
            }

            if (first)
            {
                min = mesh->getData()->getBoundingBox().corners[0];
                max = mesh->getData()->getBoundingBox().corners[1];
                first = false;
            }
            else
            {
                min = Math::min(min, mesh->getData()->getBoundingBox().corners[0]);
                max = Math::max(max, mesh->getData()->getBoundingBox().corners[1]);
            }
        }
    }

    box.corners[0] = min;
    box.corners[1] = max;
}

float Magic3D::Object::getMaxSizeFromParent()
{
    Box box = getBoundingBox();
    float maxScale = Math::max(Math::max(scale.getX(), scale.getY()), scale.getZ());

    float result = box.getMaxSize() * maxScale;

    if (getParent())
    {
        result += getParent()->getMaxSizeFromParent();
    }

    return result;
}

void Magic3D::Object::updateMatrix()
{
    if (getRigidBody())
    {
        btTransform transform;
        getRigidBody()->getMotionState()->getWorldTransform(transform);
        btQuaternion q = transform.getRotation();
        btVector3    v = transform.getOrigin();

        if (getShapeRotation().getX() != 0.0f || getShapeRotation().getY() != 0.0f || getShapeRotation().getZ() != 0.0f)
        {
            rotation = Quaternion(q.getX(), q.getY(), getRender() == eRENDER_2D ? -q.getZ() : q.getZ(), q.getW());
        }
        if (getRender() == eRENDER_2D)
        {
            position = Vector3((v.getX() + 1.0f - 1.0f), -(v.getY() + 1.0f - 1.0f), 0.0f);
        }
        else
        {
            position = Vector3(v.getX(), v.getY(), v.getZ());
        }

        matrix = Matrix4(rotation, position) * Matrix4(Matrix3::identity(), -getShapePosition());
    }
    else
    {
        matrix = Matrix4(rotation, position);
    }

    matrix = appendScale(matrix, scale);

    upward    = matrix.getCol1().getXYZ();
    forward   = matrix.getCol2().getXYZ();
    rightward = cross(forward, upward);

    std::vector<Object*>::const_iterator it_o = children.begin();
    while (it_o != children.end())
    {
        (*it_o++)->needTransform = true;
    }

    needUpdateOctree = true;
}

void Magic3D::Object::setMatrix(Matrix4 matrix)
{
    this->matrix = matrix;
}

void Magic3D::Object::applyMatrix(Matrix4 matrix)
{
    Vector3 tmpPos = matrix.getTranslation();
    Matrix3 mRot = matrix.getUpper3x3();
    Vector3 tmpScale  = Vector3(length(mRot.getCol0()), length(mRot.getCol1()), length(mRot.getCol2()));

    mRot.setCol0(normalize(mRot.getCol0()));
    mRot.setCol1(normalize(mRot.getCol1()));
    mRot.setCol2(normalize(mRot.getCol2()));

    setPosition(tmpPos);
    setRotation(Quaternion(mRot));
    setScale(tmpScale);
}

const Magic3D::Matrix4& Magic3D::Object::getMatrix()
{
    return matrix;
}

Magic3D::Matrix4 Magic3D::Object::getMatrixFromParent()
{
    Matrix4 m = getMatrix();

    if (getParent())
    {
        m = getParent()->getMatrixFromParent();

        if (getParentBone())
        {
            m = m * getParentBone()->getMatrixFromParent();
        }

        if (isParentPosition() && isParentRotation() && isParentScale())
        {
            m = m * getMatrix();
        }
        else
        {
            Vector3 tmpPos = m.getTranslation();
            Matrix3 mRot = m.getUpper3x3();
            Vector3 tmpScale  = Vector3(length(mRot.getCol0()), length(mRot.getCol1()), length(mRot.getCol2()));

            mRot.setCol0(normalize(mRot.getCol0()));
            mRot.setCol1(normalize(mRot.getCol1()));
            mRot.setCol2(normalize(mRot.getCol2()));

            m = Matrix4::identity();

            if (isParentRotation())
            {
                m = m * Matrix4(mRot, Vector3(0.0f, 0.0f, 0.0f));
            }

            if (isParentPosition())
            {
                m.setTranslation(tmpPos);
            }

            if (isParentScale())
            {
                m = appendScale(m, tmpScale); // Faster than creating and multiplying a scale transformation matrix.
            }

            m = m * getMatrix();
        }
    }

    if (billboard != eBILLBOARD_NONE)
    {
        Camera* camera = Renderer::getInstance()->getCurrentViewPort()->getPerspective();
        Matrix3 view = camera->getView().getUpper3x3();
        Matrix3 bb = Matrix3::identity();

        switch (billboard)
        {
            case eBILLBOARD_HORIZONTAL:
            {
                Vector3 v1 = Vector3(1.0f, 0.0f, 0.0f);
                Vector3 v2 = view.getCol0();

                float angle = Math::angle(v1, v2, Vector3(0.0f, 1.0f, 0.0f));

                bb = Matrix3::rotationY(angle);
                break;
            }
            case eBILLBOARD_VERTICAL:
            {
                Vector3 v1 = Vector3(0.0f, 1.0f, 0.0f);
                Vector3 v2 = view.getCol1();

                float angle = Math::angle(v1, v2, Vector3(1.0f, 0.0f, 0.0f));

                bb = Matrix3::rotationX(angle);
                break;
            }
            default:
            {
                bb = view;
                break;
            }
        }

        m = m * Matrix4(inverse(bb), Vector3(0.0f, 0.0f, 0.0f));
    }

    return m;
}

Magic3D::Matrix4 Magic3D::Object::getCompleteMatrixFromParent()
{
    Matrix4 m = getMatrix();

    if (getParent())
    {
        m = getParent()->getCompleteMatrixFromParent();

        if (getParentBone())
        {
            m = m * getParentBone()->getMatrixFromParent();
        }

        m = m * getMatrix();
    }

    return m;
}

void Magic3D::Object::setSelected(bool selected)
{
    this->selected = selected;
}

bool Magic3D::Object::isSelected()
{
    return selected;
}

void Magic3D::Object::setShowGrid(bool grid)
{
    this->grid = grid;
}

bool Magic3D::Object::isShowingGrid()
{
    return grid;
}

void Magic3D::Object::setFlag(int flag)
{
    this->flag = flag;
}

int Magic3D::Object::getFlag()
{
    return flag;
}

void Magic3D::Object::setBillboard(BILLBOARD billboard)
{
    this->billboard = billboard;
}

Magic3D::BILLBOARD Magic3D::Object::getBillboard()
{
    return billboard;
}

void Magic3D::Object::setCollisionMeshIndex(int mesh)
{
    this->collisionMesh = mesh;
}

int Magic3D::Object::getCollisionMeshIndex()
{
    return collisionMesh;
}

Magic3D::MeshData* Magic3D::Object::getCollisionMesh()
{
    MeshData* result = NULL;
    if (collisionMesh >= 0 && collisionMesh < (int)meshes.size())
    {
        result = meshes[collisionMesh]->getData();
    }

    return result;
}

void Magic3D::Object::setAI(ObjectAI* AI)
{
    if (this->AI)
    {
        delete this->AI;
    }

    this->AI = AI;
}

Magic3D::ObjectAI* Magic3D::Object::getAI()
{
    return AI;
}

void Magic3D::Object::setScripted(bool scripted)
{
    bool hasScript = this->scripted;
    this->scripted = scripted;

    if (this->scripted)
    {
        Script::getInstance()->add(this);
    }
    else if (hasScript)
    {
        Script::getInstance()->remove(this);
    }
}

bool Magic3D::Object::isScripted()
{
    return scripted;
}

void Magic3D::Object::move(Vector3 factor)
{
    setPosition(getPosition() + factor);
}

void Magic3D::Object::size(Vector3 scale)
{
    setScale(getScale() + scale);
}



void Magic3D::Object::setMeshTexture(std::string mesh, std::string texture)
{
    Mesh* m = getMesh(mesh);
    if (m)
    {
        m->getMaterials()->at(0)->setTexture(0, ResourceManager::getTextures()->get(texture));
    }
}

std::string Magic3D::Object::getMeshTexture(std::string mesh)
{
    std::string result = "";
    Mesh* m = getMesh(mesh);
    if (m)
    {
        result = m->getMaterials()->at(0)->getTexture(0)->getName();
    }
    return result;
}

void Magic3D::Object::setMeshColor(std::string mesh, ColorRGBA color)
{
    Mesh* m = getMesh(mesh);
    if (m)
    {
        m->getMaterials()->at(0)->setColorDiffuse(color);
    }
}

Magic3D::ColorRGBA Magic3D::Object::getMeshColor(std::string mesh)
{
    ColorRGBA result(0.0f, 0.0f, 0.0f, 0.0f);
    Mesh* m = getMesh(mesh);
    if (m)
    {
        result = m->getMaterials()->at(0)->getColorDiffuse();
    }
    return result;
}

void Magic3D::Object::setMeshVisible(std::string mesh, bool visible)
{
    Mesh* m = getMesh(mesh);
    if (m)
    {
        m->setVisible(visible);
    }
}

bool Magic3D::Object::isMeshVisible(std::string mesh)
{
    bool result = false;
    Mesh* m = getMesh(mesh);
    if (m)
    {
        result = m->isVisible();
    }
    return result;
}

void Magic3D::Object::play(bool reverse)
{
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->play(); break;
        case eOBJECT_PARTICLES: ((Particles*)this)->play(); break;
        case eOBJECT_SOUND: ((Sound*)this)->play(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation() && m->getSkeleton()->getAnimation()->getCurrentSequence())
            {
                m->getSkeleton()->getAnimation()->play(reverse);
            }
            break;
        }
        default: break;
    }
}

void Magic3D::Object::playAnimation(std::string animation, float interpolation, bool reverse)
{
    switch (getType())
    {
        case eOBJECT_SPRITE:
        {
            Sprite* sprite = static_cast<Sprite*>(this);
            sprite->setCurrentSequence(animation);
            sprite->play();
            break;
        }
        case eOBJECT_MODEL:
        {
            Model* m = static_cast<Model*>(this);
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                m->getSkeleton()->getAnimation()->playAnimation(animation, interpolation, reverse);
            }
            break;
        }
        default: break;
    }
}

void Magic3D::Object::stop()
{
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->stop(); break;
        case eOBJECT_PARTICLES: ((Particles*)this)->stop(); break;
        case eOBJECT_SOUND: ((Sound*)this)->stop(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation() && m->getSkeleton()->getAnimation()->getCurrentSequence())
            {
                m->getSkeleton()->getAnimation()->stop();
            }
            break;
        }
        default: break;
    }
}

bool Magic3D::Object::isPlaying()
{
    bool result = false;
    switch (getType())
    {
        case eOBJECT_SPRITE: result = ((Sprite*)this)->isPlaying(); break;
        case eOBJECT_PARTICLES: result = ((Particles*)this)->isPlaying(); break;
        case eOBJECT_SOUND: result = ((Sound*)this)->isPlaying(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation() && m->getSkeleton()->getAnimation()->getCurrentSequence())
            {
                result = m->getSkeleton()->getAnimation()->isPlaying();
            }
            break;
        }
        default: result = false; break;
    }

    return result;
}

void Magic3D::Object::setCurrentAnimation(std::string animation)
{
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->setCurrentSequence(animation); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                m->getSkeleton()->getAnimation()->setCurrentSequence(animation);
            }
            break;
        }
        default: break;
    }
}

std::string Magic3D::Object::getCurrentAnimation()
{
    std::string result;
    switch (getType())
    {
        case eOBJECT_SPRITE: result = ((Sprite*)this)->getCurrentSequenceName(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                result = m->getSkeleton()->getAnimation()->getCurrentSequenceName();
            }
            break;
        }
        default: result = ""; break;
    }

    return result;
}

void Magic3D::Object::setAnimationFrame(int frame)
{
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->setFrame(frame); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                m->getSkeleton()->getAnimation()->setCurrentFrame(frame);
            }
            break;
        }
        default: break;
    }
}

int Magic3D::Object::getAnimationFrame()
{
    int result = 0;
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->getFrame(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                result = m->getSkeleton()->getAnimation()->getCurrentFrame();
            }
            break;
        }
        default: result = 0; break;
    }

    return result;
}

int Magic3D::Object::getAnimationFrameCount()
{
    int result = 0;
    switch (getType())
    {
        case eOBJECT_SPRITE: ((Sprite*)this)->getFrameCount(); break;
        case eOBJECT_MODEL:
        {
            Model* m = (Model*)this;
            if (m->getSkeleton() && m->getSkeleton()->getAnimation())
            {
                result = m->getSkeleton()->getAnimation()->getFrameCount();
            }
            break;
        }
        default: result = 0; break;
    }

    return result;
}

Magic3D::Object* Magic3D::Object::getObject(std::string name)
{
    return ResourceManager::getObjects()->get(name);
}

Magic3D::Object* Magic3D::Object::getChild(std::string name)
{
    std::string childName = name;
    unsigned int separator = childName.rfind('.');
    childName = getName() + "." + (separator == childName.npos ? childName : childName.substr(separator + 1));
    return getObject(childName);
}

Magic3D::XMLElement* Magic3D::Object::save(XMLElement* root)
{
    if (root)
    {
        saveString(root, M3D_OBJECT_XML_NAME,        getName());
        saveString(root, M3D_OBJECT_XML_SCRIPT,      getScript());
        saveString(root, M3D_OBJECT_XML_PARENT,      getParent() ? getParent()->getName().c_str() : M3D_XML_NULL);
        saveString(root, M3D_OBJECT_XML_PARENT_BONE, getParentBone() ? getParentBone()->getName().c_str() : M3D_XML_NULL);

        saveBool(root,   M3D_OBJECT_XML_PARENT_POSITION, isParentPosition());
        saveBool(root,   M3D_OBJECT_XML_PARENT_ROTATION, isParentRotation());
        saveBool(root,   M3D_OBJECT_XML_PARENT_SCALE, isParentScale());

        saveInt(root,    M3D_OBJECT_XML_RENDER,    getRender());
        saveInt(root,    M3D_OBJECT_XML_TYPE,      getType());
        saveInt(root,    M3D_OBJECT_XML_FLAG,      getFlag());
        saveInt(root,    M3D_OBJECT_XML_BILLBOARD, getBillboard());

        saveBool(root,   M3D_OBJECT_XML_ENABLED,  isEnabled());
        saveBool(root,   M3D_OBJECT_XML_VISIBLE,  isVisible());
        saveBool(root,   M3D_OBJECT_XML_ZORDER,   isZOrder());
        saveBool(root,   M3D_OBJECT_XML_PICK,     isPickable());

        saveVector3(root, M3D_OBJECT_XML_POSITION, getPosition());
        saveVector4(root, M3D_OBJECT_XML_ROTATION, Vector4(getRotation()));
        saveVector3(root, M3D_OBJECT_XML_SCALE,    getScale());

        saveBool(root,    M3D_OBJECT_XML_SCRIPTED,  isScripted());

        std::vector<Mesh*>::const_iterator it_m = meshes.begin();
        while (it_m != meshes.end())
        {
            Mesh* mesh = *it_m++;

            XMLElement* meshtXML = root->GetDocument()->NewElement( M3D_MESH_XML );
            meshtXML->SetAttribute("materials", (int)mesh->getMaterials()->size());
            root->LinkEndChild( meshtXML );

            mesh->save(meshtXML);
        }

        PhysicsObject::save(root);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Object::load(XMLElement* root)
{
    if (root)
    {
        script   = loadString(root, M3D_OBJECT_XML_SCRIPT);
        parentPosition = loadBool(root,   M3D_OBJECT_XML_PARENT_POSITION);
        parentRotation = loadBool(root,   M3D_OBJECT_XML_PARENT_ROTATION);
        parentScale    = loadBool(root,   M3D_OBJECT_XML_PARENT_SCALE);
        flag      = loadInt(root, M3D_OBJECT_XML_FLAG);
        billboard = (BILLBOARD)loadInt(root, M3D_OBJECT_XML_BILLBOARD);
        enabled  = loadBool(root, M3D_OBJECT_XML_ENABLED);
        visible  = loadBool(root, M3D_OBJECT_XML_VISIBLE);
        zOrder   = loadBool(root, M3D_OBJECT_XML_ZORDER);
        pick     = loadBool(root, M3D_OBJECT_XML_PICK);
        position = loadVector3(root, M3D_OBJECT_XML_POSITION);
        setRotation(Quaternion(loadVector4(root, M3D_OBJECT_XML_ROTATION)));
        scale    = loadVector3(root, M3D_OBJECT_XML_SCALE);
        bool sc = loadBool(root, M3D_OBJECT_XML_SCRIPTED);
        if (sc)
        {
            setScripted(true);
        }
        else
        {
            this->scripted = false;
        }

        XMLElement* xml = root->FirstChildElement(M3D_MESH_XML);
        unsigned int meshIndex = 0;
        while (xml)
        {
            Mesh* mesh = NULL;
            if (meshIndex < meshes.size())
            {
                mesh = meshes[meshIndex++];
            }

            if (mesh)
            {
                mesh->load(xml);
            }
            xml = xml->NextSiblingElement(M3D_MESH_XML);
        }

        PhysicsObject::load(root);
    }
    return root;
}

//******************************************************************************
Magic3D::ObjectInstance::ObjectInstance(const ObjectInstance &objectInstance, std::string name) : Object(objectInstance, name)
{
    this->instance = objectInstance.instance;
    this->spawnOnLoad = objectInstance.spawnOnLoad;
}

Magic3D::ObjectInstance::ObjectInstance(std::string name) : Object(eOBJECT_INSTANCE, eRENDER_3D, name)
{
    this->instance = NULL;
    this->spawnOnLoad = false;

    box = Box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));

    ModelInfo* modelInfo = ResourceManager::getInstance()->getModels()->get(M3D_PLACEHOLDER);

    if (modelInfo)
    {
        std::vector<MeshInfo*>::const_iterator it_m = modelInfo->getMeshes()->begin();
        std::vector<MeshInfo*>::const_iterator it_end = modelInfo->getMeshes()->end();
        while (it_m != it_end)
        {
            MeshInfo* meshInfo = *it_m++;

            Mesh* mesh = new Mesh(meshInfo->getData(), meshInfo->isDoubleSide());

            addMesh(mesh);

            Material* material = meshInfo->getMaterial();
            if (!material)
            {
                material = ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL);
            }
            mesh->addMaterial(material);
        }
    }
}

Magic3D::ObjectInstance::~ObjectInstance()
{
    if (instance)
    {
        instance->removeInstance(this);
    }
    instance = NULL;
}

Magic3D::ObjectInstance* Magic3D::ObjectInstance::spawn(std::string name) const
{
    return new ObjectInstance(*this, name);
}

void Magic3D::ObjectInstance::setInstance(Object* object, bool removeOld)
{
    if (this->instance && removeOld)
    {
        this->instance->removeInstance(this);
    }

    this->instance = object;

    if (this->instance)
    {
        this->instance->addInstance(this);
    }
    else
    {
        box = Box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
    }
}

Magic3D::Object* Magic3D::ObjectInstance::getInstance()
{
    return instance;
}

void Magic3D::ObjectInstance::setSpawnOnLoad(bool spawn)
{
    this->spawnOnLoad = spawn;
}

bool Magic3D::ObjectInstance::isSpawnedOnLoad()
{
    return spawnOnLoad;
}

const Magic3D::Box& Magic3D::ObjectInstance::getBoundingBox()
{
    if (instance)
    {
        box = instance->getBoundingBox();
    }

    return box;
}

Magic3D::XMLElement* Magic3D::ObjectInstance::save(XMLElement* root)
{
    if (root)
    {
        saveString(root, M3D_OBJECT_INSTANCE_NAME, instance ? instance->getName() : M3D_XML_NULL);
        saveBool(root,   M3D_OBJECT_INSTANCE_SPAWN, spawnOnLoad);
        Object::save(root);
    }
    return root;
}

Magic3D::XMLElement* Magic3D::ObjectInstance::load(XMLElement* root)
{
    if (root)
    {
        spawnOnLoad = loadBool(root,   M3D_OBJECT_INSTANCE_SPAWN);
        Object::load(root);
    }
    return root;
}
