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

#include <magic3d/tween/tween.h>
#include <magic3d/magic3d.h>

Magic3D::Tween::Tween(const Tween &tween)
{
    this->tweenType = tween.tweenType;
    this->repeat = tween.repeat;
    this->startValue = tween.startValue;
    this->speed = tween.speed;
    this->duration = tween.duration;

    this->timeCount = tween.timeCount;
    this->repeatCount = tween.repeatCount;

    this->tStart = tween.tStart;
    this->tSpeed = tween.tSpeed;
    this->tDuration = tween.tDuration;
    this->tTime = tween.tTime;

    this->allwaysUpdate = tween.allwaysUpdate;
    this->playAtStart = tween.playAtStart;

    this->playing = tween.playing;

    this->tweenUpdateType = tween.tweenUpdateType;

    this->nextTweenObject = tween.nextTweenObject;
    this->nextTweenBone = tween.nextTweenBone;
    this->nextTweenIndex = tween.nextTweenIndex;
    this->nextTween = tween.nextTween;

    this->physicsObject = tween.physicsObject;
}

Magic3D::Tween::Tween(TWEEN tweenType)
{
    this->tweenType = tweenType;
    repeat = 0;
    startValue = 0.0f;
    speed = 1.0f;
    duration = 1.0f;

    timeCount = 0.0f;
    repeatCount = 0;

    tStart = 0.0f;
    tSpeed = 0.0f;
    tDuration = 0.0f;
    tTime = 0.0f;

    allwaysUpdate = false;
    playAtStart = false;

    playing = -1;

    tweenUpdateType = eTween_Simple_Linear;
    nextTweenObject.clear();
    nextTweenBone.clear();
    nextTweenIndex = 0;
    nextTween = NULL;

    physicsObject = NULL;
}

Magic3D::Tween::~Tween()
{
    if (nextTween)
    {
        nextTween->removeReference(this);
        nextTween = NULL;
    }
    physicsObject = NULL;
}

Magic3D::Tween* Magic3D::Tween::spawn()
{
    return (new Tween(*this));
}

Magic3D::TWEEN Magic3D::Tween::getType()
{
    return tweenType;
}

bool Magic3D::Tween::update()
{
    if (playing < 1 || (getPhysicsObject() && !allwaysUpdate  && !getPhysicsObject()->getAsObject()->isInParentFrustum()))
    {
        return false;
    }

    timeCount += Magic3D::getInstance()->getElapsedTime();
    float factor = 0.0f;

    if (timeCount > duration)
    {
        repeatCount++;

        tTime = repeatCount % 2 == 0 ? 0.0f : duration;

        factor = calculateTween();

        timeCount = 0.0f;

        if (repeatCount > repeat)
        {
            stop();
        }
    }
    else
    {
        tTime = repeatCount % 2 == 0 ? timeCount : duration - timeCount;

        factor = calculateTween();
    }

    tween(factor);

    if (playing < 1 && nextTween)
    {
        nextTween->play();
    }

    return true;
}

void Magic3D::Tween::play()
{
    if (!nextTweenObject.empty() && !nextTween)
    {
        playing = -1;
        setNextTween(nextTweenObject, nextTweenBone, nextTweenIndex);
    }
    if (playing == -1)
    {
        reset();
    }
    playing = 1;
}

void Magic3D::Tween::pause()
{
    playing = 0;
}

void Magic3D::Tween::stop()
{
    playing = -1;
}

bool Magic3D::Tween::isPlaying()
{
    bool result = playing == 1;

    if (!result && nextTween != NULL)
    {
        result = nextTween->isPlaying();
    }

    return result;
}

void Magic3D::Tween::reset()
{
    tStart = startValue;
    tSpeed = speed;
    tDuration = duration;
    repeatCount = 0;
    timeCount = 0.0f;
}

void Magic3D::Tween::setStartValue(float value)
{
    startValue = value;
}

float Magic3D::Tween::getStartValue()
{
    return startValue;
}

void Magic3D::Tween::setSpeed(float speed)
{
    this->speed = speed;
}

float Magic3D::Tween::getSpeed()
{
    return speed;
}

void Magic3D::Tween::setDuration(float duration)
{
    this->duration = duration;
}

float Magic3D::Tween::getDuration()
{
    return duration;
}

void Magic3D::Tween::setRepeat(int repeat)
{
    this->repeat = repeat;
}

int Magic3D::Tween::getRepeat()
{
    return repeat;
}

