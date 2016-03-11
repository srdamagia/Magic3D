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

#include <magic3d/magic3d.h>
#include <magic3d/resource/resources_texture.h>
#include <magic3d/image/image_tga.h>
#include <magic3d/image/image_png.h>
#include <magic3d/image/image_pvr.h>

Magic3D::Texture::Texture(std::string name, unsigned int id, TEXTURE type, int width, int height, bool mipmap, bool clamp, std::string file)
{
    this->name = name;
    this->id     = id;
    this->type   = type;
    this->width  = width;
    this->height = height;
    this->mipmap = mipmap;
    this->clamp  = clamp;
    this->file   = file;

    needSave = false;

    loadSprite();
}

Magic3D::Texture::~Texture()
{
    if (type != eTEXTURE_FBO)
    {
        Renderer::getInstance()->deleteTexture(id);
    }

    while (!sequences.empty())
    {
        SpriteSequence* seq = *sequences.begin();
        sequences.erase(sequences.begin());
        if (seq)
        {
            delete seq;
        }
    }
}

const std::string& Magic3D::Texture::getName()
{
    return name;
}

std::string Magic3D::Texture::getSpriteFile()
{
    std::string fileName = ResourceManager::getTextures()->getTexturePath(type, file);
    if (fileName.find(".pvr") == fileName.size() - 4)
    {
        fileName = fileName.substr(0, fileName.size() - 4);
    }
    fileName += M3D_SPRITE_FILE;
    return fileName;
}

void Magic3D::Texture::saveSprite()
{
    if (needSave && !file.empty())
    {
        std::string fileName = getSpriteFile();

        XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration( MAGIC3D_VERSION );
        doc.LinkEndChild( decl );

        XMLElement* root = doc.NewElement( "MGE" );
        doc.LinkEndChild( root );

        XMLComment* comment = doc.NewComment(" MGE Sprite ");
        root->LinkEndChild( comment );

        XMLElement* node = root->GetDocument()->NewElement( M3D_SPRITE_XML );
        root->LinkEndChild( node );

        std::vector<SpriteSequence*>::const_iterator it_s = sequences.begin();
        while (it_s != sequences.end())
        {
            SpriteSequence* sequence = *it_s++;

            XMLElement* sequenceXML = node->GetDocument()->NewElement( M3D_SPRITE_XML_SEQUENCE );
            sequenceXML->SetAttribute("frames", sequence->getFrameCount());
            node->LinkEndChild( sequenceXML );

            sequence->save(sequenceXML);
        }

        doc.SaveFile( fileName.c_str() );

        needSave = false;
    }
}

