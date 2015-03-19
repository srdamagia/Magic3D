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

Magic3D::SpriteSequence::SpriteSequence(std::string name)
{
    this->name = name;
    fps = 0;
}

Magic3D::SpriteSequence::~SpriteSequence()
{
    frames.clear();
}

int Magic3D::SpriteSequence::addFrame(Vector4 frame)
{
    frames.push_back(frame);

    return frames.size() - 1;
}

int Magic3D::SpriteSequence::addFrame(int col, int row, float pxWidth, float pxHeight, float imgWidth, float imgHeight)
{
    float texW = pxWidth / imgWidth;
    float texH = pxHeight / imgHeight;
    return addFrame(Vector4(col * texW, 1.0f - row * texH, texW, texH));
}

Magic3D::Vector4 Magic3D::SpriteSequence::getFrame(int index)
{
    Vector4 frame = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    int size = frames.size();
    if (index >= 0 && index < size)
    {
        frame = frames.at(index);
    }

    return frame;
}

void Magic3D::SpriteSequence::setFrame(int index, Vector4 frame)
{
    int size = frames.size();
    if (index >= 0 && index < size)
    {
        frames[index] = frame;
    }
}

void Magic3D::SpriteSequence::removeFrame(int index)
{
    frames.erase(frames.begin() + index);
}

void Magic3D::SpriteSequence::removeAllFrames()
{
    frames.clear();
}

void Magic3D::SpriteSequence::setName(std::string name)
{
    this->name = name;
}

const std::string& Magic3D::SpriteSequence::getName()
{
    return name;
}

void Magic3D::SpriteSequence::setFPS(int fps)
{
    this->fps = fps;
}

int Magic3D::SpriteSequence::getFPS()
{
    return fps;
}

int Magic3D::SpriteSequence::getFrameCount()
{
    return frames.size();
}

void Magic3D::SpriteSequence::save(XMLElement* root)
{
    if (root)
    {
        XMLElement* attribute = root->GetDocument()->NewElement( M3D_SPRITE_XML_SEQUENCE_NAME );
        attribute->LinkEndChild( attribute->GetDocument()->NewText( getName().c_str() ));
        root->LinkEndChild(attribute);

        std::stringstream st;
        st << getFPS();
        attribute = root->GetDocument()->NewElement( M3D_SPRITE_XML_SEQUENCE_FPS );
        attribute->LinkEndChild( attribute->GetDocument()->NewText( st.str().c_str() ));
        root->LinkEndChild(attribute);

        std::vector<Vector4>::const_iterator it_f = frames.begin();
        while (it_f != frames.end())
        {
            Vector4 frame = *it_f++;

            attribute = root->GetDocument()->NewElement( M3D_SPRITE_XML_FRAME );
            attribute->SetAttribute(M3D_VECTOR_XML_X, frame.getX());
            attribute->SetAttribute(M3D_VECTOR_XML_Y, frame.getY());
            attribute->SetAttribute(M3D_VECTOR_XML_Z, frame.getZ());
            attribute->SetAttribute(M3D_VECTOR_XML_W, frame.getW());
            root->LinkEndChild( attribute );
        }
    }
}

void Magic3D::SpriteSequence::load(XMLElement* root)
{
    if (root)
    {
        XMLElement* xml = root->FirstChildElement(M3D_SPRITE_XML_SEQUENCE_FPS);
        setFPS(atoi(xml->GetText()));

        xml = root->FirstChildElement(M3D_SPRITE_XML_FRAME);

        while (xml)
        {
            float x = 0.0f;
            float y = 0.0f;
            float w = 1.0f;
            float z = 1.0f;

            xml->QueryFloatAttribute("x", &x);
            xml->QueryFloatAttribute("y", &y);
            xml->QueryFloatAttribute("z", &z);
            xml->QueryFloatAttribute("w", &w);

            addFrame(Vector4(x, y, z, w));

            xml = xml->NextSiblingElement(M3D_SPRITE_XML_FRAME);
        }
    }
}

