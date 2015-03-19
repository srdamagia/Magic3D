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

bool sortFrames (Magic3D::AnimationFrame a, Magic3D::AnimationFrame b) { return (a.time < b.time); }

Magic3D::AnimationSequence::AnimationSequence(std::string name)
{
    speed        = 0.0f;
    startFrame   = 0;
    endFrame     = 0;

    loop    = false;

    this->name = name;
}

Magic3D::AnimationSequence::~AnimationSequence()
{

}

const std::string& Magic3D::AnimationSequence::getName()
{
    return name;
}

void Magic3D::AnimationSequence::setSpeed(float speed)
{
    this->speed = speed;
}

float Magic3D::AnimationSequence::getSpeed()
{
    return speed;
}

void Magic3D::AnimationSequence::setStartFrame(int frame)
{
    startFrame = frame;
}

int Magic3D::AnimationSequence::getStartFrame()
{
    return startFrame;
}

void Magic3D::AnimationSequence::setEndFrame(int frame)
{
    endFrame = frame;
}

int Magic3D::AnimationSequence::getEndFrame()
{
    return endFrame;
}

int Magic3D::AnimationSequence::getFrameCount()
{
    return endFrame - startFrame;
}

void Magic3D::AnimationSequence::setLooping(bool loop)
{
    this->loop = loop;
}

bool Magic3D::AnimationSequence::isLooping()
{
    return loop;
}

//******************************************************************************
Magic3D::Animation::Animation(const Animation& animation, Skeleton* skeleton)
{
    this->frames = animation.frames;
    this->sequences = animation.sequences;
    this->sequence = animation.sequence;
    this->nextSequence = animation.nextSequence;

    this->elapsed = animation.elapsed;
    this->speed = animation.speed;
    this->elapsedUpdate = animation.elapsedUpdate;
    this->speedUpdate = animation.speedUpdate;
    this->nextAnimationTime = animation.nextAnimationTime;

    this->frame = animation.frame;

    this->reverse = animation.reverse;
    this->nextReverse = animation.nextReverse;
    this->playing = animation.playing;
    this->playingNext = animation.playingNext;
    this->needUpdate = animation.needUpdate;

    this->skeleton = skeleton;
}

Magic3D::Animation::Animation(Skeleton* skeleton)
{
    frame = -1;
    needUpdate = false;

    reverse = false;
    nextReverse = false;
    playing = false;
    playingNext = false;

    elapsed = 0.0f;
    speed = 0.0f;

    elapsedUpdate = 0.0f;
    speedUpdate = 1.0f / 30.0f;

    nextAnimationTime = 0.0f;

    sequence = NULL;
    nextSequence = NULL;
    sequences = NULL;
    this->skeleton = skeleton;
}

Magic3D::Animation::~Animation()
{
    clear();
}

Magic3D::Animation* Magic3D::Animation::spawn(Skeleton* skeleton) const
{
    return (new Animation(*this, skeleton));
}

