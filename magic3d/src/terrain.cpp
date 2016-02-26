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
#include <magic3d/image/image_png.h>

//******************************************************************************
Magic3D::Terrain::Terrain(const Terrain& terrain, std::string name) : Object(terrain, name)
{
    if (terrain.bufferData)
    {
        this->bufferDataSize = terrain.bufferDataSize;
        this->bufferData = new float[this->bufferDataSize];
        memcpy(this->bufferData, terrain.bufferData, terrain.bufferDataSize);
    }
    else
    {
        this->bufferData = NULL;
        this->bufferDataSize = 0;
    }

    if (terrain.bufferIndex)
    {
        this->bufferIndexSize = terrain.bufferIndexSize;
        this->bufferIndex = new vindex[this->bufferIndexSize];
        memcpy(this->bufferIndex, terrain.bufferIndex, terrain.bufferIndexSize);
    }
    else
    {
        this->bufferIndex = NULL;
        this->bufferIndexSize = 0;
    }

    this->length = terrain.length;
    this->width = terrain.width;
    this->tileLength = terrain.tileLength;
    this->tileWidth = terrain.tileWidth;
    this->textureLevels = terrain.textureLevels;
    this->textureTile = terrain.textureTile;

    terrainMesh = getMeshes()->at(0);

    if (getType() == eOBJECT_TERRAIN)
    {
        generateTerrain();
    }

    needUpdate = true;
}

Magic3D::Terrain::Terrain(std::string name) : Object(eOBJECT_TERRAIN, eRENDER_3D, name)
{
    init();
}

Magic3D::Terrain::Terrain(OBJECT type, std::string name) : Object(type, eRENDER_3D, name)
{
    init();
}

void Magic3D::Terrain::init()
{
    bufferData = NULL;
    bufferDataSize = 0;

    bufferIndex = NULL;
    bufferIndexSize = 0;

    length = 128;
    width  = 128;

    tileLength = 2.0f;
    tileWidth  = 2.0f;

    textureLevels = 8.0f;
    textureTile = 4.0f;

    terrainMesh = new Mesh(eMESH_TRIANGLES_STRIP, TERRAIN_MESH_NAME);
    terrainMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_TERRAIN));
    terrainMesh->setIlluminated(true);
    terrainMesh->setFogged(true);
    terrainMesh->setCastShadows(true);
    terrainMesh->setReceiveShadows(true);

    addMesh(terrainMesh);

    if (getType() == eOBJECT_TERRAIN)
    {
        generateTerrain();
    }    

    needUpdate = true;
}


Magic3D::Terrain::~Terrain()
{
    terrainMesh = NULL;

    if (bufferData)
    {
        delete[] bufferData;
        bufferData = NULL;
        bufferDataSize = 0;
    }

    if (bufferIndex)
    {
        delete[] bufferIndex;
        bufferIndex = NULL;
        bufferIndexSize = 0;
    }
}

void* Magic3D::Terrain::spawn(std::string name) const
{
    return (new Terrain(*this, name));
}

bool Magic3D::Terrain::updateMeshes()
{
    /*if (getMeshes()->size() > 0 && getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        MeshData* data = terrainMesh->getData();

        float* buffer = data->mapBuffer();

        float x = width * 0.5f;
        float y = height * 0.5f;

        ColorRGBA color = getMeshes()->at(0)->getMaterials()->at(0)->getColorDiffuse();
        terrainMesh->getData()->setQuad(buffer, 0, -x, -y, width, height, color);

        data->unmapBuffer();
    }*/

    return true;
}

