/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2016 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2016
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

#ifndef MAGIC3D_RESOURCES_ANIMATION_H
#define MAGIC3D_RESOURCES_ANIMATION_H

#include <magic3d/resource/resources.h>

namespace Magic3D
{
#define M3D_ANIMATION_FILE ".animation"

class AnimationSequences
{
private:
    std::string name;

    std::string getAnimationFile();
public:
    std::vector<AnimationSequence*> sequences;

    bool needSave;

    AnimationSequences(std::string name);
    ~AnimationSequences();

    const std::string& getName();

    void addSequence(std::string name, float speed, int startFrame, int endFrame, bool loop);
    bool save();
    bool load();
};

//template <class M3D_Class>
class ResourcesAnimation : public Resources<AnimationSequences>
{
public:
    ResourcesAnimation();
    virtual ~ResourcesAnimation();

    AnimationSequences* load(std::string name, bool& created);
};

}

#endif // MAGIC3D_RESOURCES_ANIMATION_H