//******************************************************************************
Magic3D::Sprite::Sprite(const Sprite& sprite, std::string name) : Object(sprite, name)
{
    this->sequence = sprite.sequence;

    this->box = sprite.box;

    this->x = sprite.x;
    this->y = sprite.y;
    this->width = sprite.width;
    this->height = sprite.height;

    this->scaleX = sprite.scaleX;
    this->scaleY = sprite.scaleY;

    this->elapsed = sprite.elapsed;
    this->fps = sprite.fps;

    this->anchorHorizontal = sprite.anchorHorizontal;
    this->anchorVertical = sprite.anchorVertical;

    this->alignHorizontal = sprite.alignHorizontal;
    this->alignVertical = sprite.alignVertical;

    this->frame = sprite.frame;

    this->playing = sprite.playing;
    this->loop = sprite.loop;

    this->needUpdate = sprite.needUpdate;

    this->spriteMesh = getMeshes()->at(0);
}

Magic3D::Sprite::Sprite(std::string name) : Object(eOBJECT_SPRITE, eRENDER_2D, name)
{
    init();
}

Magic3D::Sprite::Sprite(OBJECT type, std::string name) : Object(type, eRENDER_2D, name)
{
    init();
}

void Magic3D::Sprite::init()
{
    sequence = NULL;
    frame    = -1;
    fps      = 0.0f;
    playing  = false;
    loop     = false;

    elapsed  = 0.0f;

    x      = 0.0f;
    y      = 0.0f;
    width  = 0.0f;
    height = 0.0f;

    anchorHorizontal = 0.0f;
    anchorVertical   = 0.0f;
    alignHorizontal  = eHORIZONTAL_ALIGN_LEFT;
    alignVertical    = eVERTICAL_ALIGN_TOP;

    scaleX = 1.0f;
    scaleY = 1.0f;

    box = Box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));

    setShapeRotation(Vector3(0.0f, 0.0f, 1.0f));
    setShapeTranslation(Vector3(1.0f, 1.0f, 0.0f));

    spriteMesh = new Mesh(eMESH_TRIANGLES, SPRITE_MESH_NAME, true);
    spriteMesh->setIlluminated(false);
    spriteMesh->setFogged(false);
    spriteMesh->setCastShadows(false);
    spriteMesh->setReceiveShadows(false);

    if (getType() == eOBJECT_SPRITE)
    {
        spriteMesh->getData()->addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z, ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        spriteMesh->getData()->createVbo();
    }

    addMesh(spriteMesh);

    needUpdate = true;
}


Magic3D::Sprite::~Sprite()
{
    spriteMesh = NULL;
}

Magic3D::Sprite* Magic3D::Sprite::spawn(std::string name) const
{
    return (new Sprite(*this, name));
}

bool Magic3D::Sprite::update()
{
    Sprite* sp = getParent() ? static_cast<Sprite*>(getParent()) : NULL;
    float px = getParent() ? getParent()->getMatrixFromParent().getTranslation().getX() -  sp->getWidth() * 0.5f: 0.0f;
    float x = getMatrixFromParent().getTranslation().getX();

    float py = getParent() ? getParent()->getMatrixFromParent().getTranslation().getY() -  sp->getHeight() * 0.5f: 0.0f;
    float y = getMatrixFromParent().getTranslation().getY();

    float currentWidth = getWidth();
    float currentHeight = getHeight();

    switch (getHorizontalAlign())
    {
        case eHORIZONTAL_ALIGN_RIGHT:
        {
            float w = Renderer::getInstance()->getWindow()->getWidth() - getHorizontalAnchor();
            if (x + currentWidth != w)
            {
                setPosition(w - currentWidth - px, getY());
            }
            break;
        }
        case eHORIZONTAL_ALIGN_CENTER:
        {
            float halfWidth = currentWidth * 0.5f;
            float w = (float)Renderer::getInstance()->getWindow()->getWidth() * 0.5f + getHorizontalAnchor();
            if (x + halfWidth != w)
            {
                setPosition(w - halfWidth - px, getY());
            }
            break;
        }
        default: break;
    }

    switch (getVerticalAlign())
    {
        case eVERTICAL_ALIGN_BOTTOM:
        {
            float h = Renderer::getInstance()->getWindow()->getHeight() - getVerticalAnchor();
            if (y + currentHeight != h)
            {
                setPosition(getX(), h - currentHeight - py);
            }
            break;
        }
        case eVERTICAL_ALIGN_CENTER:
        {
            float halfHeight = currentHeight * 0.5f;
            float h = (float)Renderer::getInstance()->getWindow()->getHeight() * 0.5f + getVerticalAnchor();
            if (y + halfHeight != h)
            {
                setPosition(getX(), h - halfHeight - py);
            }
            break;
        }
        default: break;
    }

    Object::update();

    if (needUpdate)
    {
        updateMeshes();
        needUpdate = false;
    }

    bool result = isPlaying() && frame >= 0 && sequence;

    if (result)
    {
        elapsed += Magic3D::Magic3D::getInstance()->getElapsedTime();
        if (elapsed >= fps)
        {
            elapsed = 0.0f;
            frame++;

            if (isLooping())
            {
                if  (frame >= getFrameCount())
                {
                    frame = 0;
                }
            }
            else if (frame >= getFrameCount())
            {
                frame = getFrameCount() - 1;
                stop();
            }

            updateFrame();
        }
    }
    return result;
}

