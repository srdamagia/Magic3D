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

#ifndef MAGIC3D_ANIMATION_H
#define MAGIC3D_ANIMATION_H

#include <magic3d/model/skeleton.h>
#include <magic3d/xml.h>

namespace Magic3D
{

#define M3D_ANIMATION_XML                  "Animation"
#define M3D_ANIMATION_XML_SEQUENCES        "sequences"
#define M3D_ANIMATION_SEQUENCE_XML         "AnimationSequence"
#define M3D_ANIMATION_XML_CURRENT_SEQUENCE "current_sequence"

#define M3D_ANIMATION_SEQUENCE_XML_NAME  "name"
#define M3D_ANIMATION_SEQUENCE_XML_SPEED "speed"
#define M3D_ANIMATION_SEQUENCE_XML_START "start"
#define M3D_ANIMATION_SEQUENCE_XML_END   "end"
#define M3D_ANIMATION_SEQUENCE_XML_LOOP  "loop"

struct AnimationBone
{
    int boneIndex;
    Quaternion rotation;
    Vector3 position;
};

struct AnimationFrame
{
    std::vector<AnimationBone> bones;
    float time;
};

class AnimationSequence
{
private:
    std::string name;

    float speed;
    int startFrame;
    int endFrame;

    bool loop;

public:
    AnimationSequence(std::string name);
    ~AnimationSequence();

    const std::string& getName();

    void setSpeed(float speed);
    float getSpeed();
    void setStartFrame(int frame);
    int getStartFrame();
    void setEndFrame(int frame);
    int getEndFrame();

    int getFrameCount();

    void setLooping(bool loop);
    bool isLooping();
};

class AnimationSequences;
class Animation : public Xml
{
private:
    std::vector<AnimationFrame> frames;
    std::vector<AnimationBone> lastFrame;
    AnimationSequences* sequences;
    //AnimationSequence* sequence;
    //AnimationSequence* nextSequence;
    Skeleton* skeleton;

    float elapsed;
    float elapsedUpdate;
    float speed;
    float speedUpdate;
    float nextAnimationTime;

    int frame;
    int sequenceIndex;
    int nextSequenceIndex;

    bool reverse;
    bool nextReverse;
    bool playing;
    bool playingNext;
    bool needUpdate;

    bool updateSequence();

protected:
    Animation(const Animation& animation, Skeleton* skeleton);

public:
    Animation(Skeleton* skeleton);
    virtual ~Animation();
    virtual Animation* spawn(Skeleton* skeleton) const;

    virtual bool update();

    void addBone(float time, int bone, Matrix4 matrix);
    bool removeFrame(float time);

    void play(bool reverse = false);
    void playAnimation(std::string animation, float interpolation, bool reverse = false);
    void stop();
    bool isPlaying();
    bool isReverse();

    void clear();

    int getCurrentFrame();
    void setCurrentFrame(int frame);
    AnimationFrame* getFrame(int frame);
    std::vector<AnimationFrame>* getFrames();
    int getFrameCount();

    void addSequence(std::string name, float speed, int startFrame, int endFrame, bool loop);
    void removeSequence(std::string name);
    void setSequences(AnimationSequences *sequences);
    std::vector<AnimationSequence*>* getSequences();
    AnimationSequence* getSequence(std::string name);
    int getSequenceIndex(std::string name);
    std::string getSequenceName(int index);

    void setCurrentSequence(std::string name);
    void setCurrentSequence(int index);
    AnimationSequence* getCurrentSequence();
    AnimationSequence* getNextSequence();
    int getCurrentSequenceIndex();
    std::string getCurrentSequenceName();

    Skeleton* getSkeleton();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // ANIMATION_H