void Magic3D::Tween::setAllwaysUpdate(bool allways)
{
    allwaysUpdate = allways;
}

bool Magic3D::Tween::isAllwaysUpdated()
{
    return allwaysUpdate;
}

void Magic3D::Tween::setPlayingAtStart(bool playing)
{
    playAtStart = playing;
}

bool Magic3D::Tween::isPlayingAtStart()
{
    return playAtStart;
}

void Magic3D::Tween::setPhysicsObject(PhysicsObject* physicsObject)
{
    this->physicsObject = physicsObject;
}

Magic3D::PhysicsObject* Magic3D::Tween::getPhysicsObject()
{
    return physicsObject;
}

void  Magic3D::Tween::setTweenUpdateType(TweenUpdateType type)
{
    tweenUpdateType = type;
}

Magic3D::TweenUpdateType Magic3D::Tween::getTweenUpdateType()
{
    return tweenUpdateType;
}

void Magic3D::Tween::setNextTween(std::string object, std::string bone, int index)
{
    bool result = false;
    PhysicsObject* obj = ResourceManager::getObjects()->get(object);
    if (!object.empty() && obj)
    {
        if (!bone.empty() && obj->getType() == eOBJECT_MODEL)
        {
            obj = static_cast<Model*>(obj)->getSkeleton(true)->getBoneByName(bone);
        }
        if (obj && index >= 0 && index < obj->getTweenCount())
        {
            nextTweenObject = object;
            nextTweenBone = bone;
            nextTweenIndex = index;
            setNextTween(obj->getTween(index));
            result = true;
        }
    }

    if (!result)
    {
        nextTweenObject.clear();
        nextTweenBone.clear();
        nextTweenIndex = 0;
        nextTween = NULL;
    }
}

const std::string& Magic3D::Tween::getNextTweenObject()
{
    return nextTweenObject;
}

const std::string& Magic3D::Tween::getNextTweenBone()
{
    return nextTweenBone;
}

int Magic3D::Tween::getNextTweenIndex()
{
    return nextTweenIndex;
}

void Magic3D::Tween::setNextTween(Tween* tween)
{
    if (nextTween)
    {
        nextTween->removeReference(this);
    }

    nextTween = tween;

    if (nextTween && tween != this)
    {
        nextTween->addReference(this);
    }
}

Magic3D::Tween* Magic3D::Tween::getNextTween()
{
    return nextTween;
}

void Magic3D::Tween::tween(float factor)
{
    if (factor > 0.0f)
    {

    }
}