bool Magic3D::Sprite::updateMeshes()
{
    if (getMeshes()->size() > 0 && getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        MeshData* data = spriteMesh->getData();

        float* buffer = data->mapBuffer();

        ColorRGBA color = getMeshes()->at(0)->getMaterials()->at(0)->getColorDiffuse();
        spriteMesh->getData()->setQuad(buffer, 0, -0.5f, -0.5f, 1.0f, 1.0f, color);

        data->unmapBuffer();
    }

    Object::setPosition(Vector3(x + width * 0.5f * scaleX, y + height * 0.5f * scaleY, 0.0f));
    Object::setScale(Vector3(width * scaleX, height * scaleY, 1.0f));

    return true;
}

void Magic3D::Sprite::addMaterial(Material* material)
{
    if (spriteMesh)
    {
        spriteMesh->addMaterial(material);
    }
}

void Magic3D::Sprite::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
    needUpdate = true;
}

void Magic3D::Sprite::setPosition(float x, float y, float z)
{
    Object::setPosition(Vector3(x, y, z));
}


void Magic3D::Sprite::setSize(float width, float height)
{
    this->width = width;
    this->height = height;
    needUpdate = true;
}

void Magic3D::Sprite::setScale(float x, float y)
{
    scaleX = x;
    scaleY = y;
    needUpdate = true;
}

void Magic3D::Sprite::setScale(float x, float y, float z)
{
    Object::setScale(Vector3(x, y, z));
}

float Magic3D::Sprite::getX()
{
    return x;
}

float Magic3D::Sprite::getY()
{
    return y;
}

float Magic3D::Sprite::getWidth()
{
    return width;
}

float Magic3D::Sprite::getHeight()
{
    return height;
}

float Magic3D::Sprite::getScaleX()
{
    return scaleX;
}

float Magic3D::Sprite::getScaleY()
{
    return scaleY;
}

void Magic3D::Sprite::setHorizontalAnchor(float anchor)
{
    anchorHorizontal = anchor;
}

float Magic3D::Sprite::getHorizontalAnchor()
{
    return anchorHorizontal;
}

void Magic3D::Sprite::setVerticalAnchor(float anchor)
{
    anchorVertical = anchor;
}

float Magic3D::Sprite::getVerticalAnchor()
{
    return anchorVertical;
}

void Magic3D::Sprite::setHorizontalAlign(HORIZONTAL_ALIGN align)
{
    alignHorizontal = align;
}

Magic3D::HORIZONTAL_ALIGN Magic3D::Sprite::getHorizontalAlign()
{
    return alignHorizontal;
}

void Magic3D::Sprite::setVerticalAlign(VERTICAL_ALIGN align)
{
    alignVertical = align;
}

Magic3D::VERTICAL_ALIGN Magic3D::Sprite::getVerticalAlign()
{
    return alignVertical;
}

const Magic3D::Box& Magic3D::Sprite::getBoundingBox()
{
    return box;
}

Magic3D::Matrix4 Magic3D::Sprite::getMatrixFromParent()
{
    Matrix4 m = getMatrix();

    if (getParent())
    {
        Matrix4 pm = getParent()->getMatrixFromParent();

        Sprite* sp = static_cast<Sprite*>(getParent());

        if (getParent()->getType() == eOBJECT_SPRITE)
        {
            Matrix3 mRot = pm.getUpper3x3();

            Vector3 tmpScale  = Vector3(length(mRot.getCol0()), length(mRot.getCol1()), length(mRot.getCol2()));
            mRot.setCol0(normalize(mRot.getCol0()));
            mRot.setCol1(normalize(mRot.getCol1()));
            mRot.setCol2(normalize(mRot.getCol2()));
            pm = Matrix4(mRot, pm.getTranslation());
            pm *= Matrix4(Quaternion::identity(), Vector3(tmpScale.getX() * -0.5f, tmpScale.getY() * -0.5f, 0.0f));
        }
        else
        {
            pm *= Matrix4(Quaternion::identity(), Vector3(sp->getWidth() * -0.5f, sp->getHeight() * -0.5f, 0.0f));
        }
        m = pm * m;
    }

    return m;
}

