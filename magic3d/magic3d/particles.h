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

#ifndef MAGIC3D_PARTICLES_H
#define MAGIC3D_PARTICLES_H

#include <magic3d/object.h>

namespace Magic3D
{
#define PARTICLES_MESH_NAME    "particles"

#define M3D_PARTICLES_XML                     "Particles"
#define M3D_PARTICLES_XML_PLAYING             "playing"
#define M3D_PARTICLES_XML_LOOP                "loop"
#define M3D_PARTICLES_XML_RECREATEWHENDIE     "recreate_when_die"
#define M3D_PARTICLES_XML_FOLLOWEMITTER       "follow_emitter"
#define M3D_PARTICLES_XML_CREATIONDIRECTION   "creation_direction"
#define M3D_PARTICLES_XML_INVERTEDDIRECTION   "inverted_direction"
#define M3D_PARTICLES_XML_MINSPEED            "min_speed"
#define M3D_PARTICLES_XML_MAXSPEED            "max_speed"
#define M3D_PARTICLES_XML_MINACCELERATION     "min_acceleration"
#define M3D_PARTICLES_XML_MAXACCELERATION     "max_acceleration"
#define M3D_PARTICLES_XML_MINAGE              "min_age"
#define M3D_PARTICLES_XML_MAXAGE              "max_age"
#define M3D_PARTICLES_XML_MINEMITSIZE         "min_emit_size"
#define M3D_PARTICLES_XML_MAXEMITSIZE         "max_emit_size"
#define M3D_PARTICLES_XML_MINDIESIZE          "min_die_size"
#define M3D_PARTICLES_XML_MAXDIESIZE          "max_die_size"
#define M3D_PARTICLES_XML_MINCREATIONDISTANCE "min_creation_distance"
#define M3D_PARTICLES_XML_MAXCREATIONDISTANCE "max_creation_distance"
#define M3D_PARTICLES_XML_ACCELERATION        "acceleration"
#define M3D_PARTICLES_XML_MINSPINSPEED        "min_spin_speed"
#define M3D_PARTICLES_XML_MAXSPINSPEED        "max_spin_speed"
#define M3D_PARTICLES_XML_MINSPINACCELERATION "min_spin_acceleration"
#define M3D_PARTICLES_XML_MAXSPINACCELERATION "max_spin_acceleration"
#define M3D_PARTICLES_XML_MINRADIUS           "min_radius"
#define M3D_PARTICLES_XML_MAXRADIUS           "max_radius"
#define M3D_PARTICLES_XML_MINEMITCOLOR        "min_emit_color"
#define M3D_PARTICLES_XML_MAXEMITCOLOR        "max_emit_color"
#define M3D_PARTICLES_XML_MINDIECOLOR         "min_die_color"
#define M3D_PARTICLES_XML_MAXDIECOLOR         "max_die_color"
#define M3D_PARTICLES_XML_BILLBOARD           "billboard_particle"
#define M3D_PARTICLES_XML_MINPARTICLESCOUNT   "min_particles_count"
#define M3D_PARTICLES_XML_MAXPARTICLESCOUNT   "max_particles_count"
#define M3D_PARTICLES_XML_PARTICLESSECOND     "particles_second"
#define M3D_PARTICLES_XML_UV1_UV2             "uv1_uv2"
#define M3D_PARTICLES_XML_UV3_UV4             "uv3_uv4"
#define M3D_PARTICLES_XML_TRAIL               "trail"
#define M3D_PARTICLES_XML_DISTORTION          "distortion"
#define M3D_PARTICLES_XML_DISTORTIONTYPE      "distortion_type"
#define M3D_PARTICLES_XML_DISTORTIONFREQUENCY "distortion_frequency"
#define M3D_PARTICLES_XML_DISTORTIONFACTOR    "distortion_factor"
#define M3D_PARTICLES_XML_DISTORTIONWAVE      "distortion_wave"

struct Particle
{
    Particle() {position = Vector3(0.0f, 0.0f, 0.0f), direction = Vector3(0.0f, 0.0f, 0.0f), dieAge = 0.0f; age = 0.0f; size = 0.0f; sizeFactor = 0.0f; angle = 0.0f; color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f); alive = false;}

    ColorRGBA dieColor;
    ColorRGBA color;
    ColorRGBA colorFactor;

    Vector3 position;
    Vector3 direction;

    float dieAge;
    float age;
    float dieSize;
    float size;
    float sizeFactor;
    float angle;

    bool alive;
};

class Particles : public Object
{
private:
    std::vector<Particle> particles;

protected:
    Mesh* particlesMesh;

private:
    Box box;

    ColorRGBA minEmitColor;
    ColorRGBA maxEmitColor;

    ColorRGBA minDieColor;
    ColorRGBA maxDieColor;

    Vector3 worldPosition;
    Vector3 minCreationDistance;
    Vector3 maxCreationDistance;

    Vector3 acceleration;

    Texture2D uv1;
    Texture2D uv2;
    Texture2D uv3;
    Texture2D uv4;

    float second;

    float minSpeed;
    float maxSpeed;