float *createMandelbrotImage(int width, int height, float xS, float yS, float rad, int maxIteration)
{
    float *buffer = (float *) malloc(width * height * sizeof(float));
    if (buffer == NULL) {
        fprintf(stderr, "Could not create image buffer\n");
        return NULL;
    }

    // Create Mandelbrot set image

    int xPos, yPos;
    float minMu = maxIteration;
    float maxMu = 0;

    for (yPos=0 ; yPos<height ; yPos++)
    {
        float yP = (yS-rad) + (2.0f*rad/height)*yPos;

        for (xPos=0 ; xPos<width ; xPos++)
        {
            float xP = (xS-rad) + (2.0f*rad/width)*xPos;

            int iteration = 0;
            float x = 0;
            float y = 0;

            while (x*x + y*y <= 4 && iteration < maxIteration)
            {
                float tmp = x*x - y*y + xP;
                y = 2*x*y + yP;
                x = tmp;
                iteration++;
            }

            if (iteration < maxIteration) {
                float modZ = sqrt(x*x + y*y);
                float mu = iteration - (log(log(modZ))) / log(2);
                if (mu > maxMu) maxMu = mu;
                if (mu < minMu) minMu = mu;
                buffer[yPos * width + xPos] = mu;
            }
            else {
                buffer[yPos * width + xPos] = 0;
            }
        }
    }

    // Scale buffer values between 0 and 1
    int count = width * height;
    while (count) {
        count --;
        buffer[count] = (buffer[count] - minMu) / (maxMu - minMu);
    }

    return buffer;
}


float unpack (Magic3D::Vector4 color)
{
    const Magic3D::Vector4 bitShifts = Magic3D::Vector4(1.0f,
                                                        1.0f / TERRAIN_COLOR_PACK,
                                                        1.0f / (TERRAIN_COLOR_PACK * TERRAIN_COLOR_PACK),
                                                        1.0f / (TERRAIN_COLOR_PACK * TERRAIN_COLOR_PACK * TERRAIN_COLOR_PACK));
    return Vectormath::Aos::dot(color, bitShifts);
}