void Magic3D::Sprite::updateFrame()
{
    if (sequence)
    {
        Vector4 box = sequence->getFrame(frame);

        MeshData* data = spriteMesh->getData();

        float* buffer = data->mapBuffer();

        spriteMesh->getData()->setQuadTexture(buffer, 0, box.getX(), box.getY(), box.getZ(), box.getW());

        data->unmapBuffer();
    }
}

std::vector<Magic3D::SpriteSequence*>* Magic3D::Sprite::getSequences()
{
    std::vector<SpriteSequence*>* result = NULL;
    if (getSpriteTexture())
    {
        result = &getSpriteTexture()->sequences;
    }
    return result;
}

void Magic3D::Sprite::addSequence(SpriteSequence* sequence)
{
    if (sequence)
    {
        getSequences()->push_back(sequence);
        getSpriteTexture()->needSave = true;
    }
}

Magic3D::SpriteSequence* Magic3D::Sprite::getSequence(std::string name)
{
    SpriteSequence* result = NULL;
    std::vector<SpriteSequence*>::const_iterator it_s = getSequences()->begin();
    while (it_s != getSequences()->end())
    {
        SpriteSequence* seq = *it_s++;

        if (seq->getName().compare(name) == 0)
        {
            result = seq;
            break;
        }
    }

    return result;
}

void Magic3D::Sprite::removeSequence(std::string name)
{
    std::vector<SpriteSequence*>::const_iterator it_s = getSequences()->begin();
    int index = 0;
    while (it_s != getSequences()->end())
    {
        SpriteSequence* seq = *it_s++;

        if (seq->getName().compare(name) == 0)
        {
            Scene::getInstance()->stopAllSprites(seq, name);
            getSequences()->erase(getSequences()->begin() + index);
            delete seq;

            getSpriteTexture()->needSave = true;
            break;
        }

        index++;
    }
}

void Magic3D::Sprite::setCurrentSequence(std::string name)
{
    setCurrentSequence(getSequence(name));
}

void Magic3D::Sprite::setCurrentSequence(SpriteSequence* sequence)
{
    this->sequence = sequence;
    frame = 0;
    if (sequence)
    {
        fps = 1.0f / sequence->getFPS();
    }
    else
    {
        fps = 0.0f;
    }
    playing = false;
    updateFrame();
}

Magic3D::SpriteSequence* Magic3D::Sprite::getCurrentSequence()
{
    return sequence;
}

std::string Magic3D::Sprite::getCurrentSequenceName()
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

void Magic3D::Sprite::play()
{
    playing = true;
    elapsed = 0.0f;
}

void Magic3D::Sprite::stop()
{
    playing = false;
}

bool Magic3D::Sprite::isPlaying()
{
    return playing;
}

void Magic3D::Sprite::setLooping(bool loop)
{
    this->loop = loop;
}

bool Magic3D::Sprite::isLooping()
{
    return loop;
}

void Magic3D::Sprite::setFrame(int index)
{
    if (index >= 0 && index < getFrameCount())
    {
        frame = index;
    }
    else
    {
        frame = -1;
    }
    Vector4 box = sequence->getFrame(frame);
    MeshData* data = spriteMesh->getData();

    float* buffer = data->mapBuffer();

    spriteMesh->getData()->setQuadTexture(buffer, 0, box.getX(), box.getY(), box.getZ(), box.getW());

    data->unmapBuffer();
}

int Magic3D::Sprite::getFrame()
{
    return frame;
}

int Magic3D::Sprite::getFrameCount()
{
    int count = 0;
    if (sequence)
    {
        count = sequence->getFrameCount();
    }

    return count;
}