bool Magic3D::Animation::updateSequence()
{
    bool result = (isPlaying() || nextSequence) && sequence && frame >= 0 && frame < getFrameCount();

    if (!result)
    {
        return false;
    }

    float et = Magic3D::Magic3D::getInstance()->getElapsedTime();
    elapsed += et * (playingNext ? 1.0f : speed);
    elapsedUpdate += et;

    if (elapsedUpdate > speedUpdate)
    {
        elapsedUpdate = 0.0f;
        if (playingNext && elapsed > nextAnimationTime)
        {
            sequence = nextSequence;
            nextSequence = NULL;
            playingNext = false;

            frame = sequence->getStartFrame();
            speed = sequence->getSpeed();
            reverse = nextReverse;
            nextReverse = false;
            nextAnimationTime = 0.0f;

            elapsed = et * speed;
            playing = true;
        }

        int nextFrame = frame;

        float startTime = getFrame(frame)->time;
        float endTime = startTime;

        if (!playingNext)
        {
            int skip = 0;
            while (nextFrame < sequence->getEndFrame())
            {
                nextFrame++;
                endTime = getFrame(nextFrame)->time;
                float diff = (endTime - startTime);
                if (elapsed > diff)
                {
                    skip++;
                    frame = nextFrame;
                }
                else
                {
                    break;
                }
            }

            if (skip > 0)
            {
                elapsed = 0.0f;
            }

            if (frame >= sequence->getEndFrame())
            {
                if (!sequence->isLooping())
                {
                    stop();
                }
                else
                {
                    frame = sequence->getStartFrame();
                    nextFrame = frame < sequence->getEndFrame() ? frame + 1 : frame;
                }
            }
        }

        if (!playingNext)
        {
            startTime = getFrame(frame)->time;
            endTime = getFrame(nextFrame)->time;
        }
        else
        {
            startTime = 0.0f;
            endTime = nextAnimationTime;
            nextFrame = nextSequence->getStartFrame();
        }

        float currentTime = 1.0f - (endTime - (startTime + elapsed)) / (endTime - startTime);

        std::vector<AnimationBone>& bonesFrame = playingNext ? lastFrame : getFrame(frame)->bones;
        std::vector<AnimationBone>& bonesNextFrame = getFrame(nextFrame)->bones;

        std::vector<AnimationBone>::iterator it_b = bonesFrame.begin();
        std::vector<AnimationBone>::iterator it_n = bonesNextFrame.begin();

        unsigned int index = 0;
        while (it_b != bonesFrame.end())
        {
            AnimationBone& ab = *it_b++;
            AnimationBone& an = *it_n++;

            Matrix4 interpolation;
            Quaternion rot;
            Vector3 pos;

            if (nextFrame == frame)
            {
                rot = an.rotation;
                pos = an.position;
                interpolation = Matrix4(rot, pos);
            }
            else
            {
                rot = slerp(currentTime, ab.rotation, an.rotation);
                pos = lerp(currentTime, ab.position, an.position);
                interpolation = Matrix4(rot, pos);
            }

            if (!playingNext)
            {
                if (index >= lastFrame.size())
                {
                    AnimationBone animBone;
                    animBone.rotation = rot;
                    animBone.position = pos;
                    animBone.boneIndex = ab.boneIndex;
                    lastFrame.push_back(animBone);
                }
                else
                {
                    lastFrame[index].rotation = rot;
                    lastFrame[index].position = pos;
                    lastFrame[index].boneIndex = ab.boneIndex;
                }
            }

            Bone* bone = getSkeleton()->getBone(ab.boneIndex);

            if (bone && bone->isAnimated())
            {
                bone->setMatrix(interpolation);
            }

            index++;
        }
    }
    return result;
}

bool Magic3D::Animation::update()
{
    bool result = false;

    if (!needUpdate && getCurrentSequence())
    {
        updateSequence();
    }
    else
    {
        if (needUpdate && frame >= 0 && frame < (int)frames.size())
        {
            std::vector<AnimationBone>& bones = frames[frame].bones;
            std::vector<AnimationBone>::iterator it_b = bones.begin();

            while (it_b != bones.end())
            {
                AnimationBone& ab = *it_b++;

                Matrix4 interpolation(ab.rotation, ab.position);

                Bone* bone = getSkeleton()->getBone(ab.boneIndex);
                if (bone && bone->isAnimated())
                {
                    bone->setMatrix(interpolation);
                }
            }

            needUpdate = false;

            result = true;
        }
    }
    return result;
}

void Magic3D::Animation::addBone(float time, int bone, Matrix4 matrix)
{
    std::vector<AnimationFrame>::iterator it_f = frames.begin();
    bool found = false;
    AnimationFrame* frame = &(*frames.begin());
    while(it_f != frames.end())
    {
        frame = &(*it_f++);

        if (frame->time == time)
        {
            found = true;
            break;
        }
    }

    AnimationBone b;
    b.boneIndex = bone;

    b.position = matrix.getTranslation();
    b.rotation = Quaternion(matrix.getUpper3x3());

    if (!found)
    {
        AnimationFrame f;
        f.time = time;

        f.bones.push_back(b);
        frames.push_back(f);
    }
    else
    {
        frame->bones.push_back(b);
    }

    sort(frames.begin(), frames.end(), sortFrames);
}

bool Magic3D::Animation::removeFrame(float time)
{
    bool result = false;

    std::vector<AnimationFrame>::iterator it_f = frames.begin();
    while(it_f != frames.end())
    {
        AnimationFrame& frame = *it_f;

        if (frame.time == time)
        {
            frames.erase(it_f);
            break;
        }

        it_f++;
    }

    return result;
}

void Magic3D::Animation::play(bool reverse)
{
    if (getCurrentSequence() && getCurrentSequence()->getFrameCount() > 0)
    {
        playing = true;
        playingNext = false;
        this->reverse = reverse;

        speed = getCurrentSequence()->getSpeed();

        frame = getCurrentSequence()->getStartFrame();
        elapsed = 0.0f;

        nextSequence = NULL;
        nextAnimationTime = 0.0f;
        nextReverse = false;

        elapsedUpdate = 0.0f;
    }
}

void Magic3D::Animation::playAnimation(std::string animation, float interpolation, bool reverse)
{
    if (!sequence || sequence->getName().compare(animation) != 0 || (nextSequence && nextSequence->getName().compare(animation) != 0))
    {
        if (!sequence || interpolation == 0.0f)
        {
            setCurrentSequence(animation);
            play();
        }
        else
        {
            elapsed = 0.0f;
            elapsedUpdate = 0.0f;
            playingNext = true;
            nextSequence = getSequence(animation);
            nextAnimationTime = interpolation;
            nextReverse = reverse;
        }
    }
}

