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

#ifndef MAGIC3D_TWEEN_H
#define MAGIC3D_TWEEN_H

#include <magic3d/physics/physics_object.h>

namespace Magic3D
{
#define M3D_TWEEN_XML                "Tween"
#define M3D_TWEEN_XML_TYPE           "type"
#define M3D_TWEEN_XML_PLAYING        "playing"
#define M3D_TWEEN_XML_START_VALUE    "start_value"
#define M3D_TWEEN_XML_SPEED          "speed"
#define M3D_TWEEN_XML_DURATION       "duration"
#define M3D_TWEEN_XML_REPEAT         "repeat"
#define M3D_TWEEN_XML_UPDATE_TYPE    "update_type"
#define M3D_TWEEN_XML_NEXT_OBJECT    "next_object"
#define M3D_TWEEN_XML_NEXT_BONE      "next_bone"
#define M3D_TWEEN_XML_NEXT_INDEX     "next_index"
#define M3D_TWEEN_XML_ALLWAYS_UPDATE "allways_update"

enum TWEEN
{
    eTWEEN_ALPHA,
    eTWEEN_DELAY,
    eTWEEN_ROTATE,
    eTWEEN_SCALE,
    eTWEEN_TRANSLATE,
    eTWEEN_GOTO,
    eTWEEN_LOOKAT,
    eTWEEN_CUSTOM
};

enum TweenUpdateType
{
    eTween_Circular_Easing_In,
    eTween_Circular_Easing_In_Out,
    eTween_Circular_Easing_Out,
    eTween_Cubic_Easing_In,
    eTween_Cubic_Easing_In_Out,
    eTween_Cubic_Easing_Out,
    eTween_Elastic_Easing_In,
    eTween_Elastic_Easing_In_Out,
    eTween_Elastic_Easing_Out,
    eTween_Exponential_Easing_In,
    eTween_Exponential_Easing_In_Out,
    eTween_Exponential_Easing_Out,
    eTween_Quadratic_Easing_In,
    eTween_Quadratic_Easing_In_Out,
    eTween_Quadratic_Easing_Out,
    eTween_Quartic_Easing_In,
    eTween_Quartic_Easing_In_Out,
    eTween_Quartic_Easing_Out,
    eTween_Quintic_Easing_In,
    eTween_Quintic_Easing_In_Out,
    eTween_Quintic_Easing_Out,
    eTween_Simple_Linear,
    eTween_Sinusoidal_Easing_In,
    eTween_Sinusoidal_Easing_In_Out,
    eTween_Sinusoidal_Easing_Out
};

class Tween : public Xml
{
private:
    float startValue;
    float speed;
    float duration;

    float timeCount;
    float tStart;
    float tSpeed;
    float tDuration;
    float tTime;

    int repeat;

    bool allwaysUpdate;
    bool playAtStart;

    PhysicsObject* physicsObject;

    TweenUpdateType tweenUpdateType;
    std::string nextTweenObject;
    std::string nextTweenBone;
    int nextTweenIndex;
    Tween* nextTween;

    float calculateTween();

protected:
    TWEEN tweenType;
    int repeatCount;
    int playing;

    Tween(const Tween &tween);
    virtual void tween(float factor);

public:
    Tween(TWEEN tweenType);
    virtual ~Tween();

    virtual Tween* spawn();

    virtual TWEEN getType();

    bool update();

    void play();
    void pause();
    void stop();

    bool isPlaying();

    virtual void reset();

    void setStartValue(float value);
    float getStartValue();

    void setSpeed(float speed);
    float getSpeed();

    void setDuration(float duration);
    float getDuration();

    void setRepeat(int repeat);
    int getRepeat();

    void setAllwaysUpdate(bool allways);
    bool isAllwaysUpdated();

    void setPlayingAtStart(bool playing);
    bool isPlayingAtStart();

    void setPhysicsObject(PhysicsObject* physicsObject);
    PhysicsObject* getPhysicsObject();

    void  setTweenUpdateType(TweenUpdateType type);
    TweenUpdateType getTweenUpdateType();

    void setNextTween(std::string object, std::string bone, int index);
    const std::string& getNextTweenObject();
    const std::string& getNextTweenBone();
    int getNextTweenIndex();

    void setNextTween(Tween* tween);
    Tween* getNextTween();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);

    virtual void removeReference(Xml* reference);
};
}

#endif // MAGIC3D_TWEEN_H
