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

Magic3D::Particles::Particles(const Particles& particles, std::string name) : Object(particles, name)
{
    this->box = particles.box;

    this->minEmitColor = particles.minEmitColor;
    this->maxEmitColor = particles.maxEmitColor;

    this->minDieColor = particles.minDieColor;
    this->maxDieColor = particles.maxDieColor;

    this->worldPosition = particles.worldPosition;
    this->minCreationDistance = particles.minCreationDistance;
    this->maxCreationDistance = particles.maxCreationDistance;

    this->acceleration = particles.acceleration;

    this->uv1 = particles.uv1;
    this->uv2 = particles.uv2;
    this->uv3 = particles.uv3;
    this->uv4 = particles.uv4;

    this->second = particles.second;

    this->minSpeed = particles.minSpeed;
    this->maxSpeed = particles.maxSpeed;

    this->minAcceleration = particles.minAcceleration;
    this->maxAcceleration = particles.maxAcceleration;

    this->minAge = particles.minAge;
    this->maxAge = particles.maxAge;

    this->minEmitSize = particles.minEmitSize;
    this->maxEmitSize = particles.maxEmitSize;

    this->minDieSize = particles.minDieSize;
    this->maxDieSize = particles.maxDieSize;

    this->minSpinSpeed = particles.minSpinSpeed;
    this->maxSpinSpeed = particles.maxSpinSpeed;

    this->minSpinAcceleration = particles.minSpinAcceleration;
    this->maxSpinAcceleration = particles.maxSpinAcceleration;

    this->minRadius = particles.minRadius;
    this->maxRadius = particles.maxRadius;

    this->minParticlesCount = particles.minParticlesCount;
    this->maxParticlesCount = particles.maxParticlesCount;
    this->particlesSecond = particles.particlesSecond;

    this->particlesCount = particles.particlesCount;
    this->particlesCreated = particles.particlesCreated;

    this->billboardParticle = particles.billboardParticle;

    this->playing = false;
    this->needStop = false;
    this->loop = particles.loop;

    this->recreateWhenDie = particles.recreateWhenDie;
    this->followEmitter = particles.followEmitter;
    this->stopCreating = particles.stopCreating;
    this->creationDirection = particles.creationDirection;
    this->invertedDirection = particles.invertedDirection;

    this->trail = particles.trail;

    this->uniqueUpdate = particles.uniqueUpdate;

    this->distortion = particles.distortion;
    this->distortionType = particles.distortionType;
    this->distortionFrequency = particles.distortionFrequency;
    this->distortionFactor = particles.distortionFactor;
    this->distortionWave = particles.distortionWave;

    particlesMesh = getMeshes()->at(0);

    if (particles.playing)
    {
        this->play();
    }
}

Magic3D::Particles::Particles(std::string name) : Object(eOBJECT_PARTICLES, eRENDER_3D, name)
{
    init();
}

void Magic3D::Particles::init()
{
    playing             = false;
    needStop            = false;
    loop                = true;

    recreateWhenDie     = false;
    followEmitter       = true;
    stopCreating        = false;
    creationDirection   = false;
    invertedDirection   = false;

    trail               = false;

    uniqueUpdate        = false;

    second              = 0.0f;

    particlesCount      = 0;
    particlesCreated    = 0;

    minSpeed            = 0.0f;
    maxSpeed            = 0.0f;

    minAcceleration     = 0.0f;
    maxAcceleration     = 0.0f;

    minAge              = 1.0f;
    maxAge              = 1.0f;

    minEmitSize         = 1.0f;
    maxEmitSize         = 1.0f;

    minDieSize          = 1.0f;
    maxDieSize          = 1.0f;

    minSpinSpeed        = 0.0f;
    maxSpinSpeed        = 0.0f;

    minSpinAcceleration = 0.0f;
    maxSpinAcceleration = 0.0f;

    minRadius           = 0.0f;
    maxRadius           = 0.0f;

    worldPosition       = Vector3(0.0f, 0.0f, 0.0f);
    minCreationDistance = Vector3(0.0f, 0.0f, 0.0f);
    maxCreationDistance = Vector3(0.0f, 0.0f, 0.0f);

    acceleration        = Vector3(0.0f, 0.0f, 0.0f);

    minEmitColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    maxEmitColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);

    minDieColor  = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    maxDieColor  = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);

    billboardParticle = eBILLBOARD_NONE;

    minParticlesCount = 1;
    maxParticlesCount = 1;
    particlesSecond   = 1;

    uv1 = Texture2D(0.0f, 1.0f);
    uv2 = Texture2D(0.0f, 0.0f);
    uv3 = Texture2D(1.0f, 0.0f);
    uv4 = Texture2D(1.0f, 1.0f);

    distortion = false;
    distortionType = eDISTORTION_BUBBLE;
    distortionFrequency = 1.0f;
    distortionFactor = 0.05f;
    distortionWave = 1.0f;

    particlesMesh = new Mesh(eMESH_TRIANGLES, PARTICLES_MESH_NAME, true);

    addMesh(particlesMesh);

    addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_PARTICLES));
}


Magic3D::Particles::~Particles()
{
    particlesMesh = NULL;
}