Magic3D::Texture* Magic3D::Sprite::getSpriteTexture()
{
    Texture* result = NULL;
    if (getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        result = getMeshes()->at(0)->getMaterials()->at(0)->getTexture(0);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::Sprite::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* sprite = root->GetDocument()->NewElement(M3D_SPRITE_XML);
        root->LinkEndChild(sprite);

        XMLElement* attribute = sprite->GetDocument()->NewElement( M3D_SPRITE_XML_POSITION );
        attribute->SetAttribute(M3D_VECTOR_XML_X, x);
        attribute->SetAttribute(M3D_VECTOR_XML_Y, y);
        sprite->LinkEndChild( attribute );

        attribute = sprite->GetDocument()->NewElement( M3D_SPRITE_XML_SCALE );
        attribute->SetAttribute(M3D_VECTOR_XML_X, scaleX);
        attribute->SetAttribute(M3D_VECTOR_XML_Y, scaleY);
        sprite->LinkEndChild( attribute );

        attribute = sprite->GetDocument()->NewElement( M3D_SPRITE_XML_SIZE );
        attribute->SetAttribute(M3D_SPRITE_XML_WIDTH, width);
        attribute->SetAttribute(M3D_SPRITE_XML_HEIGHT, height);
        sprite->LinkEndChild( attribute );

        attribute = sprite->GetDocument()->NewElement( M3D_SPRITE_XML_ANCHOR );
        saveFloat(attribute, M3D_SPRITE_XML_ANCHOR_HORIZONTAL, anchorHorizontal);
        saveFloat(attribute, M3D_SPRITE_XML_ANCHOR_VERTICAL, anchorVertical);
        saveInt(attribute, M3D_SPRITE_XML_ALIGN_HORIZONTAL, alignHorizontal);
        saveInt(attribute, M3D_SPRITE_XML_ALIGN_VERTICAL, alignVertical);
        sprite->LinkEndChild( attribute );

        Texture* texture = getSpriteTexture();

        if (texture)
        {
            texture->saveSprite();
        }

        attribute = sprite->GetDocument()->NewElement( M3D_SPRITE_XML_CURRENT_SEQUENCE );
        attribute->LinkEndChild( attribute->GetDocument()->NewText( getCurrentSequence() ? getCurrentSequence()->getName().c_str() : M3D_XML_NULL ));
        sprite->LinkEndChild(attribute);
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Sprite::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* sprite = root->FirstChildElement(M3D_SPRITE_XML);

        XMLElement* xml = sprite->FirstChildElement(M3D_SPRITE_XML_POSITION);
        if (xml)
        {
            xml->QueryFloatAttribute(M3D_VECTOR_XML_X, &x);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Y, &y);
        }

        xml = sprite->FirstChildElement(M3D_SPRITE_XML_SCALE);
        if (xml)
        {
            xml->QueryFloatAttribute(M3D_VECTOR_XML_X, &scaleX);
            xml->QueryFloatAttribute(M3D_VECTOR_XML_Y, &scaleY);
        }

        xml = sprite->FirstChildElement(M3D_SPRITE_XML_SIZE);
        if (xml)
        {
            xml->QueryFloatAttribute(M3D_SPRITE_XML_WIDTH, &width);
            xml->QueryFloatAttribute(M3D_SPRITE_XML_HEIGHT, &height);
        }

        xml = sprite->FirstChildElement(M3D_SPRITE_XML_ANCHOR);
        if (xml)
        {
            anchorHorizontal = loadFloat(xml, M3D_SPRITE_XML_ANCHOR_HORIZONTAL);
            anchorVertical   = loadFloat(xml, M3D_SPRITE_XML_ANCHOR_VERTICAL);
            alignHorizontal  = (HORIZONTAL_ALIGN)loadInt(xml, M3D_SPRITE_XML_ALIGN_HORIZONTAL);
            alignVertical    = (VERTICAL_ALIGN)loadInt(xml, M3D_SPRITE_XML_ALIGN_VERTICAL);
        }

        XMLElement* attribute = sprite->FirstChildElement(M3D_SPRITE_XML_CURRENT_SEQUENCE);
        std::string sequenceName = std::string(attribute->GetText());

        if (sequenceName.compare(M3D_XML_NULL) != 0)
        {
            setCurrentSequence(sequenceName);
            setLooping(true);
            play();
        }

        needUpdate = true;
        needTransform = true;
    }

    return root;
}