void Magic3D::Terrain::generateTerrain()
{
    /*float *image = createMandelbrotImage(512, 512, -0.802, -0.177, 0.011, 110);
    if (image != NULL) {
        int result = writeImage((char*)ResourceManager::getTextures()->getPath(eTEXTURE_DIFFUSE, "terrain.terrain").c_str(), 512, 512, image, "Terrain");
        free(image);
    }*/

    if (terrainMesh)
    {
        unsigned char* heightsRaw = NULL;
        long numVertices = 0;

        if (!heightMap.empty() && heightMap.rfind(".raw") != std::string::npos)
        {

            std::string fileName = ResourceManager::getTextures()->getTexturePath(eTEXTURE_DIFFUSE, heightMap);
            if (ResourceManager::getInstance()->getPackage())
            {
                Memory mem;
                ResourceManager::getInstance()->unpack(fileName, &mem);

                mem.seeki( 0, std::ios::end );
                numVertices = mem.telli();
                mem.seeki( 0, std::ios::beg );

                heightsRaw = new unsigned char[numVertices];
                mem.read( (char *)heightsRaw, numVertices );
            }
            else
            {
                std::ifstream heightStream;
                heightStream.open( fileName.c_str(), std::ios::binary );
                if ( heightStream.fail() )
                {
                    Log::log(eLOG_FAILURE, "Could not open height file.");
                }

                if (heightStream.is_open())
                {
                    // Get number of vertices
                    heightStream.seekg( 0, std::ios::end );
                    numVertices = heightStream.tellg();
                    heightStream.seekg( 0, std::ios::beg );

                    // Allocate memory and read the data
                    heightsRaw = new unsigned char[numVertices];
                    heightStream.read( (char *)heightsRaw, numVertices );
                    heightStream.close();
                }
            }
        }

        if (heightsRaw)
        {
            width = sqrtl(numVertices);
            length = width;
        }


        float* heights = NULL;
        if (!heightMap.empty())
        {
            std::string fileName = ResourceManager::getTextures()->getTexturePath(eTEXTURE_DIFFUSE, heightMap + TERRAIN_FILE);
            DataBuffer* io;
            bool ready = false;

            if (ResourceManager::getInstance()->getPackage())
            {
                io = new Memory();
                ready = ResourceManager::getInstance()->unpack(fileName, io);
            }
            else
            {
                io = new File();
                ready = static_cast<File*>(io)->open(fileName.c_str(), "rb");
            }

            Image* image = NULL;

            if (ready)
            {
                image = new PNG();
                image->decode(io);
            }
            delete io;

            if (image)
            {
                width = image->getWidth();
                length = image->getHeight();

                heights = new float[image->getWidth() * image->getHeight()];

                byte* src = (byte*)image->getPixels();

                for (int y = image->getHeight() - 1; y >= 0 ; y--)
                {
                    for (int x = 0; x < image->getWidth(); x++)
                    {
                        int index = y * image->getHeight() + x;
                        ColorRGBApack c = image->getPixelColor(src);
                        Vector4 color = Vector4((float)c.r / TERRAIN_COLOR_PACK, (float)c.g / TERRAIN_COLOR_PACK, (float)c.b / TERRAIN_COLOR_PACK, (float)c.a / TERRAIN_COLOR_PACK);
                        float value = unpack(color) * (fabsf(TERRAIN_HEIGHT_MIN) + TERRAIN_HEIGHT_MAX);

                        heights[index] = value - fabsf(TERRAIN_HEIGHT_MIN);

                        src += image->getBPP() / 8;
                    }
                }

                delete image;
            }
        }

        MeshData* dataTerrain = terrainMesh->getData();
        dataTerrain->clear();
        dataTerrain->deleteVbo();

        float x = (float)width * tileWidth * -0.5f;
        float y = (float)length * tileLength * -0.5f;

        for (int r = 0; r < length; r++)
        {
            for (int c = 0; c < width ; c++)
            {
                Vertex3D vertex;
                if (heightsRaw || heights)
                {
                    float h = 0.0f;
                    int index = r * length + c;
                    if (heightsRaw)
                    {
                        h = heightsRaw[index] * 0.5f - 25.0f;
                    }
                    else
                    {
                        h = heights[index];
                    }
                    vertex.position = Vector3(x + (float)c * tileWidth, h, y + (float)r * tileLength);
                }
                else
                {
                    vertex.position = Vector3(x + (float)c * tileWidth, 0.0f, y + (float)r * tileLength);
                }
                vertex.color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
                vertex.normal = Vector3(0.0f, 1.0f, 0.0f);
                vertex.tangent = Vector3(0.0f, 0.0f, 0.0f);

                float pixel = 0.005f;
                float u = (c % (int)textureTile) * (1.0f / textureTile) + (c > 0.0f && c % (int)textureTile == 0 ? 1.0f - pixel : 0.0f);
                if (c > 0 && (int)ceilf((float)c / textureTile) % 2 == 0)
                {
                    u = 1.0 - u + pixel;
                }
                float v = (r % (int)textureTile) * (1.0f / textureLevels) * (1.0f / textureTile) + (r > 0.0f && r % (int)textureTile == 0 ? (1.0f / textureLevels) - pixel : 0.0f);
                if (r > 0 && (int)ceilf((float)r / textureTile) % 2 == 0)
                {
                    v = (1.0f / textureLevels) - v + pixel;
                }

                vertex.uv[0] = Texture2D(u, v);
                vertex.uv[1] = Texture2D(1.0f / (float)width * (float)c, 1.0f - 1.0f / (float)length * (float)r);

                dataTerrain->addVertex(vertex);
            }
        }

        if (heights)
        {
            delete[] heights;
        }

        MeshData::createPlaneStrip(dataTerrain->getIndexes(), dataTerrain->getLines(), length, width);
        MeshData::updatePlaneStripNormals((float*)&dataTerrain->getVertices()->front(), dataTerrain->getVertices()->size(), (vindex*)&dataTerrain->getIndexes()->front(), dataTerrain->getIndexes()->size());

        if (bufferData)
        {
            delete[] bufferData;
            bufferData = NULL;
            bufferDataSize = 0;
        }

        if (bufferIndex)
        {
            delete[] bufferIndex;
            bufferIndex = NULL;
            bufferIndexSize = 0;
        }

        int inc = sizeof(Vertex3D) / sizeof(float);
        bufferDataSize = dataTerrain->getVerticesCount() * inc;

        if (bufferDataSize > 0)
        {
            bufferData = new float[bufferDataSize];
            memcpy(bufferData, &dataTerrain->getVertices()->front(), bufferDataSize * sizeof(float));
        }

        bufferIndexSize = dataTerrain->getIndexesCount();

        if (bufferIndexSize > 0)
        {
            bufferIndex = new vindex[bufferIndexSize];
            memcpy(bufferIndex, &dataTerrain->getIndexes()->front(), bufferIndexSize * sizeof(vindex));
        }

        dataTerrain->createVbo();        

        Object::updateBoundingBox();

        if (getRigidBody())
        {
            dataTerrain->setTriangleMesh(NULL);
            Physics::getInstance()->add(this);
        }
    }
}