void Magic3D::Texture::loadSprite()
{
    if (!file.empty())
    {
        std::string fileName = getSpriteFile();

        XMLDocument* doc = new XMLDocument();

        bool result = false;
        if (ResourceManager::getInstance()->getPackage())
        {
            Memory mem;
            result = ResourceManager::getInstance()->unpack(fileName, &mem);
            std::string str = mem.getBuffer()->str();
            result = doc->Parse(str.c_str(), str.size()) == XML_SUCCESS && result;
        }
        else
        {
            result = doc->LoadFile(fileName.c_str()) == XML_SUCCESS;
        }

        if (result)
        {
            XMLElement* root = doc->FirstChildElement();

            // should always have a valid root but handle gracefully if it does
            if (root)
            {
                XMLElement* node = root->FirstChildElement(M3D_SPRITE_XML)->FirstChildElement(M3D_SPRITE_XML_SEQUENCE);

                while (node)
                {
                    XMLElement* attribute = node->FirstChildElement(M3D_SPRITE_XML_SEQUENCE_NAME);

                    std::string s(attribute->GetText());

                    SpriteSequence* sequence = new SpriteSequence(s);

                    sequence->load(node);

                    sequences.push_back(sequence);

                    node = node->NextSiblingElement(M3D_SPRITE_XML_SEQUENCE);
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
}

Magic3D::Texture* Magic3D::Texture::load(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created)
{
    created = false;
    Texture* texture = NULL;

    std::string fileName = ResourceManager::getTextures()->getTexturePath(type, file);

    DataBuffer* io;
    bool ready = false;

    if (ResourceManager::getInstance()->getPackage())
    {
        io = new Memory();
#ifdef MAGIC3D_IOS
        std::string tmpName = fileName + ".pvr";
        ready = ResourceManager::getInstance()->unpack(fileName, io);
        if (ready)
        {
            fileName = tmpName;
        }
        else
#endif
        {
            ready = ResourceManager::getInstance()->unpack(fileName, io);
        }
    }
    else
    {
        io = new File();

#ifdef MAGIC3D_IOS
        std::string tmpName = fileName + ".pvr";
        ready = static_cast<File*>(io)->open(tmpName.c_str(), "rb");
        if (ready)
        {
            fileName = tmpName;
        }
        else
#endif
        {
            ready = static_cast<File*>(io)->open(fileName.c_str(), "rb");
        }
    }

    if (ready)
    {
        Image* image = NULL;

        std::string ending = ".pvr";
        std::string lower = fileName;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower.compare (lower.length() - ending.length(), ending.length(), ending) == 0)
        {
            image = new PVR();

            texture = static_cast<PVR*>(image)->getTexture(io, type, name, file, mipmap, clamp);
            
            created = true;
            delete image;
        }
        else
        {
            ending = ".png";
            if (lower.compare (lower.length() - ending.length(), ending.length(), ending) == 0)
            {
                image = new PNG();
            }
            else
            {
                image = new TGA();
            }

            if (image)
            {
                image->decode(io);

                texture = new Texture(name, Renderer::getInstance()->createTexture(image, mipmap, clamp), type, image->getWidth(), image->getHeight(), mipmap, clamp, file);
                created = true;

                delete image;
            }
        }
        
        if (!ResourceManager::getInstance()->getPackage())
        {
            static_cast<File*>(io)->close();
        }
    }

    if (created)
    {
        Log::logFormat(eLOG_RENDERER, "Texture \"%s\" loaded: %s", name.c_str(), fileName.c_str());
    }
    else
    {
        Log::logFormat(eLOG_FAILURE, "Texture \"%s\" not found: %s", name.c_str(), fileName.c_str());
    }
    delete io;

    return texture;
}

void Magic3D::Texture::setWrap(bool clamp)
{
    this->clamp = clamp;
    Renderer::getInstance()->setTextureWrap(this, clamp);
}

//******************************************************************************
Magic3D::ResourcesTexture::ResourcesTexture() : Resources<Texture>()
{
    resourceName = M3D_RESOURCE_TEXTURE;
}

Magic3D::ResourcesTexture::~ResourcesTexture()
{

}

Magic3D::Texture* Magic3D::ResourcesTexture::load(TEXTURE type, std::string name, std::string file, bool mipmap, bool clamp, bool& created)
{
    Texture* texture = Resources<Texture>::get(name);
    created = !texture;

    if (!texture)
    {
        texture = Texture::load(type, name, file, mipmap, clamp, created);
        add(texture);
    }

    return texture;
}

std::string Magic3D::ResourcesTexture::getPrefix(TEXTURE type)
{
    std::string prefix = "";
    switch (type)
    {
        case eTEXTURE_DIFFUSE: prefix += M3D_PATH_TEXTURE; break;
        case eTEXTURE_GUI:     prefix += M3D_PATH_GUI;     break;
        case eTEXTURE_FONT:    prefix += M3D_PATH_FONT;    break;
        default: break;
    }

    return prefix;
}

std::string Magic3D::ResourcesTexture::getTexturePath(TEXTURE type, std::string texture)
{
    std::string fileName = Resources<Texture>::getPath() + getPrefix(type) + texture;

    return fileName;
}

void Magic3D::ResourcesTexture::addDefault()
{
    bool created = false;
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_TEXTURE_NAME,   M3D_DEFAULT_TEXTURE,   true,  true, created);
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_PARTICLES_NAME, M3D_DEFAULT_PARTICLES, true,  true, created);
    load(eTEXTURE_GUI,     M3D_DEFAULT_THEME_NAME,     M3D_DEFAULT_THEME,     true, true, created);
    load(eTEXTURE_GUI,     M3D_DEFAULT_PROGRESS_NAME,  M3D_DEFAULT_PROGRESS,  true, true, created);

    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_TRUNKS_NAME,  M3D_DEFAULT_TRUNKS,  true, true,  created);
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_TWIGS_NAME,   M3D_DEFAULT_TWIGS,   true, true,  created);
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_GRASS_NAME,   M3D_DEFAULT_GRASS,   true, true,  created);
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_WATER_NAME,   M3D_DEFAULT_WATER,   true, false, created);
    load(eTEXTURE_DIFFUSE, M3D_DEFAULT_TERRAIN_NAME, M3D_DEFAULT_TERRAIN, true, false, created);

    add(new Texture("fbo_color", Renderer::getInstance()->getRenderToTextureColor(), eTEXTURE_FBO, Renderer::getInstance()->getRenderToTextureWidth(), Renderer::getInstance()->getRenderToTextureHeight(), true, true, ""));
    add(new Texture("fbo_depth", Renderer::getInstance()->getRenderToTextureDepth(), eTEXTURE_FBO, Renderer::getInstance()->getRenderToTextureWidth(), Renderer::getInstance()->getRenderToTextureHeight(), true, true, ""));
    add(new Texture("fbo_shadows", Renderer::getInstance()->getShadowsTexture(), eTEXTURE_FBO, Renderer::getInstance()->getShadowsTextureWidth(), Renderer::getInstance()->getShadowsTextureHeight(), true, true, ""));
    add(new Texture("fbo_reflection", Renderer::getInstance()->getReflectionsTexture(), eTEXTURE_FBO, Renderer::getInstance()->getReflectionsTextureWidth(), Renderer::getInstance()->getReflectionsTextureHeight(), true, true, ""));
    add(new Texture("fbo_glow_color", Renderer::getInstance()->getGlowTextureColor(), eTEXTURE_FBO, Renderer::getInstance()->getGlowTextureWidth(), Renderer::getInstance()->getGlowTextureHeight(), true, true, ""));
    //add(new Texture("fbo_glow_depth", Renderer::getInstance()->getGlowTextureDepth(), eTEXTURE_FBO, Renderer::getInstance()->getGlowTextureWidth(), Renderer::getInstance()->getGlowTextureHeight(), true, true, ""));
    add(new Texture("fbo_screen_color", Renderer::getInstance()->getScreenTextureColor(), eTEXTURE_FBO, Renderer::getInstance()->getScreenTextureWidth(), Renderer::getInstance()->getScreenTextureHeight(), true, true, ""));
    add(new Texture("fbo_screen_depth", Renderer::getInstance()->getScreenTextureDepth(), eTEXTURE_FBO, Renderer::getInstance()->getScreenTextureWidth(), Renderer::getInstance()->getScreenTextureHeight(), true, true, ""));
}