void Magic3D::Animation::stop()
{
    playing = false;
}

bool Magic3D::Animation::isPlaying()
{
    return playing;
}

bool Magic3D::Animation::isReverse()
{
    return reverse;
}

void Magic3D::Animation::clear()
{
    sequence = NULL;
    nextSequence = NULL;

    frames.clear();
}

int Magic3D::Animation::getCurrentFrame()
{
    return frame;
}

void Magic3D::Animation::setCurrentFrame(int frame)
{
    if (frame >= 0 && frame < (int)frames.size())
    {
        elapsed = 0.0f;
        elapsedUpdate = 0.0f;
        this->frame = frame;
    }
    needUpdate = true;
}

Magic3D::AnimationFrame* Magic3D::Animation::getFrame(int frame)
{
    AnimationFrame* f = NULL;

    if (frame >= 0 && frame < (int)frames.size())
    {
        f = &frames[frame];
    }
    return f;
}

std::vector<Magic3D::AnimationFrame>* Magic3D::Animation::getFrames()
{
    return &frames;
}

int Magic3D::Animation::getFrameCount()
{
    return frames.size();
}

void Magic3D::Animation::addSequence(std::string name, float speed, int startFrame, int endFrame, bool loop)
{
    if (getSequences())
    {
        AnimationSequence* sequence = new AnimationSequence(name);
        sequence->setSpeed(speed);
        sequence->setStartFrame(startFrame);
        sequence->setEndFrame(endFrame);
        sequence->setLooping(loop);

        getSequences()->push_back(sequence);
        sequences->needSave = true;
    }
}

void Magic3D::Animation::removeSequence(std::string name)
{
    std::vector<AnimationSequence*>::iterator it_s = getSequences()->begin();

    while (it_s != getSequences()->end())
    {
        AnimationSequence* sequence = *it_s;

        if (sequence && sequence->getName().compare(name) == 0)
        {
            Scene::getInstance()->stopAllModels(sequence, name);
            getSequences()->erase(it_s);
            delete sequence;

            sequences->needSave = true;
            break;
        }

        it_s++;
    }
}

void Magic3D::Animation::setSequences(AnimationSequences* sequences)
{
    this->sequences = sequences;
}

std::vector<Magic3D::AnimationSequence*>* Magic3D::Animation::getSequences()
{
    std::vector<AnimationSequence*>* result = NULL;
    if (sequences)
    {
        result = &sequences->sequences;
    }
    return result;
}

Magic3D::AnimationSequence* Magic3D::Animation::getSequence(std::string name)
{
    AnimationSequence* result = NULL;

    if (getSequences())
    {
        std::vector<AnimationSequence*>::const_iterator it_s = getSequences()->begin();

        while (it_s != getSequences()->end())
        {
            AnimationSequence* sequence = *it_s++;

            if (sequence && sequence->getName().compare(name) == 0)
            {
                result = sequence;
                break;
            }
        }
    }

    return result;
}

void Magic3D::Animation::setCurrentSequence(std::string name)
{
    if (!sequence || sequence->getName().compare(name) != 0)
    {
        setCurrentSequence(getSequence(name));
    }
}

void Magic3D::Animation::setCurrentSequence(AnimationSequence* sequence)
{
    stop();
    this->sequence = sequence;
    if (this->sequence)
    {
        setCurrentFrame(this->sequence->getStartFrame());
    }
}

Magic3D::AnimationSequence* Magic3D::Animation::getCurrentSequence()
{
    return sequence;
}

std::string Magic3D::Animation::getCurrentSequenceName()
{
    if (sequence)
    {
        return sequence->getName();
    }
    else
    {
        return std::string("");
    }
}

Magic3D::Skeleton* Magic3D::Animation::getSkeleton()
{
    return skeleton;
}

Magic3D::XMLElement* Magic3D::Animation::save(XMLElement* root)
{
    if (root)
    {
        if (sequences)
        {
            sequences->save();
        }
        saveString(root, M3D_ANIMATION_XML_CURRENT_SEQUENCE, getCurrentSequence() ? getCurrentSequence()->getName() : std::string(M3D_XML_NULL));
    }
    return root;
}

Magic3D::XMLElement* Magic3D::Animation::load(XMLElement* root)
{
    if (root)
    {
        std::string current = loadString(root, M3D_ANIMATION_XML_CURRENT_SEQUENCE);
        if (current.compare(std::string(M3D_XML_NULL)) != 0)
        {
            setCurrentSequence(current);
            play();
        }
    }
    return root;
}