void Magic3D::Terrain::addMaterial(Material* material)
{
    if (terrainMesh)
    {
        terrainMesh->addMaterial(material);
    }
}

void Magic3D::Terrain::setSize(int length, int width, float tileLength, float tileWidth)
{
    this->length = length;
    this->width = width;

    this->tileLength = tileLength;
    this->tileWidth = tileWidth;

    needUpdate = true;
}

int Magic3D::Terrain::getLength()
{
    return length;
}

int Magic3D::Terrain::getWidth()
{
    return width;
}

float Magic3D::Terrain::getTileLength()
{
    return tileLength;
}

float Magic3D::Terrain::getTileWidth()
{
    return tileWidth;
}

void Magic3D::Terrain::setHeight(float x, float z, float height)
{
    if (bufferData && terrainMesh)
    {
        int tx = (int)roundf(x / tileWidth  + (float)width * 0.5f);
        int tz = (int)roundf(z / tileLength + (float)length * 0.5f);

        int inc = sizeof(Vertex3D) / sizeof(float);
        int index = tz * length + tx;

        if (index >= 0 && index < bufferDataSize / inc)
        {
            float* v = bufferData + index * inc + 1;

            float& py = *v;
            py = height;

            if (py > TERRAIN_HEIGHT_MAX)
            {
                py = TERRAIN_HEIGHT_MAX;
            }
            if (py < TERRAIN_HEIGHT_MIN)
            {
                py = TERRAIN_HEIGHT_MIN;
            }

            Renderer::getInstance()->updateVBOVertices(terrainMesh->getData()->getRenderID().data, bufferData, bufferDataSize * sizeof(float));
        }
    }
}

float Magic3D::Terrain::getHeight(float x, float z)
{
    float result = 0.0f;

    if (bufferData && terrainMesh)
    {
        int tx = (int)roundf(x / tileWidth  + (float)width * 0.5f);
        int tz = (int)roundf(z / tileLength + (float)length * 0.5f);

        int index = tz * length + tx;

        result = getHeight(index);
    }

    return result;
}

float Magic3D::Terrain::getHeight(int index)
{
    float result = 0.0f;

    if (bufferData && terrainMesh)
    {
        int inc = sizeof(Vertex3D) / sizeof(float);
        if (index >= 0 && index < bufferDataSize / inc)
        {
            float* v = bufferData + index * inc + 1;

            float& py = *v;
            result = py;
        }
    }

    return result;
}