Magic3D::Particles* Magic3D::Particles::spawn(std::string name) const
{
    return (new Particles(*this, name));
}

void Magic3D::Particles::updateMatrix()
{
    Object::updateMatrix();

    if (!isFollowingEmitter())
    {
        setMatrix(Matrix4::identity());
    }
}

Magic3D::Matrix4 Magic3D::Particles::getMatrixFromParent()
{
    Matrix4 m = getMatrix();

    if (getParent() && isFollowingEmitter())
    {
        m = getParent()->getMatrixFromParent();

        if (getParentBone())
        {
            m = m * getParentBone()->getMatrixFromParent();
        }
        m = m * getMatrix();
    }

    return m;
}

const Magic3D::Vector3& Magic3D::Particles::getWorldPosition()
{
    return worldPosition;
}

bool Magic3D::Particles::update()
{
    Object::update();

    bool result = isPlaying();
    bool needUpdate = Scene::getInstance()->getUniqueUpdateFlag() != uniqueUpdate;

    second += Magic3D::Magic3D::getInstance()->getElapsedTime();
    if (needUpdate && result && (isInParentFrustum() || isInEffectFrustum()))
    {
        uniqueUpdate = !uniqueUpdate;

        if (second >= 1.0f)
        {
            second = 0.0f;
            particlesCreated = 0;
        }
        updateMeshes();
    }

    if (result && isVisible() && isInParentFrustum() && distortion)
    {
        Box box = getBoundingBox();
        Vector3 pos = (getMatrixFromParent() * Vector4(box.getCenter(), 1.0f)).getXYZ();
        Renderer::getInstance()->addDistortion(pos, distortionType, box.getMaxSize() * 0.5f, distortionFrequency, distortionFactor, distortionWave);
    }

    return result;
}