    float minAcceleration;
    float maxAcceleration;

    float minAge;
    float maxAge;

    float minEmitSize;
    float maxEmitSize;

    float minDieSize;
    float maxDieSize;

    float minSpinSpeed;
    float maxSpinSpeed;

    float minSpinAcceleration;
    float maxSpinAcceleration;

    float minRadius;
    float maxRadius;

    float distortionFrequency;
    float distortionFactor;
    float distortionWave;

    int minParticlesCount;
    int maxParticlesCount;
    int particlesSecond;

    int particlesCount;
    int particlesCreated;

    BILLBOARD billboardParticle;
    DISTORTION distortionType;

    bool playing;
    bool needStop;
    bool loop;

    bool recreateWhenDie;
    bool followEmitter;
    bool stopCreating;
    bool creationDirection;
    bool invertedDirection;

    bool trail;

    bool uniqueUpdate;

    bool distortion;

    void createParticle(Particle* particle, bool reset);
    void updateBoundingBox(Vector3& v1, Vector3& v2, Vector3& v3, Vector3& v4, bool reset);

protected:

    void init();
    virtual void updateMatrix();

    Particles(const Particles& particles, std::string name);

public:
    Particles(std::string name);
    virtual ~Particles();
    virtual Particles* spawn(std::string name) const;

    virtual bool update();
    virtual bool updateMeshes();
    void addMaterial(Material* material);

    virtual const Box& getBoundingBox();
    virtual Matrix4 getMatrixFromParent();

    const Vector3& getWorldPosition();

    void play();
    void stop();
    bool isPlaying();

    void setLooping(bool loop);
    bool isLooping();

    void setRecreateWhenDie(bool recreate);
    bool isRecreatedWhenDie();

    void setFollowEmitter(bool follow);
    bool isFollowingEmitter();

    void setUseCreationDirection(bool creation);
    bool isUsingCreationDirection();

    void setInvertedDirection(bool inverted);
    bool isInvertedDirection();

    void setMinSpeed(float speed);
    float getMinSpeed();

    void setMaxSpeed(float speed);
    float getMaxSpeed();

    void setMinAcceleration(float acceleration);
    float getMinAcceleration();

    void setMaxAcceleration(float acceleration);
    float getMaxAcceleration();

    void setMinAge(float age);
    float getMinAge();

    void setMaxAge(float age);
    float getMaxAge();

    void setMinEmitSize(float size);
    float getMinEmitSize();

    void setMaxEmitSize(float size);
    float getMaxEmitSize();

    void setMinDieSize(float size);
    float getMinDieSize();

    void setMaxDieSize(float size);
    float getMaxDieSize();

    void setMinSpinSpeed(float speed);
    float getMinSpinSpeed();

    void setMaxSpinSpeed(float speed);
    float getMaxSpinSpeed();

    void setMinSpinAcceleration(float acceleration);
    float getMinSpinAcceleration();

    void setMaxSpinAcceleration(float acceleration);
    float getMaxSpinAcceleration();

    void setMinRadius(float radius);
    float getMinRadius();

    void setMaxRadius(float radius);
    float getMaxRadius();

    void setMinCreationDistance(Vector3 distance);
    const Vector3& getMinCreationDistance();

    void setMaxCreationDistance(Vector3 distance);
    const Vector3& getMaxCreationDistance();

    void setAcceleration(Vector3 acceleration);
    const Vector3& getAcceleration();

    void setMinEmitColor(ColorRGBA color);
    const ColorRGBA& getMinEmitColor();

    void setMaxEmitColor(ColorRGBA color);
    const ColorRGBA& getMaxEmitColor();

    void setMinDieColor(ColorRGBA color);
    const ColorRGBA& getMinDieColor();

    void setMaxDieColor(ColorRGBA color);
    const ColorRGBA& getMaxDieColor();

    void setBillboardParticle(BILLBOARD billboard);
    BILLBOARD getBillboardParticle();

    void setMinParticlesCount(int count);
    int getMinParticlesCount();

    void setMaxParticlesCount(int count);
    int getMaxParticlesCount();

    void setParticlesSecond(int count);
    int getParticlesSecond();

    void setUV1(Texture2D uv);
    const Texture2D& getUV1();

    void setUV2(Texture2D uv);
    const Texture2D& getUV2();

    void setUV3(Texture2D uv);
    const Texture2D& getUV3();

    void setUV4(Texture2D uv);
    const Texture2D& getUV4();

    void setTrail(bool trail);
    bool isTrail();

    void setDistorting(bool distort);
    bool isDistorting();
    void setDistortionType(DISTORTION type);
    DISTORTION getDistortiionType();
    void setDistortionFrequency(float value);
    float getDistortionFrequency();
    void setDistortionFactor(float value);
    float getDistortionFactor();
    void setDistortionWave(float value);
    float getDistortionWave();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);

    void saveNode(XMLElement* root);
    void loadNode(XMLElement* root);

    void save(std::string fileName);
    void load(std::string fileName);
};

}

#endif // MAGIC3D_PARTICLES_H
