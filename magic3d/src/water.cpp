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

#include <magic3d/resource/resourcemngr.h>

//******************************************************************************
Magic3D::Water::Water(const Water& water, std::string name) : Object(water, name)
{
    this->length = water.length;
    this->width = water.width;
    this->tileLength = water.tileLength;
    this->tileWidth = water.tileWidth;

    waterMesh = getMeshes()->at(0);

    if (getType() == eOBJECT_WATER)
    {
        generateWater();
    }

    needUpdate = true;
}

Magic3D::Water::Water(std::string name) : Object(eOBJECT_WATER, eRENDER_3D, name)
{
    init();
}

Magic3D::Water::Water(OBJECT type, std::string name) : Object(type, eRENDER_3D, name)
{
    init();
}

void Magic3D::Water::init()
{
    length = 256;
    width  = 256;

    tileLength = 2.0f;
    tileWidth  = 2.0f;

    waterMesh = new Mesh(eMESH_TRIANGLES_STRIP, WATER_MESH_NAME, true);
    waterMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_WATER));
    waterMesh->setIlluminated(true);
    waterMesh->setFogged(true);
    waterMesh->setCastShadows(false);
    waterMesh->setReceiveShadows(true);
    waterMesh->setReflective(true);

    addMesh(waterMesh);

    if (getType() == eOBJECT_WATER)
    {
        generateWater();
    }    

    needUpdate = true;
}


Magic3D::Water::~Water()
{
    waterMesh = NULL;
}

void* Magic3D::Water::spawn(std::string name) const
{
    return (new Water(*this, name));
}

bool Magic3D::Water::updateMeshes()
{
    /*if (getMeshes()->size() > 0 && getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        MeshData* data = waterMesh->getData();

        float* buffer = data->mapBuffer();

        float x = width * 0.5f;
        float y = height * 0.5f;

        ColorRGBA color = getMeshes()->at(0)->getMaterials()->at(0)->getColorDiffuse();
        waterMesh->getData()->setQuad(buffer, 0, -x, -y, width, height, color);

        data->unmapBuffer();
    }*/

    return true;
}

void Magic3D::Water::generateWater()
{
    if (waterMesh)
    {
        MeshData* dataWater = waterMesh->getData();
        dataWater->clear();

        float x = (float)width * tileWidth * -0.5f;
        float y = (float)length * tileLength * -0.5f;

        for (int r = 0; r < length; r++)
        {
            for (int c = 0; c < width ; c++)
            {
                Vertex3D vertex;
                vertex.position = Vector3(x + (float)c * tileWidth, 0.0, y + (float)r * tileLength);
                vertex.color = ColorRGBA(0.0f, Math::randomize(0.25f, 0.75f), 0.0f, 1.0f);
                vertex.normal = Vector3(0.0f, 1.0f, 0.0f);
                vertex.tangent = Vector3(0.0f, 0.0f, 0.0f);

                vertex.uv[0] = Texture2D(1.0f / (float)width * (float)c, 1.0f - 1.0f / (float)length * r);
                vertex.uv[1] = vertex.uv[0];

                dataWater->addVertex(vertex);
            }
        }

        MeshData::createPlaneStrip(dataWater->getIndexes(), dataWater->getLines(), length, width);
        MeshData::updatePlaneStripNormals((float*)&dataWater->getVertices()->front(), dataWater->getVertices()->size(), (vindex*)&dataWater->getIndexes()->front(), dataWater->getIndexes()->size());

        dataWater->createVbo();

        Object::updateBoundingBox();
    }
}

void Magic3D::Water::addMaterial(Material* material)
{
    if (waterMesh)
    {
        waterMesh->addMaterial(material);
    }
}

void Magic3D::Water::setSize(int length, int width, float tileLength, float tileWidth)
{
    this->length = length;
    this->width = width;

    this->tileLength = tileLength;
    this->tileWidth = tileWidth;

    needUpdate = true;
}

int Magic3D::Water::getLength()
{
    return length;
}

int Magic3D::Water::getWidth()
{
    return width;
}

float Magic3D::Water::getTileLength()
{
    return tileLength;
}

float Magic3D::Water::getTileWidth()
{
    return tileWidth;
}

Magic3D::Texture* Magic3D::Water::getWaterTexture()
{
    Texture* result = NULL;
    if (getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        result = getMeshes()->at(0)->getMaterials()->at(0)->getTexture(0);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::Water::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* water = root->GetDocument()->NewElement(M3D_WATER_XML);
        root->LinkEndChild(water);

        XMLElement* attribute = water->GetDocument()->NewElement( M3D_WATER_XML_SIZE );
        attribute->SetAttribute(M3D_WATER_XML_LENGTH, length);
        attribute->SetAttribute(M3D_WATER_XML_WIDTH, width);
        attribute->SetAttribute(M3D_WATER_XML_TILE_LENGTH, tileLength);
        attribute->SetAttribute(M3D_WATER_XML_TILE_WIDTH, tileWidth);

        water->LinkEndChild( attribute );
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Water::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* water = root->FirstChildElement(M3D_WATER_XML);

        XMLElement* xml = water->FirstChildElement(M3D_WATER_XML_SIZE);

        if (xml)
        {
            xml->QueryIntAttribute(M3D_WATER_XML_LENGTH, &length);
            xml->QueryIntAttribute(M3D_WATER_XML_WIDTH, &width);
            xml->QueryFloatAttribute(M3D_WATER_XML_TILE_LENGTH, &tileLength);
            xml->QueryFloatAttribute(M3D_WATER_XML_TILE_WIDTH, &tileWidth);

        }

        needUpdate = true;
        needTransform = true;

        generateWater();
    }

    return root;
}