void Magic3D::Terrain::modify(float x, float z, float radius, float height, bool flatten, bool square)
{
    if (bufferData && terrainMesh)
    {
        int tx = (int)roundf(x / tileWidth  + (float)width * 0.5f);
        int tz = (int)roundf(z / tileLength + (float)length * 0.5f);
        int rx = (int)roundf(radius / tileWidth);
        int rz = (int)roundf(radius / tileLength);

        int inc = sizeof(Vertex3D) / sizeof(float);

        std::vector<vindex> vertices;
        for (int r = tz - rz; r < tz + rz; r++)
        {
            for (int c = tx - rx; c < tx + rx; c++)
            {
                if (r < 0 || r >= length || c < 0 || c >= width)
                {
                    continue;
                }
                float pd = dist(Point3((float)c, 0.0f, (float)r), Point3((float)tx, 0.0f, (float)tz)) * 2.0f / radius;
                float h = height;

                if (fabsf(pd) <= 1.0f || square)
                {
                    if (!flatten)
                    {
                        h = height * 0.5f + cos(pd * 3.14f) * height * 0.5f;
                    }
                    int index = r * length + c;

                    if (index >= 0 && index < bufferDataSize / inc)
                    {
                        float* v = bufferData + index * inc + 1;

                        float& py = *v;
                        if (flatten)
                        {
                            py = h;
                        }
                        else
                        {
                            py += h;
                        }

                        if (py > TERRAIN_HEIGHT_MAX)
                        {
                            py = TERRAIN_HEIGHT_MAX;
                        }
                        if (py < TERRAIN_HEIGHT_MIN)
                        {
                            py = TERRAIN_HEIGHT_MIN;
                        }

                        vertices.push_back(index);
                    }
                }
            }
        }

        if (vertices.size() > 0)
        {
            Renderer::getInstance()->updateVBOVertices(terrainMesh->getData()->getRenderID().data, bufferData, bufferDataSize * sizeof(float));            
        }
    }
}

void Magic3D::Terrain::finishModify()
{
    MeshData::updatePlaneStripNormals(bufferData, bufferDataSize, bufferIndex, bufferIndexSize);
    Renderer::getInstance()->updateVBOVertices(terrainMesh->getData()->getRenderID().data, bufferData, bufferDataSize * sizeof(float));
    if (getRigidBody())
    {
        terrainMesh->getData()->setTriangleMesh(NULL);
        Physics::getInstance()->add(this);
    }
}

Magic3D::Texture* Magic3D::Terrain::getTerrainTexture()
{
    Texture* result = NULL;
    if (getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        result = getMeshes()->at(0)->getMaterials()->at(0)->getTexture(0);
    }
    return result;
}

void Magic3D::Terrain::setHeightMap(std::string heightMap)
{
    this->heightMap = heightMap;
}

const std::string& Magic3D::Terrain::getHeightMap()
{
    return heightMap;
}

void Magic3D::Terrain::loadFromHeightMap()
{
    generateTerrain();
}