tinyxml2::XMLElement* Magic3D::ResourcesTexture::saveTexture(XMLElement* root, const char* name, Texture* texture)
{
    XMLElement* attribute = NULL;
    if (texture)
    {
        attribute = root->GetDocument()->NewElement( name );
        attribute->SetAttribute(M3D_MATERIAL_XML_TEX_NAME,   texture->getName().c_str());
        attribute->SetAttribute(M3D_MATERIAL_XML_TEX_FILE,   texture->file.c_str());
        attribute->SetAttribute(M3D_MATERIAL_XML_TEX_MIPMAP, texture->mipmap);
        attribute->SetAttribute(M3D_MATERIAL_XML_TEX_CLAMP,  texture->clamp);
        attribute->SetAttribute(M3D_MATERIAL_XML_TEX_TYPE,   texture->type);
        root->LinkEndChild( attribute );
    }

    return attribute;
}

Magic3D::Texture* Magic3D::ResourcesTexture::loadTexture(XMLElement* root)
{
    Texture* result = NULL;
    std::string texName = root->Attribute(M3D_MATERIAL_XML_TEX_NAME);
    std::string texFile = root->Attribute(M3D_MATERIAL_XML_TEX_FILE);
    bool mipmap = false;
    bool clamp = false;
    int type = -1;
    root->QueryBoolAttribute(M3D_MATERIAL_XML_TEX_MIPMAP, &mipmap);
    root->QueryBoolAttribute(M3D_MATERIAL_XML_TEX_CLAMP,  &clamp);
    root->QueryIntAttribute(M3D_MATERIAL_XML_TEX_TYPE,  &type);

    if (!texFile.empty() && texFile.compare(M3D_XML_NULL) != 0)
    {
        bool created = false;
        result = ResourceManager::getInstance()->addTexture((TEXTURE)type, texName, texFile, mipmap, clamp, created);
    }
    else if (!texName.empty() && texName.compare(M3D_XML_NULL) != 0)
    {
        result = ResourceManager::getTextures()->get(texName);
    }

    return result;
}