float Magic3D::Tween::calculateTween()
{
    if (duration == 0.0f)
    {
        return 1.0f;
    }

    float t = tTime;
    float b = tStart;
    float c = tSpeed;
    float d = tDuration;
    float result = 0.0f;

    switch (tweenUpdateType)
    {
        case eTween_Simple_Linear:
        {
            result = c * t / d + b;
            break;
        };

        case eTween_Quadratic_Easing_In:
        {
            t /= d;
            result = c * t * t + b;
            break;
        };

        case eTween_Quadratic_Easing_Out:
        {
            t /= d;
            result = -c * t * (t - 2.0f) + b;
            break;
        };

        case eTween_Quadratic_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = c / 2.0f * t * t + b;
            }
            else
            {
                t--;
                result = -c / 2.0f * (t * (t - 2.0f) - 1.0f) + b;
            }
            break;
        };

        case eTween_Cubic_Easing_In:
        {
            t /= d;
            result = c * t * t * t + b;
            break;
        };

        case eTween_Cubic_Easing_Out:
        {
            t /= d;
            t--;
            result = c * (t * t * t + 1.0f) + b;
            break;
        };

        case eTween_Cubic_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = c / 2.0f * t * t * t + b;
            }
            else
            {
                t -= 2.0f;
                result = c / 2.0f * (t * t * t + 2.0f) + b;
            }
            break;
        };

        case eTween_Quartic_Easing_In:
        {
            t /= d;
            result = c * t * t * t * t + b;
            break;
        };

        case eTween_Quartic_Easing_Out:
        {
            t /= d;
            t--;
            result = -c * (t * t * t * t - 1.0f) + b;
            break;
        };

        case eTween_Quartic_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = c / 2.0f * t * t * t * t + b;
            }
            else
            {
                t -= 2.0f;
                result = -c / 2.0f * (t * t * t * t - 2.0f) + b;
            }
            break;
        };

        case eTween_Quintic_Easing_In:
        {
            t /= d;
            result = c * t * t * t * t * t + b;
            break;
        };

        case eTween_Quintic_Easing_Out:
        {
            t /= d;
            t--;
            result = c * (t * t * t * t * t + 1.0f) + b;
            break;
        };

        case eTween_Quintic_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = c / 2.0f * t * t * t * t * t + b;
            }
            else
            {
                t -= 2.0f;
                result = c / 2.0f * (t * t * t * t * t + 2.0f) + b;
            }
            break;
        };


        case eTween_Sinusoidal_Easing_In:
        {
            result = -c * cos(t / d * (M_PI / 2.0f)) + c + b;
            break;
        };

        case eTween_Sinusoidal_Easing_Out:
        {
            result = c * sin(t / d * (M_PI / 2.0f)) + b;
            break;
        };

        case eTween_Sinusoidal_Easing_In_Out:
        {
            result = -c / 2.0f * (cos(M_PI * t / d) - 1.0f) + b;
            break;
        };

        case eTween_Exponential_Easing_In:
        {
            result = c * pow( 2.0f, 10.0f * (t / d - 1.0f) ) + b;
            break;
        };

        case eTween_Exponential_Easing_Out:
        {
            result = c * ( -pow( 2.0f, -10.0f * t / d ) + 1.0f ) + b;
            break;
        };

        case eTween_Exponential_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = c / 2.0f * pow( 2.0f, 10.0f * (t - 1.0f) ) + b;
            }
            else
            {
                t--;
                result = c / 2.0f * ( -pow( 2.0f, -10.0f * t) + 2.0f ) + b;
            }
            break;
        };

        case eTween_Circular_Easing_In:
        {
            t /= d;
            result = -c * (sqrt(1.0f - t * t) - 1.0f) + b;
            break;
        };

        case eTween_Circular_Easing_Out:
        {
            t /= d;
            t--;
            result = c * sqrt(1.0f - t * t) + b;
            break;
        };

        case eTween_Circular_Easing_In_Out:
        {
            t /= d / 2.0f;
            if (t < 1.0f)
            {
                result = -c / 2.0f * (sqrt(1.0f - t * t) - 1.0f) + b;
            }
            else
            {
                t -= 2.0f;
                result = c / 2.0f * (sqrt(1.0f - t * t) + 1.0f) + b;
            }
            break;
        };

        case eTween_Elastic_Easing_In:
        {
            float fator = 0.0f;
            if (t == 0.0f)
            {
                result = b;
            }
            else
            {
                float td  = t / d;
                t = t / d;
                if (td == 1.0f)
                {
                    result = b + c;
                }
                else
                {
                    float p = d * 0.3f;

                    float a = 0.0f;
                    if (a == 0.0f || (c > 0.0f && a < c) || (c < 0.0f && a < -c))
                    {
                        a = c;
                        fator = p / 4.0f;
                    }
                    else
                    {
                        fator = p / (M_PI * 2.0f) * asin(c / a);
                    }
                    /*t = t - 1.0f; NOT TURN ON

                    result = -a * pow(2.0f, 10.0f * --t) * sin((t * d - fator) * (M_PI * 2.0f) / p) + b;*/
                    t = t - 1.0f;
                    result = -a * pow(2.0f, 10.0f * t) * sin((t * d - fator) * (M_PI * 2.0f) / p) + b;
                }
            }
            break;
        }

        case eTween_Elastic_Easing_Out:
        {
            float fator = 0.0f;
            if (t == 0.0f)
            {
                result = b;
            }
            else
            {
                float td = t / d;
                t = t / d;
                if (td == 1.0f)
                {
                    result = b + c;
                }
                else
                {
                    float p = d * 0.3f;

                    float a = 0.0f;
                    if (a == 0.0f || (c > 0.0f && a < c) || (c < 0.0f && a < -c))
                    {
                        a = c;
                        fator = p / 4.0f;
                    }
                    else
                    {
                        fator = p / (M_PI * 2.0f) * asin(c / a);
                    }

                    result = a * pow(2.0f, -10.0f * t) * sin((t * d - fator) * (M_PI * 2.0f) / p) + c + b;
                }
            }
            break;
        }

        case eTween_Elastic_Easing_In_Out:
        {
            float fator = 0.0f;
            if (t == 0.0f)
            {
                result = b;
            }
            else
            {
                float td = t / (d * 0.5f);
                t = t / (d * 0.5f);
                if (td == 2.0f)
                {
                    result = b + c;
                }
                else
                {
                    float p = d * (0.3f * 1.5f);

                    float a = 0.0f;
                    if (a == 0.0f || (c > 0.0f && a < c) || (c < 0.0f && a < -c))
                    {
                        a = c;
                        fator = p / 4.0f;
                    }
                    else
                    {
                        fator = p / (M_PI * 2.0f) * asin(c / a);
                    }

                    if (t < 1.0f)
                    {
                        /*t = t - 1.0f; //NOT TURN ON
                        result = -0.5f * (a * pow(2.0f, 10.0f * --t) * sin((t * d - fator) * (M_PI * 2.0f) / p)) + b;*/
                        t = t - 1.0f;
                        result = -0.5f * (a * pow(2.0f, 10.0f * t) * sin((t * d - fator) * (M_PI * 2.0f) / p)) + b;
                    }
                    else
                    {
                        /*t = t - 1.0f; NOT TURN ON
                        result = a * pow(2.0f, -10.0f * --t) * sin((t * d - fator) * (M_PI * 2.0f) / p) * 0.5f + c + b;*/
                        t = t - 1.0f;
                        result = a * pow(2.0f, -10.0f * t) * sin((t * d - fator) * (M_PI * 2.0f) / p) * 0.5f + c + b;
                    }
                }
            }
            break;
        }
    }

    return result;
}