bool Magic3D::Particles::updateMeshes()
{
    ViewPort* view = Renderer::getInstance()->getCurrentViewPort();
    Camera* camera = getRender() == eRENDER_2D ? view->getOrthographic() : view->getPerspective();

    Vector3 bbX = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 bbY = Vector3(0.0f, 1.0f, 0.0f);

    if (camera && getBillboardParticle() != eBILLBOARD_NONE)
    {
        Matrix4 m = isFollowingEmitter() ? inverse(camera->getView() * getMatrixFromParent()) : camera->getMatrixFromParent();

        if (getBillboardParticle() == eBILLBOARD_HORIZONTAL || getBillboardParticle() == eBILLBOARD_BOTH)
        {
            bbX = isFollowingEmitter() ? m.getCol0().getXYZ() : cross(m.getCol2().getXYZ(), m.getCol1().getXYZ());
        }
        if (getBillboardParticle() == eBILLBOARD_VERTICAL || getBillboardParticle() == eBILLBOARD_BOTH)
        {
            bbY = m.getCol1().getXYZ();
        }
    }

    float step = Magic3D::Magic3D::getInstance()->getElapsedTime();
    bool first = true;
    if (particlesMesh)
    {
        MeshData* data = particlesMesh->getData();

        float* buffer = data->mapBuffer();

        Vector3 n2 = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 n3 = Vector3(0.0f, 0.0f, 0.0f);

        bool moved = false;
        Vector3 lastPos = Vector3(0.0f, 0.0f, 0.0f);

        bool hasAlive = false;
        for (int i = 0; i < getMaxParticlesCount(); i++)
        {
            int idx = i;
            float factor = 1.0f - idx * 1.0f / getMaxParticlesCount();

            if (idx < getMaxParticlesCount())
            {
                Particle* particle = &particles.at(idx);
                Particle* prior = NULL;
                Particle* next = NULL;

                if (idx > 0)
                {
                    prior = &particles.at(idx - 1);
                }

                if (idx < getMaxParticlesCount() - 1)
                {
                    next = &particles.at(idx + 1);
                    if (!next->alive)
                    {
                        next = NULL;
                    }
                }

                if(idx == 0 && isTrail())
                {
                    createParticle(particle, false);
                }
                if (!stopCreating && !particle->alive)
                {
                    if (particlesCreated < getParticlesSecond() || particlesCount < getMinParticlesCount())
                    {
                        createParticle(particle, false);

                        particlesCount++;
                        particlesCreated++;
                    }
                }

                if (particle->alive)
                {
                    hasAlive = true;
                    particle->age += step;

                    if (moved)
                    {
                        Vector3 pos = lastPos;
                        lastPos = particle->position;
                        createParticle(particle, true);
                        particle->position = pos;

                        particle->age = 0.0f;
                    }

                    if (particle->age > particle->dieAge)
                    {
                        if (isRecreatedWhenDie())
                        {
                            createParticle(particle, false);
                        }
                        else
                        {
                            if (!isLooping())
                            {
                                if (particlesCount >= getMinParticlesCount())
                                {
                                    stopCreating = true;
                                }

                                if (stopCreating && particlesCount <= 0)
                                {
                                    stop();
                                }
                            }

                            if (isTrail())
                            {
                                if (prior)
                                {
                                    lastPos = particle->position;
                                    createParticle(particle, true);
                                    particle->position = prior->position;
                                    particle->age = 0.0f;
                                    moved = true;
                                }                                
                            }
                            else
                            {
                                particle->alive = false;
                                particlesCount--;
                            }
                        }
                    }
                    //else
                    {
                        float size = particle->size;
                        if (isTrail())
                        {
                            size = getMinDieSize() + factor * getMinEmitSize();
                        }
                        else
                        {
                            size = particle->size + particle->sizeFactor * particle->age / particle->dieAge;
                        }
                        float halfSize = size * 0.5f;

                        particle->angle += particle->angle * Math::randomize(minSpinAcceleration, maxSpinAcceleration) * (particle->age / particle->dieAge) * step;

                        Vector3 v1 = particle->position;
                        Vector3 v2 = particle->position;
                        Vector3 v3 = particle->position;
                        Vector3 v4 = particle->position;

                        if (isTrail())
                        {
                            Vector3 vDir = Vector3(0.0f, 0.0f, 0.0f);
                            float distance = 0.0f;

                            if (next)
                            {
                                vDir = particle->position - next->position;
                                distance = fabsf(length(vDir));
                                vDir = normalize(vDir);
                            }

                            if (distance > 0.0f && distance <= size)
                            {
                                vDir = cross(vDir, camera->getDirectionFront());

                                v1 = next->position + (-vDir) * halfSize;
                                v2 = n2;
                                v3 = n3;
                                v4 = next->position + (vDir) * halfSize;

                                n2 = v1;
                                n3 = v4;
                            }
                            if (idx == 0)
                            {
                                if (distance > 0.0f)
                                {
                                    v1 = particle->position + (-vDir) * halfSize;
                                    v2 = particle->position;
                                    v3 = particle->position;
                                    v4 = particle->position + (vDir) * halfSize;
                                }
                                else
                                {
                                    v1 = particle->position;
                                    v2 = particle->position;
                                    v3 = particle->position;
                                    v4 = particle->position;
                                }
                                n2 = v1;
                                n3 = v4;
                            }
                        }
                        else
                        {
                            float angle = Math::radians(particle->angle);
                            Vector3 rotX = bbX;
                            Vector3 rotY = bbY;

                            Vector3 bX = rotX * cosf(angle) + rotY * sinf(angle);
                            Vector3 bY = rotY * cosf(angle) - rotX * sinf(angle);

                            bX *= halfSize;
                            bY *= halfSize;

                            v1 = particle->position - bX + bY;
                            v2 = particle->position - bX - bY;
                            v3 = particle->position + bX - bY;
                            v4 = particle->position + bX + bY;
                        }

                        ColorRGBA color1;
                        ColorRGBA color2;

                        color2.r = particle->color.r + particle->colorFactor.r * particle->age / particle->dieAge;
                        color2.g = particle->color.g + particle->colorFactor.g * particle->age / particle->dieAge;
                        color2.b = particle->color.b + particle->colorFactor.b * particle->age / particle->dieAge;

                        color1.r = color2.r;
                        color1.g = color2.g;
                        color1.b = color2.b;

                        if (isTrail())
                        {
                            color2.a = 1.0f - (idx - 1) * 1.0f / getMaxParticlesCount();
                            color1.a = factor;
                        }
                        else
                        {
                            color2.a = particle->color.a + particle->colorFactor.a * particle->age / particle->dieAge;
                            color1.a = color2.a;
                        }

                        Vector3 dir = isUsingCreationDirection() ? particle->direction : (isFollowingEmitter() ? Vector3(0.0f, 0.0f, 1.0f) : particle->direction);

                        Vector3 accel = acceleration;
                        if (!isFollowingEmitter())
                        {
                            accel = (Matrix4(getRotationFromParent(), Vector3(0.0f, 0.0f, 0.0f)) * Vector4(accel, 0.0f)).getXYZ();
                        }

                        particle->position += (dir * (isInvertedDirection() ? -1.0f : 1.0f)) * Math::randomize(minSpeed, maxSpeed) * ((particle->dieAge - particle->age) / particle->dieAge) * step;
                        particle->position += accel * Math::randomize(minAcceleration, maxAcceleration) * particle->age / particle->dieAge * step;

                        particlesMesh->getData()->setQuad(buffer, idx, v1, v2, v3, v4, color1, color2, color2, color1);
                        particlesMesh->getData()->setQuadTexture(buffer, i, uv1, uv2, uv3, uv4);

                        updateBoundingBox(v1, v2, v3, v4, first);
                        if (first)
                        {
                            first = false;
                        }
                    }
                }
                else
                {
                    particlesMesh->getData()->setQuad(buffer, idx, 0.0f, 0.0f, 0.0f, 0.0f, ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f));
                }
            }
        }

        if (!hasAlive && needStop)
        {
            needStop = false;
            playing = false;
        }

        data->unmapBuffer();
    }

    return true;
}