void pack (png_byte *ptr, float val)
{
    const Magic3D::Vector4 bias = Magic3D::Vector4(1.0f / TERRAIN_COLOR_PACK,
                                                   1.0f / TERRAIN_COLOR_PACK,
                                                   1.0f / TERRAIN_COLOR_PACK,
                                                   0.0f);

    float r = val;
    float g = Magic3D::Math::fract(r * TERRAIN_COLOR_PACK);
    float b = Magic3D::Math::fract(g * TERRAIN_COLOR_PACK);
    float a = Magic3D::Math::fract(b * TERRAIN_COLOR_PACK);
    Magic3D::Vector4 color = Magic3D::Vector4(r, g, b, a);

    color = color - (Vectormath::Aos::mulPerElem(Magic3D::Vector4(color.getY(), color.getZ(), color.getW(), color.getW()), bias));

    ptr[0] = (byte)(color.getX() * TERRAIN_COLOR_PACK);
    ptr[1] = (byte)(color.getY() * TERRAIN_COLOR_PACK);
    ptr[2] = (byte)(color.getZ() * TERRAIN_COLOR_PACK);
    ptr[3] = (byte)(color.getW() * TERRAIN_COLOR_PACK);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclobbered"
int Magic3D::Terrain::saveToHeightMap(std::string name)
{
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;    

    // Open file for writing (binary mode)
    fp = fopen(ResourceManager::getTextures()->getTexturePath(eTEXTURE_DIFFUSE, name + ".terrain").c_str(), "wb");
    if (fp == NULL)
    {
        Log::logFormat(eLOG_FAILURE, "Could not open file %s for writing", name.c_str());
        code = 1;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        Log::log(eLOG_FAILURE, "Could not allocate write struct");
        code = 2;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        Log::log(eLOG_FAILURE, "Could not allocate info struct");
        code = 3;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        Log::log(eLOG_FAILURE, "Error during png creation");
        code = 4;
    }    

    if (code == 0)
    {
        png_bytep row = NULL;
        png_init_io(png_ptr, fp);

        // Write header (8 bit colour depth)
        png_set_IHDR(png_ptr, info_ptr, width, length, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        // Set title
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = (char*)"Title";
        title_text.text = (char*)"Magic3D - Terrain";
        png_set_text(png_ptr, info_ptr, &title_text, 1);

        png_write_info(png_ptr, info_ptr);

        // Allocate memory for one row (4 bytes per pixel - RGBA)
        row = (png_bytep) malloc(4 * width * sizeof(png_byte));

        int inc = sizeof(Vertex3D) / sizeof(float);
        // Write image data
        int x, y;
        for (y = 0; y < length; y++)
        {
            for (x = 0 ; x < width ; x++)
            {
                int index = y * length + x;
                float v = *(bufferData + index * inc + 1) + fabsf(TERRAIN_HEIGHT_MIN);
                v /= (fabsf(TERRAIN_HEIGHT_MIN) + TERRAIN_HEIGHT_MAX);

                pack(&(row[x * 4]), v);
            }
            png_write_row(png_ptr, row);
        }

        // End write
        png_write_end(png_ptr, NULL);
        if (row != NULL)
        {
            free(row);
        }
    }

    if (fp != NULL)
    {
        fclose(fp);
    }
    if (info_ptr != NULL)
    {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    }
    if (png_ptr != NULL)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    }    

    return code;
}
#pragma GCC diagnostic pop

Magic3D::XMLElement* Magic3D::Terrain::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* terrain = root->GetDocument()->NewElement(M3D_TERRAIN_XML);
        root->LinkEndChild(terrain);

        XMLElement* attribute = terrain->GetDocument()->NewElement( M3D_TERRAIN_XML_SIZE );
        attribute->SetAttribute(M3D_TERRAIN_XML_LENGTH, length);
        attribute->SetAttribute(M3D_TERRAIN_XML_WIDTH, width);
        attribute->SetAttribute(M3D_TERRAIN_XML_TILE_LENGTH, tileLength);
        attribute->SetAttribute(M3D_TERRAIN_XML_TILE_WIDTH, tileWidth);                

        terrain->LinkEndChild( attribute );

        saveString(terrain, M3D_TERRAIN_XML_HEIGHT_MAP, heightMap);
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Terrain::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* terrain = root->FirstChildElement(M3D_TERRAIN_XML);

        XMLElement* xml = terrain->FirstChildElement(M3D_TERRAIN_XML_SIZE);

        if (xml)
        {
            xml->QueryIntAttribute(M3D_TERRAIN_XML_LENGTH, &length);
            xml->QueryIntAttribute(M3D_TERRAIN_XML_WIDTH, &width);
            xml->QueryFloatAttribute(M3D_TERRAIN_XML_TILE_LENGTH, &tileLength);
            xml->QueryFloatAttribute(M3D_TERRAIN_XML_TILE_WIDTH, &tileWidth);

        }

        heightMap = loadString(terrain, M3D_TERRAIN_XML_HEIGHT_MAP);
        if (heightMap.compare(M3D_XML_NULL) == 0)
        {
            heightMap.clear();
        }

        needUpdate = true;
        needTransform = true;

        generateTerrain();
    }

    return root;
}
