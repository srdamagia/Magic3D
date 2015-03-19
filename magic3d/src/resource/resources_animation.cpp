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
#include <magic3d/resource/resources_animation.h>

Magic3D::AnimationSequences::AnimationSequences(std::string name)
{
    this->name = name;
    needSave = false;
}

Magic3D::AnimationSequences::~AnimationSequences()
{
    while (!sequences.empty()) {
        AnimationSequence* sequence = *sequences.begin();
        sequences.erase(sequences.begin());
        if (sequence)
        {
            delete sequence;
        }
    }
}

const std::string& Magic3D::AnimationSequences::getName()
{
    return name;
}

void Magic3D::AnimationSequences::addSequence(std::string name, float speed, int startFrame, int endFrame, bool loop)
{
    AnimationSequence* sequence = new AnimationSequence(name);
    sequence->setSpeed(speed);
    sequence->setStartFrame(startFrame);
    sequence->setEndFrame(endFrame);
    sequence->setLooping(loop);

    sequences.push_back(sequence);
    needSave = true;
}
std::string Magic3D::AnimationSequences::getAnimationFile()
{
    std::string fileName = ResourceManager::getInstance()->getPath() + M3D_PATH_MODEL + name + M3D_ANIMATION_FILE;
    return fileName;
}

bool Magic3D::AnimationSequences::save()
{
    bool result = false;
    if (needSave && !name.empty())
    {
        //
        std::string fileName = getAnimationFile();

        XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
        doc.LinkEndChild( decl );

        XMLElement* root = doc.NewElement( "MGE" );
        doc.LinkEndChild( root );

        XMLComment* comment = doc.NewComment(" MGE Animation ");
        root->LinkEndChild( comment );

        XMLElement* animation = root->GetDocument()->NewElement( M3D_ANIMATION_XML );
        animation->SetAttribute(M3D_ANIMATION_XML_SEQUENCES, (int)sequences.size());
        root->LinkEndChild( animation );

        std::vector<AnimationSequence*>::const_iterator it_s = sequences.begin();

        while (it_s != sequences.end())
        {
            AnimationSequence* sequence = *it_s++;

            XMLElement* seq = animation->GetDocument()->NewElement( M3D_ANIMATION_SEQUENCE_XML );
            animation->LinkEndChild( seq );

            Xml::saveString(seq, M3D_ANIMATION_SEQUENCE_XML_NAME, sequence->getName());
            Xml::saveFloat(seq, M3D_ANIMATION_SEQUENCE_XML_SPEED, sequence->getSpeed());
            Xml::saveInt(seq, M3D_ANIMATION_SEQUENCE_XML_START, sequence->getStartFrame());
            Xml::saveInt(seq, M3D_ANIMATION_SEQUENCE_XML_END, sequence->getEndFrame());
            Xml::saveInt(seq, M3D_ANIMATION_SEQUENCE_XML_LOOP, sequence->isLooping() ? 1 : 0);
        }

        result = doc.SaveFile( fileName.c_str() ) == XML_SUCCESS;

        needSave = false;
    }

    return result;
}

bool Magic3D::AnimationSequences::load()
{
    bool result = false;
    if (!name.empty())
    {
        std::string fileName = getAnimationFile();

        XMLDocument* doc = new XMLDocument();
        result = doc->LoadFile(fileName.c_str()) == XML_SUCCESS;
        if (result)
        {
            XMLElement* root = doc->FirstChildElement();

            // should always have a valid root but handle gracefully if it does
            if (root)
            {
                XMLElement* animation = root->FirstChildElement(M3D_ANIMATION_XML);

                if (animation)
                {
                    XMLElement* sequence = animation->FirstChildElement(M3D_ANIMATION_SEQUENCE_XML);

                    while (sequence)
                    {
                        std::string name = Xml::loadString(sequence, M3D_ANIMATION_SEQUENCE_XML_NAME);


                        float speed = Xml::loadFloat(sequence, M3D_ANIMATION_SEQUENCE_XML_SPEED);
                        int start = Xml::loadInt(sequence, M3D_ANIMATION_SEQUENCE_XML_START);
                        int end = Xml::loadInt(sequence, M3D_ANIMATION_SEQUENCE_XML_END);
                        bool loop = Xml::loadInt(sequence, M3D_ANIMATION_SEQUENCE_XML_LOOP) == 0 ? false : true;

                        addSequence(name, speed, start, end, loop);


                        sequence = sequence->NextSiblingElement(M3D_ANIMATION_SEQUENCE_XML);
                    }
                }
            }
        }

        if (doc)
        {
            delete doc;
            doc = NULL;
        }

        needSave = false;
    }

    return result;
}

//******************************************************************************
Magic3D::ResourcesAnimation::ResourcesAnimation() : Resources<AnimationSequences>()
{
    resourceName = M3D_RESOURCE_ANIMATION;
}

Magic3D::ResourcesAnimation::~ResourcesAnimation()
{

}

Magic3D::AnimationSequences* Magic3D::ResourcesAnimation::load(std::string name, bool& created)
{
    AnimationSequences* result = Resources<AnimationSequences>::get(name);
    created = !result;
    if (!result)
    {
        result = new AnimationSequences(name);
        result->load();
        add(result);
    }
    return result;
}