void Magic3D::Particles::createParticle(Particle* particle, bool reset)
{
    if (needStop || !playing)
    {
        return;
    }
    particle->alive = true;
    particle->dieAge  = isTrail() ? getMinAge() : Math::randomize(getMinAge(), getMaxAge());
    if (particle->dieAge == 0.0f)
    {
        particle->dieAge = 1.0f;
    }
    particle->age     = 0.0f;

    particle->dieSize    = isTrail() ? getMinDieSize() : Math::randomize(getMinDieSize(), getMaxDieSize());
    particle->size       = isTrail() ? getMinEmitSize() : Math::randomize(getMinEmitSize(), getMaxEmitSize());
    particle->sizeFactor = particle->dieSize - particle->size;

    particle->dieColor.r = Math::randomize(minDieColor.r, maxDieColor.r);
    particle->dieColor.g = Math::randomize(minDieColor.g, maxDieColor.g);
    particle->dieColor.b = Math::randomize(minDieColor.b, maxDieColor.b);
    particle->dieColor.a = Math::randomize(minDieColor.a, maxDieColor.a);

    particle->color.r = Math::randomize(minEmitColor.r, maxEmitColor.r);
    particle->color.g = Math::randomize(minEmitColor.g, maxEmitColor.g);
    particle->color.b = Math::randomize(minEmitColor.b, maxEmitColor.b);
    particle->color.a = Math::randomize(minEmitColor.a, maxEmitColor.a);

    particle->colorFactor.r = particle->dieColor.r - particle->color.r;
    particle->colorFactor.g = particle->dieColor.g - particle->color.g;
    particle->colorFactor.b = particle->dieColor.b - particle->color.b;
    particle->colorFactor.a = particle->dieColor.a - particle->color.a;

    if (reset)
    {
        particle->position.setX(0.0f);
        particle->position.setY(0.0f);
        particle->position.setZ(0.0f);
        particle->direction.setX(0.0f);
        particle->direction.setY(0.0f);
        particle->direction.setZ(0.0f);
        particle->angle = 0.0f;
    }
    else
    {
        Vector3 pos = getPosition();

        if (!followEmitter)
        {
            if (getParent())
            {
                Matrix4 p = getParent()->getMatrixFromParent();
                if (getParentBone())
                {
                    p = p * getParentBone()->getMatrixFromParent();
                }
                pos = (p * Vector4(pos, 1.0f)).getXYZ();
            }
            worldPosition = pos;
        }

        particle->position.setX((isFollowingEmitter() ? 0.0f : pos.getX()) + Math::randomize(minCreationDistance.getX(), maxCreationDistance.getX()));
        particle->position.setY((isFollowingEmitter() ? 0.0f : pos.getY()) + Math::randomize(minCreationDistance.getY(), maxCreationDistance.getY()));
        particle->position.setZ((isFollowingEmitter() ? 0.0f : pos.getZ()) + Math::randomize(minCreationDistance.getZ(), maxCreationDistance.getZ()));

        if (creationDirection)
        {
            particle->direction = particle->position - (isFollowingEmitter() ? Vector3(0.0f, 0.0f, 0.0f) : pos);
        }
        else
        {
            particle->direction = Matrix3(Quaternion(Object::getMatrixFromParent().getUpper3x3()) * getRotation()).getCol2();
        }

        if (length(particle->direction) != 0)
        {
            particle->direction = normalize(particle->direction);
        }

        particle->position += particle->direction * Math::randomize(getMinRadius(), getMaxRadius());


        particle->angle = Math::randomize(getMinSpinSpeed(), getMaxSpinSpeed());
    }
}

void Magic3D::Particles::updateBoundingBox(Vector3& v1, Vector3& v2, Vector3& v3, Vector3& v4, bool reset)
{
    Vector3 min = Vector3(Math::min(Math::min(v1.getX(), v2.getX()), Math::min(v3.getX(), v4.getX())),
                          Math::min(Math::min(v1.getY(), v2.getY()), Math::min(v3.getY(), v4.getY())),
                          Math::min(Math::min(v1.getZ(), v2.getZ()), Math::min(v3.getZ(), v4.getZ())));

    Vector3 max = Vector3(Math::max(Math::max(v1.getX(), v2.getX()), Math::max(v3.getX(), v4.getX())),
                          Math::max(Math::max(v1.getY(), v2.getY()), Math::max(v3.getY(), v4.getY())),
                          Math::max(Math::max(v1.getZ(), v2.getZ()), Math::max(v3.getZ(), v4.getZ())));
    if (reset)
    {
        box.corners[0] = min;
        box.corners[1] = max;
    }
    else
    {
        box.corners[0] = Vector3(Math::min(min.getX(), box.corners[0].getX()),
                                 Math::min(min.getY(), box.corners[0].getY()),
                                 Math::min(min.getZ(), box.corners[0].getZ()));

        box.corners[1] = Vector3(Math::max(max.getX(), box.corners[1].getX()),
                                 Math::max(max.getY(), box.corners[1].getY()),
                                 Math::max(max.getZ(), box.corners[1].getZ()));
    }
}

void Magic3D::Particles::addMaterial(Material* material)
{
    if (particlesMesh && material)
    {
        material->setShader(ResourceManager::getShaders()->get(M3D_DEFAULT_SHADER_PARTICLES));
        particlesMesh->getMaterials()->clear();
        particlesMesh->addMaterial(material);
    }
}

const Magic3D::Box& Magic3D::Particles::getBoundingBox()
{
    return box;
}