Magic3D::XMLElement* Magic3D::Tween::save(XMLElement* root)
{
    XMLElement* result = NULL;
    if (root)
    {
        result = root->GetDocument()->NewElement(M3D_TWEEN_XML);
        root->LinkEndChild(result);

        saveInt(result,    M3D_TWEEN_XML_TYPE,           tweenType);
        saveBool(result,   M3D_TWEEN_XML_ALLWAYS_UPDATE, allwaysUpdate);
        saveBool(result,   M3D_TWEEN_XML_PLAYING,        playAtStart);
        saveFloat(result,  M3D_TWEEN_XML_START_VALUE,    startValue);
        saveFloat(result,  M3D_TWEEN_XML_SPEED,          speed);
        saveFloat(result,  M3D_TWEEN_XML_DURATION,       duration);
        saveInt(result,    M3D_TWEEN_XML_REPEAT,         repeat);
        saveInt(result,    M3D_TWEEN_XML_UPDATE_TYPE,    tweenUpdateType);
        saveString(result, M3D_TWEEN_XML_NEXT_OBJECT,    nextTweenObject);
        saveString(result, M3D_TWEEN_XML_NEXT_BONE,      nextTweenBone);
        saveInt(result,    M3D_TWEEN_XML_NEXT_INDEX,     nextTweenIndex);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::Tween::load(XMLElement* root)
{
    if (root)
    {
        tweenType       = (TWEEN)loadInt(root,           M3D_TWEEN_XML_TYPE);
        allwaysUpdate   = loadBool(root,                 M3D_TWEEN_XML_ALLWAYS_UPDATE);
        playAtStart     = loadBool(root,                 M3D_TWEEN_XML_PLAYING);
        startValue      = loadFloat(root,                M3D_TWEEN_XML_START_VALUE);
        speed           = loadFloat(root,                M3D_TWEEN_XML_SPEED);
        duration        = loadFloat(root,                M3D_TWEEN_XML_DURATION);
        repeat          = loadInt(root,                  M3D_TWEEN_XML_REPEAT);
        tweenUpdateType = (TweenUpdateType)loadInt(root, M3D_TWEEN_XML_UPDATE_TYPE);
        nextTweenObject = loadString(root,               M3D_TWEEN_XML_NEXT_OBJECT);
        if (nextTweenObject.compare(M3D_XML_NULL) == 0)
        {
            nextTweenObject.clear();
        }
        nextTweenBone = loadString(root,               M3D_TWEEN_XML_NEXT_BONE);
        if (nextTweenBone.compare(M3D_XML_NULL) == 0)
        {
            nextTweenBone.clear();
        }
        nextTweenIndex  = loadInt(root,                  M3D_TWEEN_XML_NEXT_INDEX);
    }
    return root;
}

void Magic3D::Tween::removeReference(Xml* reference)
{
    Xml::removeReference(reference);
    if (reference)
    {
        stop();
        if (nextTween)
        {
            if (nextTween->hasReference(this))
            {
                nextTween->removeReference(this);
            }
            nextTween = NULL;
        }
    }        
}
