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

#ifndef MAGIC3D_SPRITE_H
#define MAGIC3D_SPRITE_H

#include <magic3d/object.h>

namespace Magic3D
{

#define SPRITE_MESH_NAME                 "sprite"

#define M3D_SPRITE_XML                   "Sprite"
#define M3D_SPRITE_XML_POSITION          "position"
#define M3D_SPRITE_XML_SCALE             "scale"
#define M3D_SPRITE_XML_SIZE              "size"
#define M3D_SPRITE_XML_WIDTH             "width"
#define M3D_SPRITE_XML_HEIGHT            "height"
#define M3D_SPRITE_XML_ANCHOR            "anchor"
#define M3D_SPRITE_XML_ANCHOR_HORIZONTAL "anchor_horizontal"
#define M3D_SPRITE_XML_ANCHOR_VERTICAL   "anchor_vertical"
#define M3D_SPRITE_XML_ALIGN_HORIZONTAL  "align_horizontal"
#define M3D_SPRITE_XML_ALIGN_VERTICAL    "align_vertical"
#define M3D_SPRITE_XML_CURRENT_SEQUENCE  "current_sequence"
#define M3D_SPRITE_XML_SEQUENCE          "Sequence"
#define M3D_SPRITE_XML_FRAME             "frame"
#define M3D_SPRITE_XML_SEQUENCE_NAME     "name"
#define M3D_SPRITE_XML_SEQUENCE_FPS      "fps"


class SpriteSequence
{
private:
    std::vector<Vector4> frames;
    std::string name;

    int fps;

public:
    SpriteSequence(std::string name);
    ~SpriteSequence();

    int addFrame(Vector4 frame);
    int addFrame(int col, int row, float pxWidth, float pxHeight, float imgWidth, float imgHeight);
    Vector4 getFrame(int index);
    void setFrame(int index, Vector4 frame);
    void removeFrame(int index);
    void removeAllFrames();

    void setName(std::string name);
    const std::string& getName();

    void setFPS(int fps);
    int getFPS();

    int getFrameCount();

    void save(XMLElement* root);
    void load(XMLElement* root);

};

enum HORIZONTAL_ALIGN
{
    eHORIZONTAL_ALIGN_LEFT,
    eHORIZONTAL_ALIGN_CENTER,
    eHORIZONTAL_ALIGN_RIGHT
};

enum VERTICAL_ALIGN
{
    eVERTICAL_ALIGN_TOP,
    eVERTICAL_ALIGN_CENTER,
    eVERTICAL_ALIGN_BOTTOM
};

class Sprite : public Object
{
private:
    SpriteSequence* sequence;

protected:
    Mesh* spriteMesh;

    Box box;

    float width;
    float height;
private:
    float elapsed;
    float fps;

    float anchorHorizontal;
    float anchorVertical;

    HORIZONTAL_ALIGN alignHorizontal;
    VERTICAL_ALIGN alignVertical;

    int frame;

    bool playing;
    bool loop;

protected:
    bool needUpdate;

    void init();
    void updateFrame();

    Texture* getSpriteTexture();

    Sprite(const Sprite& sprite, std::string name);

public:
    Sprite(std::string name);
    Sprite(OBJECT type, std::string name);
    virtual ~Sprite();
    virtual Sprite* spawn(std::string name) const;

    virtual bool update();
    virtual bool updateMeshes();
    void addMaterial(Material* material);

    void setSize(float width, float height);

    float getWidth();
    float getHeight();

    void setHorizontalAnchor(float anchor);
    float getHorizontalAnchor();
    void setVerticalAnchor(float anchor);
    float getVerticalAnchor();

    void setHorizontalAlign(HORIZONTAL_ALIGN align);
    HORIZONTAL_ALIGN getHorizontalAlign();
    void setVerticalAlign(VERTICAL_ALIGN align);
    VERTICAL_ALIGN getVerticalAlign();

    virtual const Box& getBoundingBox();

    std::vector<SpriteSequence*>* getSequences();
    void addSequence(SpriteSequence* sequence);
    SpriteSequence* getSequence(std::string name);
    void removeSequence(std::string name);

    void setCurrentSequence(std::string name);
    void setCurrentSequence(SpriteSequence* sequence);
    SpriteSequence* getCurrentSequence();
    std::string getCurrentSequenceName();

    void play();
    void stop();
    bool isPlaying();

    void setLooping(bool loop);
    bool isLooping();

    void setFrame(int index);
    int getFrame();
    int getFrameCount();

    void saveSpriteTexture();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_SPRITE_H