void Magic3D::Particles::play()
{
    particles.clear();
    particlesMesh->getData()->clear();

    for (int i = 0; i < getMaxParticlesCount(); i++)
    {
        Particle particle;
        particles.push_back(particle);
        particlesMesh->getData()->addQuad(0.0f, 0.0f, 0.0f, 0.0f, eAXIS_Z, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    }

    particlesMesh->getData()->createVbo();

    particlesMesh->setDoubleSide(getBillboardParticle() == eBILLBOARD_NONE);

    second = 0.0f;
    particlesCount = 0;
    particlesCreated = 0;

    stopCreating = false;

    playing = true;
    needStop = false;
}

void Magic3D::Particles::stop()
{
    needStop = true;
}

bool Magic3D::Particles::isPlaying()
{
    return playing;
}

void Magic3D::Particles::setLooping(bool loop)
{
    this->loop = loop;
}

bool Magic3D::Particles::isLooping()
{
    return loop;
}

void Magic3D::Particles::setRecreateWhenDie(bool recreate)
{
    recreateWhenDie = recreate;
}

bool Magic3D::Particles::isRecreatedWhenDie()
{
    return recreateWhenDie;
}

void Magic3D::Particles::setFollowEmitter(bool follow)
{
    followEmitter = follow;
    needTransform = true;
}

bool Magic3D::Particles::isFollowingEmitter()
{
    return followEmitter;
}

void Magic3D::Particles::setUseCreationDirection(bool creation)
{
    creationDirection = creation;
}

bool Magic3D::Particles::isUsingCreationDirection()
{
    return creationDirection;
}

void Magic3D::Particles::setInvertedDirection(bool inverted)
{
    this->invertedDirection = inverted;
}

bool Magic3D::Particles::isInvertedDirection()
{
    return invertedDirection;
}

void Magic3D::Particles::setMinSpeed(float speed)
{
    minSpeed = speed;
}

float Magic3D::Particles::getMinSpeed()
{
    return minSpeed;
}

void Magic3D::Particles::setMaxSpeed(float speed)
{
    maxSpeed = speed;
}

float Magic3D::Particles::getMaxSpeed()
{
    return maxSpeed;
}

void Magic3D::Particles::setMinAcceleration(float acceleration)
{
    minAcceleration = acceleration;
}

float Magic3D::Particles::getMinAcceleration()
{
    return minAcceleration;
}

void Magic3D::Particles::setMaxAcceleration(float acceleration)
{
    maxAcceleration = acceleration;
}

float Magic3D::Particles::getMaxAcceleration()
{
    return maxAcceleration;
}

void Magic3D::Particles::setMinAge(float age)
{
    minAge = age;
}

float Magic3D::Particles::getMinAge()
{
    return minAge;
}

void Magic3D::Particles::setMaxAge(float age)
{
    maxAge = age;
}

float Magic3D::Particles::getMaxAge()
{
    return maxAge;
}

void Magic3D::Particles::setMinEmitSize(float size)
{
    minEmitSize = size;
}

float Magic3D::Particles::getMinEmitSize()
{
    return minEmitSize;
}

void Magic3D::Particles::setMaxEmitSize(float size)
{
    maxEmitSize = size;
}

float Magic3D::Particles::getMaxEmitSize()
{
    return maxEmitSize;
}

void Magic3D::Particles::setMinDieSize(float size)
{
    minDieSize = size;
}

float Magic3D::Particles::getMinDieSize()
{
    return minDieSize;
}

void Magic3D::Particles::setMaxDieSize(float size)
{
    maxDieSize = size;
}

float Magic3D::Particles::getMaxDieSize()
{
    return maxDieSize;
}

void Magic3D::Particles::setMinSpinSpeed(float speed)
{
    minSpinSpeed = speed;
}

float Magic3D::Particles::getMinSpinSpeed()
{
    return minSpinSpeed;
}

void Magic3D::Particles::setMaxSpinSpeed(float speed)
{
    maxSpinSpeed = speed;
}

float Magic3D::Particles::getMaxSpinSpeed()
{
    return maxSpinSpeed;
}

void Magic3D::Particles::setMinSpinAcceleration(float acceleration)
{
    minSpinAcceleration = acceleration;
}

float Magic3D::Particles::getMinSpinAcceleration()
{
    return minSpinAcceleration;
}

void Magic3D::Particles::setMaxSpinAcceleration(float acceleration)
{
    maxSpinAcceleration = acceleration;
}

float Magic3D::Particles::getMaxSpinAcceleration()
{
    return maxSpinAcceleration;
}

void Magic3D::Particles::setMinRadius(float radius)
{
    minRadius = radius;
}

float Magic3D::Particles::getMinRadius()
{
    return minRadius;
}

void Magic3D::Particles::setMaxRadius(float radius)
{
    maxRadius = radius;
}

float Magic3D::Particles::getMaxRadius()
{
    return maxRadius;
}

void Magic3D::Particles::setMinCreationDistance(Vector3 distance)
{
    minCreationDistance = distance;
}

const Magic3D::Vector3& Magic3D::Particles::getMinCreationDistance()
{
    return minCreationDistance;
}

void Magic3D::Particles::setMaxCreationDistance(Vector3 distance)
{
    maxCreationDistance = distance;
}

const Magic3D::Vector3& Magic3D::Particles::getMaxCreationDistance()
{
    return maxCreationDistance;
}

void Magic3D::Particles::setAcceleration(Vector3 acceleration)
{
    this->acceleration = acceleration;
}

const Magic3D::Vector3& Magic3D::Particles::getAcceleration()
{
    return acceleration;
}

void Magic3D::Particles::setMinEmitColor(ColorRGBA color)
{
    minEmitColor = color;
}

const Magic3D::ColorRGBA& Magic3D::Particles::getMinEmitColor()
{
    return minEmitColor;
}

void Magic3D::Particles::setMaxEmitColor(ColorRGBA color)
{
    maxEmitColor = color;
}

const Magic3D::ColorRGBA& Magic3D::Particles::getMaxEmitColor()
{
    return maxEmitColor;
}

void Magic3D::Particles::setMinDieColor(ColorRGBA color)
{
    minDieColor = color;
}

const Magic3D::ColorRGBA& Magic3D::Particles::getMinDieColor()
{
    return minDieColor;
}

void Magic3D::Particles::setMaxDieColor(ColorRGBA color)
{
    maxDieColor = color;
}

const Magic3D::ColorRGBA& Magic3D::Particles::getMaxDieColor()
{
    return maxDieColor;
}

void Magic3D::Particles::setBillboardParticle(BILLBOARD billboard)
{
    this->billboardParticle = billboard;
}

Magic3D::BILLBOARD Magic3D::Particles::getBillboardParticle()
{
    return billboardParticle;
}

void Magic3D::Particles::setMinParticlesCount(int count)
{
    minParticlesCount = count;
}

int Magic3D::Particles::getMinParticlesCount()
{
    return minParticlesCount;
}

void Magic3D::Particles::setMaxParticlesCount(int count)
{
    maxParticlesCount = count;
}

int Magic3D::Particles::getMaxParticlesCount()
{
    return maxParticlesCount;
}

void Magic3D::Particles::setParticlesSecond(int count)
{
    particlesSecond = count;
}

int Magic3D::Particles::getParticlesSecond()
{
    return particlesSecond;
}

void Magic3D::Particles::setUV1(Texture2D uv)
{
    uv1 = uv;
}

const Magic3D::Texture2D& Magic3D::Particles::getUV1()
{
    return uv1;
}

void Magic3D::Particles::setUV2(Texture2D uv)
{
    uv2 = uv;
}

const Magic3D::Texture2D& Magic3D::Particles::getUV2()
{
    return uv2;
}

void Magic3D::Particles::setUV3(Texture2D uv)
{
    uv3 = uv;
}

const Magic3D::Texture2D& Magic3D::Particles::getUV3()
{
    return uv3;
}

void Magic3D::Particles::setUV4(Texture2D uv)
{
    uv4 = uv;
}

const Magic3D::Texture2D& Magic3D::Particles::getUV4()
{
    return uv4;
}

void Magic3D::Particles::setTrail(bool trail)
{
    this->trail = trail;
}

bool Magic3D::Particles::isTrail()
{
    return trail;
}

void Magic3D::Particles::setDistorting(bool distort)
{
    distortion = distort;
}

bool Magic3D::Particles::isDistorting()
{
    return distortion;
}

void Magic3D::Particles::setDistortionType(DISTORTION type)
{
    distortionType = type;
}

Magic3D::DISTORTION Magic3D::Particles::getDistortiionType()
{
    return distortionType;
}

void Magic3D::Particles::setDistortionFrequency(float value)
{
    distortionFrequency = value;
}

float Magic3D::Particles::getDistortionFrequency()
{
    return distortionFrequency;
}

void Magic3D::Particles::setDistortionFactor(float value)
{
    distortionFactor = value;
}

float Magic3D::Particles::getDistortionFactor()
{
    return distortionFactor;
}

void Magic3D::Particles::setDistortionWave(float value)
{
    distortionWave = value;
}

float Magic3D::Particles::getDistortionWave()
{
    return distortionWave;
}

Magic3D::XMLElement* Magic3D::Particles::save(XMLElement* root)
{
    Object::save(root);
    saveNode(root);
    return root;
}

void Magic3D::Particles::saveNode(XMLElement* root)
{
    if (root)
    {
        XMLElement* node = root->GetDocument()->NewElement( M3D_PARTICLES_XML );
        root->LinkEndChild( node );

        saveInt(node,       M3D_PARTICLES_XML_PLAYING,             playing && !needStop ? 1 : 0);
        saveInt(node,       M3D_PARTICLES_XML_LOOP,                loop ? 1 : 0);

        saveInt(node,       M3D_PARTICLES_XML_RECREATEWHENDIE,     recreateWhenDie ? 1 : 0);
        saveInt(node,       M3D_PARTICLES_XML_FOLLOWEMITTER,       followEmitter ? 1 : 0);
        saveInt(node,       M3D_PARTICLES_XML_CREATIONDIRECTION,   creationDirection ? 1 : 0);
        saveInt(node,       M3D_PARTICLES_XML_INVERTEDDIRECTION,   invertedDirection ? 1 : 0);

        saveInt(node,       M3D_PARTICLES_XML_TRAIL,               trail ? 1 : 0);

        saveFloat(node,     M3D_PARTICLES_XML_MINSPEED,            minSpeed);
        saveFloat(node,     M3D_PARTICLES_XML_MAXSPEED,            maxSpeed);

        saveFloat(node,     M3D_PARTICLES_XML_MINACCELERATION,     minAcceleration);
        saveFloat(node,     M3D_PARTICLES_XML_MAXACCELERATION,     maxAcceleration);

        saveFloat(node,     M3D_PARTICLES_XML_MINAGE,              minAge);
        saveFloat(node,     M3D_PARTICLES_XML_MAXAGE,              maxAge);

        saveFloat(node,     M3D_PARTICLES_XML_MINEMITSIZE,         minEmitSize);
        saveFloat(node,     M3D_PARTICLES_XML_MAXEMITSIZE,         maxEmitSize);

        saveFloat(node,     M3D_PARTICLES_XML_MINDIESIZE,          minDieSize);
        saveFloat(node,     M3D_PARTICLES_XML_MAXDIESIZE,          maxDieSize);

        saveFloat(node,     M3D_PARTICLES_XML_MINSPINSPEED,        minSpinSpeed);
        saveFloat(node,     M3D_PARTICLES_XML_MAXSPINSPEED,        maxSpinSpeed);

        saveFloat(node,     M3D_PARTICLES_XML_MINSPINACCELERATION, minSpinAcceleration);
        saveFloat(node,     M3D_PARTICLES_XML_MAXSPINACCELERATION, maxSpinAcceleration);

        saveFloat(node,     M3D_PARTICLES_XML_MINRADIUS,           minRadius);
        saveFloat(node,     M3D_PARTICLES_XML_MAXRADIUS,           maxRadius);

        saveVector3(node,   M3D_PARTICLES_XML_MINCREATIONDISTANCE, minCreationDistance);
        saveVector3(node,   M3D_PARTICLES_XML_MAXCREATIONDISTANCE, maxCreationDistance);

        saveVector3(node,   M3D_PARTICLES_XML_ACCELERATION,        acceleration);

        saveColorRGBA(node, M3D_PARTICLES_XML_MINEMITCOLOR,        minEmitColor);
        saveColorRGBA(node, M3D_PARTICLES_XML_MAXEMITCOLOR,        maxEmitColor);

        saveColorRGBA(node, M3D_PARTICLES_XML_MINDIECOLOR,         minDieColor);
        saveColorRGBA(node, M3D_PARTICLES_XML_MAXDIECOLOR,         maxDieColor);

        saveInt(node,       M3D_PARTICLES_XML_BILLBOARD,           billboardParticle);

        saveInt(node,       M3D_PARTICLES_XML_MINPARTICLESCOUNT,   minParticlesCount);
        saveInt(node,       M3D_PARTICLES_XML_MAXPARTICLESCOUNT,   maxParticlesCount);

        saveInt(node,       M3D_PARTICLES_XML_PARTICLESSECOND,     particlesSecond);

        saveVector4(node,   M3D_PARTICLES_XML_UV1_UV2,             Vector4(uv1.u, uv1.v, uv2.u, uv2.v));
        saveVector4(node,   M3D_PARTICLES_XML_UV3_UV4,             Vector4(uv3.u, uv3.v, uv4.u, uv4.v));

        saveBool(node,      M3D_PARTICLES_XML_DISTORTION,          distortion);
        saveInt(node,       M3D_PARTICLES_XML_DISTORTIONTYPE,      distortionType);
        saveFloat(node,     M3D_PARTICLES_XML_DISTORTIONFREQUENCY, distortionFrequency);
        saveFloat(node,     M3D_PARTICLES_XML_DISTORTIONFACTOR,    distortionFactor);
        saveFloat(node,     M3D_PARTICLES_XML_DISTORTIONWAVE,      distortionWave);
    }
}

Magic3D::XMLElement* Magic3D::Particles::load(XMLElement* root)
{
    Object::load(root);
    loadNode(root);
    return root;
}

void Magic3D::Particles::loadNode(XMLElement* root)
{
    if (root)
    {
        XMLElement* node = root->FirstChildElement(M3D_PARTICLES_XML);

        loop                = loadInt(node,       M3D_PARTICLES_XML_LOOP)              == 0 ? false : true;

        recreateWhenDie     = loadInt(node,       M3D_PARTICLES_XML_RECREATEWHENDIE)   == 0 ? false : true;
        followEmitter       = loadInt(node,       M3D_PARTICLES_XML_FOLLOWEMITTER)     == 0 ? false : true;
        creationDirection   = loadInt(node,       M3D_PARTICLES_XML_CREATIONDIRECTION) == 0 ? false : true;
        invertedDirection   = loadInt(node,       M3D_PARTICLES_XML_INVERTEDDIRECTION) == 0 ? false : true;

        trail               = loadInt(node,       M3D_PARTICLES_XML_TRAIL)             == 0 ? false : true;

        minSpeed            = loadFloat(node,     M3D_PARTICLES_XML_MINSPEED);
        maxSpeed            = loadFloat(node,     M3D_PARTICLES_XML_MAXSPEED);

        minAcceleration     = loadFloat(node,     M3D_PARTICLES_XML_MINACCELERATION);
        maxAcceleration     = loadFloat(node,     M3D_PARTICLES_XML_MAXACCELERATION);

        minAge              = loadFloat(node,     M3D_PARTICLES_XML_MINAGE);
        maxAge              = loadFloat(node,     M3D_PARTICLES_XML_MAXAGE);

        minEmitSize         = loadFloat(node,     M3D_PARTICLES_XML_MINEMITSIZE);
        maxEmitSize         = loadFloat(node,     M3D_PARTICLES_XML_MAXEMITSIZE);

        minDieSize          = loadFloat(node,     M3D_PARTICLES_XML_MINDIESIZE);
        maxDieSize          = loadFloat(node,     M3D_PARTICLES_XML_MAXDIESIZE);

        minSpinSpeed        = loadFloat(node,     M3D_PARTICLES_XML_MINSPINSPEED);
        maxSpinSpeed        = loadFloat(node,     M3D_PARTICLES_XML_MAXSPINSPEED);

        minSpinAcceleration = loadFloat(node,     M3D_PARTICLES_XML_MINSPINACCELERATION);
        maxSpinAcceleration = loadFloat(node,     M3D_PARTICLES_XML_MAXSPINACCELERATION);

        minRadius           = loadFloat(node,     M3D_PARTICLES_XML_MINRADIUS);
        maxRadius           = loadFloat(node,     M3D_PARTICLES_XML_MAXRADIUS);

        minCreationDistance = loadVector3(node,   M3D_PARTICLES_XML_MINCREATIONDISTANCE);
        maxCreationDistance = loadVector3(node,   M3D_PARTICLES_XML_MAXCREATIONDISTANCE);

        acceleration        = loadVector3(node,   M3D_PARTICLES_XML_ACCELERATION);

        minEmitColor        = loadColorRGBA(node, M3D_PARTICLES_XML_MINEMITCOLOR);
        maxEmitColor        = loadColorRGBA(node, M3D_PARTICLES_XML_MAXEMITCOLOR);

        minDieColor         = loadColorRGBA(node, M3D_PARTICLES_XML_MINDIECOLOR);
        maxDieColor         = loadColorRGBA(node, M3D_PARTICLES_XML_MAXDIECOLOR);

        billboardParticle   = BILLBOARD(loadInt(node, M3D_PARTICLES_XML_BILLBOARD));

        minParticlesCount   = loadInt(node,       M3D_PARTICLES_XML_MINPARTICLESCOUNT);
        maxParticlesCount   = loadInt(node,       M3D_PARTICLES_XML_MAXPARTICLESCOUNT);

        particlesSecond     = loadInt(node,       M3D_PARTICLES_XML_PARTICLESSECOND);

        Vector4 v1_v2 = loadVector4(node, M3D_PARTICLES_XML_UV1_UV2);
        Vector4 v3_v4 = loadVector4(node, M3D_PARTICLES_XML_UV3_UV4);

        uv1.u = v1_v2.getX();
        uv1.v = v1_v2.getY();
        uv2.u = v1_v2.getZ();
        uv2.v = v1_v2.getW();
        uv3.u = v3_v4.getX();
        uv3.v = v3_v4.getY();
        uv4.u = v3_v4.getZ();
        uv4.v = v3_v4.getW();

        distortion          = loadBool(node,      M3D_PARTICLES_XML_DISTORTION);
        distortionType      = (DISTORTION)loadInt(node,      M3D_PARTICLES_XML_DISTORTIONTYPE);
        distortionFrequency = loadFloat(node,     M3D_PARTICLES_XML_DISTORTIONFREQUENCY);
        distortionFactor    = loadFloat(node,     M3D_PARTICLES_XML_DISTORTIONFACTOR);
        distortionWave      = loadFloat(node,     M3D_PARTICLES_XML_DISTORTIONWAVE);

        if (loadInt(node, M3D_PARTICLES_XML_PLAYING) == 1)
        {
            play();
        }
    }
}

void Magic3D::Particles::save(std::string fileName)
{
    if (!fileName.empty())
    {
        XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
        doc.LinkEndChild( decl );

        XMLElement* root = doc.NewElement( "MGE" );
        doc.LinkEndChild( root );

        XMLComment* comment = doc.NewComment(" MGE Particles ");
        root->LinkEndChild( comment );

        if (getMeshes()->at(0))
        {
            getMeshes()->at(0)->save(root);
        }
        saveNode(root);

        doc.SaveFile( fileName.c_str() );
    }
}

void Magic3D::Particles::load(std::string fileName)
{
    stop();
    needTransform = true;

    std::string path = ResourceManager::getInstance()->getPath() + M3D_PATH_PARTICLES + fileName.c_str();
    XMLDocument* doc = new XMLDocument();

    if (doc->LoadFile(path.c_str()) == XML_SUCCESS)
    {
        XMLElement* pElem = doc->FirstChildElement();

        // should always have a valid root but handle gracefully if it does
        if (pElem)
        {
            if (getMeshes()->at(0))
            {
                getMeshes()->at(0)->load(pElem);
            }
            loadNode(pElem);
        }
    }

    if (doc)
    {
        delete doc;
        doc = NULL;
    }
}
