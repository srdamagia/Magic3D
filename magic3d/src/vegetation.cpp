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

//******************************************************************************

Magic3D::Branch::Branch()
{
    init(Vector3(0.0f, 0.0f, 0.0f), NULL);
}

Magic3D::Branch::Branch(Vector3 head, Branch* parent)
{
    init(head, parent);
}

void Magic3D::Branch::init(Vector3 head, Branch* parent)
{
    this->parent = parent;

    child0 = NULL;
    child1 = NULL;

    this->head = head;
    tangent = Vector3(0.0f, 0.0f, 0.0f);
    length = 1.0f;
    trunk = false;
    radius = 0.0f;
    end = 0;
}

Magic3D::Branch::~Branch()
{
    if (child0)
    {
        delete child0;
        child0 = NULL;
    }

    if (child1)
    {
        delete child1;
        child1 = NULL;
    }

    parent = NULL;
}

float Magic3D::Branch::random(float a)
{
    return fabs(cosf(a + a * a));
}

Magic3D::Vector3 Magic3D::Branch::mirrorBranch(Vector3 vec, Vector3 norm, float branchFactor)
{
    Vector3 v = cross(norm,cross(vec, norm));
    float s = branchFactor * dot(v, vec);
    return Vector3(vec.getX() - v.getX() * s, vec.getY() - v.getY() * s, vec.getZ() - v.getZ() * s);
}

void Magic3D::Branch::split(int level, int steps, TreeProperties& properties, int l1, int l2)
{
    int rLevel = properties.levels - level;
    Vector3 po = Vector3(0.0f, 0.0f, 0.0f);

    if (this->parent)
    {
        po = this->parent->head;
    }
    else
    {
        this->trunk = true;
    }

    Vector3 so = this->head;
    Vector3 dir = normalize(so - po);

    Vector3 normal = cross(dir, Vector3(dir.getZ(),dir.getX(),dir.getY()));
    Vector3 tangent = cross(dir,normal);
    float r = random((float)rLevel * 10.0f + (float)l1 * 5.0f + (float)l2 + (float)properties.seed);
    //float r2 = random((float)rLevel * 10.0f + l1 * 5.0f + l2 + 1.0f + (float)properties.seed);
    float clumpmax = properties.clumpMax;
    float clumpmin = properties.clumpMin;

    Vector3 adj = (normal * r) + (tangent * (1.0f - r));
    if (r > 0.5f)
    {
        adj = adj * -1.0f;
    }

    float clump = (clumpmax - clumpmin) * r + clumpmin;
    Vector3 newdir = normalize((adj * (1.0f - clump)) + (dir * clump));

    Vector3 newdir2 = mirrorBranch(newdir, dir, properties.branchFactor);

    if (r > 0.5)
    {
        Vector3 tmp = newdir;
        newdir = newdir2;
        newdir2 = tmp;
    }
    if (steps > 0)
    {
        float angle = (float)steps / (float)properties.treeSteps * 2.0f * M_PI * properties.twistRate;
        newdir2 = normalize(Vector3(sinf(angle), r, cosf(angle)));
    }

    float growAmount = (float)level * (float)level / ((float)properties.levels * (float)properties.levels) * properties.growAmount;
    float dropAmount = (float)rLevel * properties.dropAmount;
    float sweepAmount = (float)rLevel * properties.sweepAmount;
    newdir = normalize(newdir + Vector3(sweepAmount, dropAmount + growAmount, 0.0f));
    newdir2 = normalize(newdir2 + Vector3(sweepAmount, dropAmount + growAmount, 0.0f));

    Vector3 head0 = so + newdir * length;
    Vector3 head1 = so + newdir2 * length;
    child0 = new Branch(head0, this);
    child1 = new Branch(head1, this);
    child0->length = powf(this->length, properties.lengthFalloffPower) * properties.lengthFalloffFactor;
    child1->length = powf(this->length, properties.lengthFalloffPower) * properties.lengthFalloffFactor;

    if (level > 0)
    {
        if (steps > 0)
        {
            child0->head = head + Vector3((r - 0.5f) * 2.0f * properties.trunkKink, properties.climbRate, (r - 0.5f) * 2.0f * properties.trunkKink);
            child0->trunk = true;
            child0->length = length * properties.taperRate;
            child0->split(level, steps - 1, properties, l1 + 1, l2);
        }
        else
        {
            child0->split(level - 1, 0, properties, l1 + 1, l2);
        }

        child1->split(level - 1, 0, properties, l1, l2 + 1);
    }
}

//******************************************************************************
Magic3D::Vegetation::Vegetation(const Vegetation& vegetation, std::string name) : Object(vegetation, name)
{
    this->properties = vegetation.properties;
    this->terrainName = vegetation.terrainName;
    this->density = vegetation.density;
    this->minHeight = vegetation.minHeight;
    this->maxHeight = vegetation.maxHeight;

    vegetationMesh = getMeshes()->at(0);
    twigMesh = getMeshes()->at(1);
    grassMesh = getMeshes()->at(2);

    if (getType() == eOBJECT_VEGETATION)
    {
        generateVegetation();
    }

    needUpdate = true;
}

Magic3D::Vegetation::Vegetation(std::string name) : Object(eOBJECT_VEGETATION, eRENDER_3D, name)
{
    init();
}

Magic3D::Vegetation::Vegetation(OBJECT type, std::string name) : Object(type, eRENDER_3D, name)
{
    init();
}

void Magic3D::Vegetation::init()
{
    addDefaultTrees();

    properties = trees[0];    

    density = 1.0f;
    minHeight = TERRAIN_HEIGHT_MIN;
    maxHeight = TERRAIN_HEIGHT_MAX;

    vegetationMesh = new Mesh(eMESH_TRIANGLES, TRUNK_MESH_NAME);
    vegetationMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_TRUNK));
    vegetationMesh->setIlluminated(true);
    vegetationMesh->setFogged(true);
    vegetationMesh->setCastShadows(true);
    vegetationMesh->setReceiveShadows(true);

    addMesh(vegetationMesh);

    twigMesh = new Mesh(eMESH_TRIANGLES, TWIG_MESH_NAME, true);
    twigMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_TWIG));
    twigMesh->setIlluminated(true);
    twigMesh->setFogged(true);
    twigMesh->setCastShadows(true);
    twigMesh->setReceiveShadows(true);

    addMesh(twigMesh);

    grassMesh = new Mesh(eMESH_TRIANGLES, GRASS_MESH_NAME, true);
    grassMesh->addMaterial(ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_GRASS));
    grassMesh->setIlluminated(true);
    grassMesh->setFogged(true);
    grassMesh->setCastShadows(true);
    grassMesh->setReceiveShadows(true);

    addMesh(grassMesh);

    if (getType() == eOBJECT_VEGETATION)
    {
        generateVegetation();
    }    

    needUpdate = true;
}


Magic3D::Vegetation::~Vegetation()
{
    vegetationMesh = NULL;
    twigMesh = NULL;
    grassMesh = NULL;
}

void* Magic3D::Vegetation::spawn(std::string name) const
{
    return (new Vegetation(*this, name));
}

bool Magic3D::Vegetation::updateMeshes()
{
    /*if (getMeshes()->size() > 0 && getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        MeshData* data = vegetationMesh->getData();

        float* buffer = data->mapBuffer();

        float x = width * 0.5f;
        float y = height * 0.5f;

        ColorRGBA color = getMeshes()->at(0)->getMaterials()->at(0)->getColorDiffuse();
        vegetationMesh->getData()->setQuad(buffer, 0, -x, -y, width, height, color);

        data->unmapBuffer();
    }*/

    return true;
}

void Magic3D::Vegetation::generateVegetation()
{
    if (vegetationMesh && twigMesh && grassMesh)
    {

        MeshData* dataVegetation = vegetationMesh->getData();
        dataVegetation->clear();
        dataVegetation->deleteVbo();

        MeshData* dataTwig = twigMesh->getData();
        dataTwig->clear();
        dataTwig->deleteVbo();

        MeshData* dataGrass = grassMesh->getData();
        dataGrass->clear();
        dataGrass->deleteVbo();

        Terrain* terrain = NULL;
        if (!terrainName.empty())
        {
            Object* obj = ResourceManager::getObjects()->get(terrainName);
            if (obj && obj->getType() == eOBJECT_TERRAIN)
            {
                terrain = static_cast<Terrain*>(obj);
            }
        }
        if (terrain)
        {
            bool first = true;
            float x = (float)terrain->getWidth() * terrain->getTileWidth() * -0.5f;
            float y = (float)terrain->getLength()* terrain->getTileLength() * -0.5f;

            int treeIndex = 0;
            int oldseed = properties.seed;
            for (int r = 0; r < terrain->getLength(); r++)
            {
                for (int c = 0; c < terrain->getWidth(); c++)
                {

                    int index = r * terrain->getLength() + c;
                    float height = terrain->getHeight(index);
                    Vector3 pos = Vector3(x + c * terrain->getTileWidth(), height - ((terrain->getTileLength() + terrain->getTileWidth()) * 0.5f * 0.25f) , y + r * terrain->getTileLength());

                    int random = (int)(Branch::random(index + properties.seed) * 100.0f);
                    if ((random <= density || first) && height >= minHeight && height <= maxHeight)
                    {
                        first = false;
                        int startVertex = dataVegetation->getVertices()->size();
                        int startIndex = dataVegetation->getIndexes()->size();                        

                        if (trees.size() > 0)
                        {
                            properties = trees[treeIndex++];
                            if (treeIndex >= (int)trees.size())
                            {
                                treeIndex = 0;
                            }
                        }

                        Branch* root = new Branch(Vector3(0.0f, properties.trunkLength + Branch::random(index + properties.seed) * properties.trunkLength * 0.5f, 0.0f), NULL);
                        root->length = properties.initalBranchLength;
                        root->split(properties.levels, properties.treeSteps, properties);
                        createForks(root, properties.maxRadius);
                        createTwigs(pos, root);
                        doFaces(root, startVertex);
                        calcNormals(startVertex, startIndex);
                        updateForks(pos, startVertex);

                        delete root;

                        properties.seed++;
                    }
                    else if (height >= minHeight && height <= maxHeight)
                    {
                        pos.setY(height);
                        createGrass(pos, terrain->getTileLength(), terrain->getTileWidth());
                    }
                }
            }
            properties.seed = oldseed;
        }
        else
        {
            Branch* root = new Branch(Vector3(0.0f, properties.trunkLength, 0.0f), NULL);
            root->length = properties.initalBranchLength;
            root->split(properties.levels, properties.treeSteps, properties);
            createForks(root, properties.maxRadius);
            createTwigs(Vector3(0.0f, 0.0f, 0.0f), root);
            //doUVS(root);
            doFaces(root);            
            calcNormals();
            updateForks(Vector3(0.0f, 0.0f, 0.0f), 0);

            delete root;
        }

        dataVegetation->update(this);
        dataTwig->update(this);
        dataGrass->update(this);

        dataVegetation->createVbo();
        dataTwig->createVbo();
        dataGrass->createVbo();

        Object::updateBoundingBox();

        if (getRigidBody())
        {
            dataVegetation->setTriangleMesh(NULL);
            Physics::getInstance()->add(this);
        }
    }
}

void Magic3D::Vegetation::calcNormals(int startVertex, int startIndex)
{
    MeshData* dataVegetation = vegetationMesh->getData();
    std::vector<Vertex3D>* vertices = dataVegetation->getVertices();
    std::vector< std::vector<Vector3> > allNormals;
    allNormals.resize(vertices->size() - startVertex);

    for (int i = startIndex; i < dataVegetation->getIndexesCount(); i += 3)
    {
        vindex face[3];
        face[0] = dataVegetation->getIndexes()->at(i);
        face[1] = dataVegetation->getIndexes()->at(i + 1);
        face[2] = dataVegetation->getIndexes()->at(i + 2);

        Vector3 norm = normalize(cross(vertices->at(face[1]).position - vertices->at(face[2]).position, vertices->at(face[1]).position - vertices->at(face[0]).position));
        allNormals[face[0] - startVertex].push_back(norm);
        allNormals[face[1] - startVertex].push_back(norm);
        allNormals[face[2] - startVertex].push_back(norm);
    }

    //float maxX = 0.0;
    //float maxY = 0.0;
    size_t nsize = allNormals.size();
    for (size_t i = 0; i < nsize; i++)
    {
        Vector3 total = Vector3(0.0f, 0.0f, 0.0f);
        unsigned int l = allNormals[i].size();
        for (unsigned int j = 0; j < l; j++)
        {
            total = total + allNormals[i][j] * (1.0f / (float)l);
        }
        vertices->at(i + startVertex).normal = total;

        float uvu = 1.0f / 4.0f;
        float uvv = 1.0f / 4.0f;

        int r = properties.trunkIndex / 4;
        int c = properties.trunkIndex % 4;
        Vector3 uvInc = Vector3((float)c * uvu, 1.0f - ((float)(r + 1) * uvv), 0.0f);

        Vector3 uv = Vector3(vertices->at(i + startVertex).uv[0].u, vertices->at(i + startVertex).uv[0].v, 1.0f);

        /*float fx = Math::fract(uv.getX());
        float fy = Math::fract(uv.getY());

        if (uv.getX() >= 1.0f && fx == 0)
        {
            uv.setX(1.0f);
        }
        else
        {
            uv.setX(fx);
        }
        if (uv.getY() >= 1.0f && fy == 0)
        {
            uv.setY(1.0f);
        }
        else
        {
            uv.setY(fy);
        }*/


        Vector3 uvs =  mulPerElem(uv, Vector3(uvu, uvv, 1.0)) + uvInc;
        vertices->at(i + startVertex).uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vertices->at(i + startVertex).uv[1] = Texture2D(uvs.getX(), uvs.getY());
    }
}

void Magic3D::Vegetation::doFaces(Branch* branch, int startIndex, bool invert)
{
    MeshData* dataVegetation = vegetationMesh->getData();
    std::vector<Vertex3D>* vertices = dataVegetation->getVertices();

    int segments = properties.segments;
    vindex v1 = startIndex;
    vindex v2 = startIndex;
    vindex v3 = startIndex;
    vindex v4 = startIndex;

    float texX = 1.0f;

    float texY = 1.0f;

    if (invert)
    {
        texY = 1.0f - properties.vMultiplier;
    }

    if (!branch->parent)
    {
        Vector3 tangent = normalize(cross(branch->child0->head - branch->head, branch->child1->head - branch->head));
        Vector3 normal = normalize(branch->head);
        float angle = acosf(Math::limitRange(-1.0f, dot(tangent, Vector3(-1.0f, 0.0f, 0.0f)), 1.0f));
        if (dot(cross(Vector3(-1.0f, 0.0f, 0.0f), tangent), normal) > 0)
        {
            angle = 2.0f * M_PI - angle;
        }
        int segOffset = (int)(angle / M_PI / 2.0f * (float)segments);
        for (int i = 0; i < segments; i++)
        {
            v1 = branch->ring0[i];
            v2 = branch->root[(i + segOffset + 1) % segments];
            v3 = branch->root[(i + segOffset) % segments];
            v4 = branch->ring0[(i + 1) % segments];

            int index = startIndex + ((i + segOffset) % segments);
            dataVegetation->addTriangle(TriangleIndexes(v1, v4, v3));
            dataVegetation->addTriangle(TriangleIndexes(v4, v2, v3));
            vertices->at(index).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, 0.0f);
            vertices->at(index).uv[1] = vertices->at(index).uv[0];
            //float len = length(vertices->at(branch->ring0[i]).position - vertices->at(branch->root[(i + segOffset) % segments]).position) * properties.vMultiplier;
            //vertices->at(branch->ring0[i]).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, len);
            vertices->at(branch->ring0[i]).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, 1.0f);
            vertices->at(branch->ring0[i]).uv[1] = vertices->at(branch->ring0[i]).uv[0];
            //vertices->at(branch->ring2[i]).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, len);
            vertices->at(branch->ring2[i]).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, 1.0f);
            vertices->at(branch->ring2[i]).uv[1] = vertices->at(branch->ring2[i]).uv[0];
        }
    }

    if (branch->child0->ring0.size() > 0)
    {
        bool first0 = true;
        bool first1 = true;
        int segOffset0 = 0;
        int segOffset1 = 0;
        float match0 = 0.0f;
        float match1 = 0.0f;

        Vector3 vec1 = normalize(vertices->at(branch->ring1[0]).position - branch->head);
        Vector3 vec2 = normalize(vertices->at(branch->ring2[0]).position - branch->head);

        vec1 = Math::scaleInDirection(vec1, normalize(branch->child0->head - branch->head), 0.0f);
        vec2 = Math::scaleInDirection(vec2, normalize(branch->child1->head - branch->head), 0.0f);

        for (int i = 0; i < segments; i++)
        {
            Vector3 d = normalize(vertices->at(branch->child0->ring0[i]).position - branch->child0->head);
            float l = dot(d, vec1);
            if (first0 || l > match0){
                first0 = false;
                match0 = l;
                segOffset0 = segments - i;
            }
            d = normalize(vertices->at(branch->child1->ring0[i]).position - branch->child1->head);
            l = dot(d, vec2);
            if (first1 || l > match1){
                first1 = false;
                match1 = l;
                segOffset1 = segments - i;
            }
        }

        //float UVScale = properties.maxRadius / branch->radius;

        for (int i = 0; i < segments; i++)
        {
            v1 = branch->child0->ring0[i];
            v2 = branch->ring1[(i + segOffset0 + 1) % segments];
            v3 = branch->ring1[(i + segOffset0) % segments];
            v4 = branch->child0->ring0[(i + 1) % segments];

            dataVegetation->addTriangle(TriangleIndexes(v1, v4, v3));
            dataVegetation->addTriangle(TriangleIndexes(v4, v2, v3));
            v1 = branch->child1->ring0[i];
            v2 = branch->ring2[(i + segOffset1 + 1) % segments];
            v3 = branch->ring2[(i + segOffset1) % segments];
            v4 = branch->child1->ring0[(i + 1) % segments];

            dataVegetation->addTriangle(TriangleIndexes(v1, v2, v3));
            dataVegetation->addTriangle(TriangleIndexes(v1, v4, v2));

            //float len1 = length(vertices->at(branch->child0->ring0[i]).position - vertices->at(branch->ring1[(i + segOffset0) % segments]).position) * UVScale;
            Texture2D uv1 = vertices->at(branch->ring1[(i + segOffset0 - 1) % segments]).uv[0];

            //vertices->at(branch->child0->ring0[i]).uv[0] = Texture2D(uv1.u, uv1.v + len1 * properties.vMultiplier);
            vertices->at(branch->child0->ring0[i]).uv[0] = Texture2D(uv1.u, texY);
            vertices->at(branch->child0->ring0[i]).uv[1] = vertices->at(branch->child0->ring0[i]).uv[0];
            //vertices->at(branch->child0->ring2[i]).uv[0] = Texture2D(uv1.u, uv1.v + len1 * properties.vMultiplier);
            vertices->at(branch->child0->ring2[i]).uv[0] = Texture2D(uv1.u, texY);
            vertices->at(branch->child0->ring2[i]).uv[1] = vertices->at(branch->child0->ring2[i]).uv[0];

            //float len2 = length(vertices->at(branch->child1->ring0[i]).position - vertices->at(branch->ring2[(i + segOffset1) % segments]).position) * UVScale;
            Texture2D uv2 = vertices->at(branch->ring2[(i + segOffset1 - 1) % segments]).uv[0];

            //vertices->at(branch->child1->ring0[i]).uv[0] = Texture2D(uv2.u, uv2.v + len2 * properties.vMultiplier);
            vertices->at(branch->child1->ring0[i]).uv[0] = Texture2D(uv2.u, texY);
            vertices->at(branch->child1->ring0[i]).uv[1] = vertices->at(branch->child1->ring0[i]).uv[0];
            //vertices->at(branch->child1->ring2[i]).uv[0] = Texture2D(uv2.u, uv2.v + len2 * properties.vMultiplier);
            vertices->at(branch->child1->ring2[i]).uv[0] = Texture2D(uv2.u, texY);
            vertices->at(branch->child1->ring2[i]).uv[1] = vertices->at(branch->child1->ring2[i]).uv[0];
        }

        doFaces(branch->child0, startIndex, !invert);
        doFaces(branch->child1, startIndex, !invert);
    }
    else
    {
        for (int i = 0; i < segments; i++)
        {
            dataVegetation->addTriangle(TriangleIndexes(branch->child0->end, branch->ring1[(i + 1) % segments], branch->ring1[i]));
            dataVegetation->addTriangle(TriangleIndexes(branch->child1->end, branch->ring2[(i + 1) % segments], branch->ring2[i]));

            //float len = length(vertices->at(branch->child0->end).position - vertices->at(branch->ring1[i]).position);
            //vertices->at(branch->child0->end).uv[0] = Texture2D(fabs((float)i / (float)segments - 1.0f - 0.5f) * texX, len * properties.vMultiplier);
            vertices->at(branch->child0->end).uv[0] = Texture2D(fabs((float)i / (float)segments - 1.0f - 0.5f) * texX, texY);
            vertices->at(branch->child0->end).uv[1] = vertices->at(branch->child0->end).uv[0];
            //len = length(vertices->at(branch->child1->end).position - vertices->at(branch->ring2[i]).position);
            //vertices->at(branch->child1->end).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, len * properties.vMultiplier);
            vertices->at(branch->child1->end).uv[0] = Texture2D(fabs((float)i / (float)segments - 0.5f) * texX, texY);
            vertices->at(branch->child1->end).uv[1] = vertices->at(branch->child1->end).uv[0];
        }
    }
}

void Magic3D::Vegetation::createTwigs(Vector3 position, Branch* branch)
{
    MeshData* dataTwig = twigMesh->getData();
    std::vector<Vertex3D>* vertices = dataTwig->getVertices();

    if (!branch->child0)
    {
        Vector3 tangent = normalize(cross(branch->parent->child0->head - branch->parent->head, branch->parent->child1->head - branch->parent->head));
        Vector3 binormal = normalize(branch->head - branch->parent->head);

        int index = vertices->size();
        Vertex3D vert1;
        Vertex3D vert2;
        Vertex3D vert3;
        Vertex3D vert4;
        /*Vertex3D vert5;
        Vertex3D vert6;
        Vertex3D vert7;
        Vertex3D vert8;*/

        vert1.position = position + ((branch->head + (tangent * properties.twigScale)) + (binormal * (properties.twigScale * 2.0f - branch->length)));
        vert2.position = position + ((branch->head + (tangent * -properties.twigScale)) + (binormal * (properties.twigScale * 2.0f - branch->length)));
        vert3.position = position + ((branch->head + (tangent * -properties.twigScale)) + (binormal * -branch->length));
        vert4.position = position + ((branch->head + (tangent * properties.twigScale)) + (binormal * -branch->length));

        /*vert8.position = position + ((branch->head + (tangent * properties.twigScale)) + (binormal * (properties.twigScale * 2.0f - branch->length)));
        vert7.position = position + ((branch->head + (tangent * -properties.twigScale)) + (binormal * (properties.twigScale * 2.0f - branch->length)));
        vert6.position = position + ((branch->head + (tangent * -properties.twigScale)) + (binormal * -branch->length));
        vert5.position = position + ((branch->head + (tangent * properties.twigScale)) + (binormal * -branch->length));*/

        Vector3 normal = normalize(cross(vert1.position - vert3.position, vert2.position - vert3.position));
        //Vector3 normal2 = normalize(cross(vert7.position - vert6.position, vert8.position - vert6.position));

        vert1.normal = normal;
        vert2.normal = normal;
        vert3.normal = normal;
        vert4.normal = normal;

        /*vert8.normal = normal2;
        vert7.normal = normal2;
        vert6.normal = normal2;
        vert5.normal = normal2;*/

        float uvu = 1.0f / 4.0f;
        float uvv = 1.0f / 4.0f;

        int r = properties.twigIndex / 4;
        int c = properties.twigIndex % 4;
        Vector3 uvInc = Vector3((float)c * uvu, 1.0f - ((float)(r + 1) * uvv), 0.0f);

        Vector3 uvs =  mulPerElem(Vector3(0.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert1.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert1.uv[1] = vert1.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert2.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert2.uv[1] = vert2.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert3.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert3.uv[1] = vert3.uv[0];
        uvs =  mulPerElem(Vector3(0.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert4.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert4.uv[1] = vert4.uv[0];

        /*uvs =  mulPerElem(Vector3(0.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert8.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert8.uv[1] = vert8.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert7.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert7.uv[1] = vert7.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert6.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert6.uv[1] = vert6.uv[0];
        uvs =  mulPerElem(Vector3(0.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert5.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert5.uv[1] = vert5.uv[0];*/

        vertices->push_back(vert1);
        vertices->push_back(vert2);
        vertices->push_back(vert3);
        vertices->push_back(vert4);
        /*vertices->push_back(vert5);
        vertices->push_back(vert6);
        vertices->push_back(vert7);
        vertices->push_back(vert8);*/

        dataTwig->addTriangle(TriangleIndexes(index, index + 1, index + 2));
        dataTwig->addTriangle(TriangleIndexes(index + 3, index, index + 2));

        /*dataTwig->addTriangle(TriangleIndexes(index + 5, index + 6, index + 7));
        dataTwig->addTriangle(TriangleIndexes(index + 5, index + 7, index + 4));*/
    }
    else
    {
        createTwigs(position, branch->child0);
        createTwigs(position, branch->child1);
    }
}

void Magic3D::Vegetation::createGrass(Vector3 position, float tileLength, float tileWidth)
{
    MeshData* dataGrass = grassMesh->getData();
    std::vector<Vertex3D>* vertices = dataGrass->getVertices();

    float lpos = Branch::random(length(position));

    float mult = 1.0f;
    if (lpos > 0.5f)
    {
        mult = -1.0f;
    }

    float grassSize = 0.5f * lpos + 0.5f;
    Vector3 pos = position + Vector3(tileWidth * 0.5f * lpos * mult, grassSize * 0.75f, tileLength * 0.5f * lpos * mult);
    for (int i = 0; i < 2; i++)
    {
        Vector3 n = normalize(Vector3(0.5f, 0.0f, 0.5f));

        if (i == 1)
        {
            n = cross(n, Vector3(0.0f, 1.0f, 0.0f));
        }
        Vector3 tangent = n;
        Vector3 binormal = Vector3(0.0f, 1.0f, 0.0f);        

        int index = vertices->size();
        Vertex3D vert1;
        Vertex3D vert2;
        Vertex3D vert3;
        Vertex3D vert4;
        /*Vertex3D vert5;
        Vertex3D vert6;
        Vertex3D vert7;
        Vertex3D vert8;*/

        vert1.position = pos + (((tangent * grassSize)) + (binormal * grassSize));
        vert2.position = pos + (((tangent * -grassSize)) + (binormal * grassSize));
        vert3.position = pos + (((tangent * -grassSize)) + (binormal * -grassSize));
        vert4.position = pos + (((tangent * grassSize)) + (binormal * -grassSize));

        //vert1.color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
        //vert2.color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
        vert3.color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
        vert4.color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

        /*vert8.position = pos + (((tangent * grassSize)) + (binormal * grassSize));
        vert7.position = pos + (((tangent * -grassSize)) + (binormal * grassSize));
        vert6.position = pos + (((tangent * -grassSize)) + (binormal * -grassSize));
        vert5.position = pos + (((tangent * grassSize)) + (binormal * -grassSize));*/

        Vector3 normal = normalize(cross(vert1.position - vert3.position, vert2.position - vert3.position));
        //Vector3 normal2 = normalize(cross(vert7.position - vert6.position, vert8.position - vert6.position));

        vert1.normal = normal;
        vert2.normal = normal;
        vert3.normal = normal;
        vert4.normal = normal;

        /*vert8.normal = normal2;
        vert7.normal = normal2;
        vert6.normal = normal2;
        vert5.normal = normal2;*/

        float uvu = 1.0f / 2.0f;
        float uvv = 1.0f / 2.0f;

        float ri = Branch::random(pos.getX() + pos.getY() + pos.getZ());

        int tex = 0;        
        if (ri < 0.05f)
        {
            tex = 2;
        }
        else if (ri > 0.995f)
        {
            tex = 3;
        }
        else if (ri > 0.5f)
        {
            tex = 1;
        }

        int r = tex / 2;
        int c = tex % 2;
        Vector3 uvInc = Vector3((float)c * uvu, 1.0f - ((float)(r + 1) * uvv), 0.0f);

        Vector3 uvs =  mulPerElem(Vector3(0.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert1.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert1.uv[1] = vert1.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert2.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert2.uv[1] = vert2.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert3.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert3.uv[1] = vert3.uv[0];
        uvs =  mulPerElem(Vector3(0.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert4.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert4.uv[1] = vert4.uv[0];

        /*uvs =  mulPerElem(Vector3(0.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert8.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert8.uv[1] = vert8.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 1.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert7.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert7.uv[1] = vert7.uv[0];
        uvs =  mulPerElem(Vector3(1.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert6.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert6.uv[1] = vert6.uv[0];
        uvs =  mulPerElem(Vector3(0.0f, 0.0f, 1.0f), Vector3(uvu, uvv, 1.0)) + uvInc;
        vert5.uv[0] = Texture2D(uvs.getX(), uvs.getY());
        vert5.uv[1] = vert5.uv[0];*/

        vertices->push_back(vert1);
        vertices->push_back(vert2);
        vertices->push_back(vert3);
        vertices->push_back(vert4);
        /*vertices->push_back(vert5);
        vertices->push_back(vert6);
        vertices->push_back(vert7);
        vertices->push_back(vert8);*/

        dataGrass->addTriangle(TriangleIndexes(index, index + 1, index + 2));
        dataGrass->addTriangle(TriangleIndexes(index + 3, index, index + 2));

        /*dataGrass->addTriangle(TriangleIndexes(index + 5, index + 6, index + 7));
        dataGrass->addTriangle(TriangleIndexes(index + 5, index + 7, index + 4));*/
    }
}

void Magic3D::Vegetation::createForks(Branch* branch, float radius)
{
    MeshData* dataVegetation = vegetationMesh->getData();
    std::vector<Vertex3D>* vertices = dataVegetation->getVertices();

    Vertex3D vert;    

    branch->radius = radius;

    if (radius > branch->length)
    {
        radius = branch->length;
    }

    int segments = properties.segments;

    float segmentAngle = M_PI * 2.0f / (float)segments;

    if (!branch->parent)
    {
        vert.color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
        //create the root of the tree
        Vector3 axis = Vector3(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < segments; i++)
        {
            Vector3 vec = Math::vectorAxisAngle(Vector3(-1.0f, 0.0f, 0.0f), axis, -segmentAngle * (float)i);
            branch->root.push_back(vertices->size());
            vert.position = (vec * (radius / properties.radiusFalloffRate));
            vertices->push_back(vert);
        }
    }

    if (branch->trunk)
    {
        vert.color = ColorRGBA(0.1f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        vert.color = ColorRGBA(0.2f, 0.0f, 0.0f, 1.0f);
    }

    //cross the branches to get the left
    //add the branches to get the up
    if (branch->child0)
    {
        Vector3 axis = normalize(branch->head);

        if (branch->parent)
        {
            axis = normalize(branch->head - branch->parent->head);
        }

        Vector3 axis1 = normalize(branch->head - branch->child0->head);
        Vector3 axis2 = normalize(branch->head - branch->child1->head);
        Vector3 tangent = normalize(cross(axis1, axis2));
        branch->tangent = tangent;

        Vector3 axis3 = normalize(cross(tangent, normalize((axis1 * -1.0f) + (axis2 * -1.0f))));
        Vector3 dir = Vector3(axis2.getX(), 0.0f, axis2.getZ());
        Vector3 centerloc = branch->head + (dir * -properties.maxRadius / 2.0f);

        float scale = properties.radiusFalloffRate;

        if (branch->child0->trunk || branch->trunk)
        {
            scale = 1.0f / properties.taperRate;
        }

        //main segment ring
        int linch0 = vertices->size();
        branch->ring0.push_back(linch0);
        branch->ring2.push_back(linch0);

        vert.position = (centerloc + (tangent * (radius * scale)));
        vertices->push_back(vert);

        int start = vertices->size() - 1;
        Vector3 d1 = Math::vectorAxisAngle(tangent, axis2, 1.57f);
        Vector3 d2 = normalize(cross(tangent, axis));
        float s = 1.0f / dot(d1, d2);
        for (int i = 1; i < segments / 2; i++)
        {
            Vector3 vec = Math::vectorAxisAngle(tangent, axis2, segmentAngle * (float)i);
            branch->ring0.push_back(start + i);
            branch->ring2.push_back(start + i);
            vec = Math::scaleInDirection(vec, d2, s);
            vert.position = (centerloc + (vec * (radius * scale)));
            vertices->push_back(vert);
        }
        int linch1 = vertices->size();
        branch->ring0.push_back(linch1);
        branch->ring1.push_back(linch1);
        vert.position = (centerloc + (tangent * (-radius * scale)));
        vertices->push_back(vert);
        for(int i = segments / 2 + 1; i < segments; i++){
            Vector3 vec = Math::vectorAxisAngle(tangent, axis1, segmentAngle * (float)i);
            int l = vertices->size();
            branch->ring0.push_back(l);
            branch->ring1.push_back(l);
            vert.position = (centerloc + (vec * (radius * scale)));
            vertices->push_back(vert);
        }
        branch->ring1.push_back(linch0);
        branch->ring2.push_back(linch1);
        start = vertices->size() - 1;
        for (int i = 1; i < segments / 2; i++){
            Vector3 vec = Math::vectorAxisAngle(tangent, axis3, segmentAngle * (float)i);
            branch->ring1.push_back(start + i);
            branch->ring2.push_back(start + (segments / 2 - i));
            Vector3 v = vec * (radius * scale);
            vert.position = (centerloc + v);
            vertices->push_back(vert);
        }

        //child radius is related to the brans direction and the length of the branch
        /*float length0 = length(branch->head - branch->child0->head);
        float length1 = length(branch->head - branch->child1->head);*/

        float radius0 = 1.0f * radius * properties.radiusFalloffRate;
        float radius1 = 1.0f * radius * properties.radiusFalloffRate;
        if (branch->child0->trunk)
        {
            radius0 = radius * properties.taperRate;
        }
        createForks(branch->child0, radius0);
        createForks(branch->child1, radius1);
    }
    else
    {
        //add points for the ends of braches
        branch->end = vertices->size();
        //branch.head=addVec(branch.head,scaleVec([this.properties.xBias,this.properties.yBias,this.properties.zBias],branch.length*3));
        vert.position = branch->head;
        vert.color = ColorRGBA(0.3f, 0.0f, 0.0f, 1.0f);
        vertices->push_back(vert);

    }
}

void Magic3D::Vegetation::updateForks(Vector3 position, int startIndex)
{
    MeshData* dataVegetation = vegetationMesh->getData();
    std::vector<Vertex3D>* vertices = dataVegetation->getVertices();

    for (int i = startIndex; i < (int)vertices->size(); i++)
    {
        vertices->at(i).position += position;
    }
}

void Magic3D::Vegetation::addMaterial(Material* material)
{
    if (vegetationMesh)
    {
        vegetationMesh->addMaterial(material);
    }
}

void Magic3D::Vegetation::setTreeProperties(TreeProperties properties)
{
    this->properties = properties;
}

Magic3D::TreeProperties& Magic3D::Vegetation::getTreeProperties()
{
    return properties;
}

void Magic3D::Vegetation::setTerrain(std::string terrain)
{
    this->terrainName = terrain;
}

const std::string& Magic3D::Vegetation::getTerrain()
{
    return terrainName;
}

void Magic3D::Vegetation::setMinHeight(float min)
{
    minHeight = min;
}

float Magic3D::Vegetation::getMinHeight()
{
    return minHeight;
}

void Magic3D::Vegetation::setMaxHeight(float max)
{
    maxHeight = max;
}

float Magic3D::Vegetation::getMaxHeight()
{
    return maxHeight;
}

void Magic3D::Vegetation::setDensity(float density)
{
    this->density = density;
}

float Magic3D::Vegetation::getDensity()
{
    return density;
}

std::vector<Magic3D::TreeProperties>* Magic3D::Vegetation::getTreesList()
{
    return &trees;
}

Magic3D::Texture* Magic3D::Vegetation::getVegetationTexture()
{
    Texture* result = NULL;
    if (getMeshes()->at(0)->getMaterials()->size() > 0)
    {
        result = getMeshes()->at(0)->getMaterials()->at(0)->getTexture(0);
    }
    return result;
}

Magic3D::XMLElement* Magic3D::Vegetation::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* vegetation = root->GetDocument()->NewElement(M3D_VEGETATION_XML);
        root->LinkEndChild(vegetation);

        saveString(vegetation, M3D_VEGETATION_XML_TERRAIN, !getTerrain().empty() ? getTerrain().c_str() : M3D_XML_NULL);

        saveFloat(vegetation, M3D_VEGETATION_XML_MIN_HEIGHT, minHeight);
        saveFloat(vegetation, M3D_VEGETATION_XML_MAX_HEIGHT, maxHeight);
        saveFloat(vegetation, M3D_VEGETATION_XML_DENSITY,    density);

        if (trees.empty())
        {
            trees.push_back(properties);
        }

        size_t tsize = trees.size();
        for (unsigned int i = 0; i < tsize; i++)
        {
            XMLElement* tree = root->GetDocument()->NewElement(M3D_VEGETATION_XML_TREE);
            vegetation->LinkEndChild(tree);

            TreeProperties tmp = trees[i];

            saveFloat(tree, M3D_VEGETATION_XML_clumpMax,            tmp.clumpMax           );
            saveFloat(tree, M3D_VEGETATION_XML_clumpMin,            tmp.clumpMin           );
            saveFloat(tree, M3D_VEGETATION_XML_lengthFalloffFactor, tmp.lengthFalloffFactor);
            saveFloat(tree, M3D_VEGETATION_XML_branchFactor,        tmp.branchFactor       );
            saveFloat(tree, M3D_VEGETATION_XML_radiusFalloffRate,   tmp.radiusFalloffRate  );
            saveFloat(tree, M3D_VEGETATION_XML_climbRate,           tmp.climbRate          );
            saveFloat(tree, M3D_VEGETATION_XML_trunkKink,           tmp.trunkKink          );
            saveFloat(tree, M3D_VEGETATION_XML_maxRadius,           tmp.maxRadius          );
            saveFloat(tree, M3D_VEGETATION_XML_taperRate,           tmp.taperRate          );
            saveFloat(tree, M3D_VEGETATION_XML_initalBranchLength,  tmp.initalBranchLength );
            saveFloat(tree, M3D_VEGETATION_XML_trunkLength,         tmp.trunkLength        );
            saveFloat(tree, M3D_VEGETATION_XML_dropAmount,          tmp.dropAmount         );
            saveFloat(tree, M3D_VEGETATION_XML_growAmount,          tmp.growAmount         );
            saveFloat(tree, M3D_VEGETATION_XML_vMultiplier,         tmp.vMultiplier        );
            saveFloat(tree, M3D_VEGETATION_XML_twigScale,           tmp.twigScale          );
            saveFloat(tree, M3D_VEGETATION_XML_sweepAmount,         tmp.sweepAmount        );
            saveFloat(tree, M3D_VEGETATION_XML_lengthFalloffPower,  tmp.lengthFalloffPower );
            saveFloat(tree, M3D_VEGETATION_XML_twistRate,           tmp.twistRate          );
            saveInt(tree,   M3D_VEGETATION_XML_treeSteps,           tmp.treeSteps          );
            saveInt(tree,   M3D_VEGETATION_XML_segments,            tmp.segments           );
            saveInt(tree,   M3D_VEGETATION_XML_levels,              tmp.levels             );
            saveInt(tree,   M3D_VEGETATION_XML_seed,                tmp.seed               );
            saveInt(tree,   M3D_VEGETATION_XML_trunkIndex,          tmp.trunkIndex         );
            saveInt(tree,   M3D_VEGETATION_XML_twigIndex,           tmp.twigIndex          );
        }
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Vegetation::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* vegetation = root->FirstChildElement(M3D_VEGETATION_XML);        

        minHeight = loadFloat(vegetation, M3D_VEGETATION_XML_MIN_HEIGHT);
        maxHeight = loadFloat(vegetation, M3D_VEGETATION_XML_MAX_HEIGHT);
        density   = loadFloat(vegetation, M3D_VEGETATION_XML_DENSITY);

        std::string name = loadString(vegetation, M3D_VEGETATION_XML_TERRAIN);
        if (name.compare(M3D_XML_NULL) == 0)
        {
            generateVegetation();
        }

        XMLElement* tree = vegetation->FirstChildElement(M3D_VEGETATION_XML_TREE);
        if (tree)
        {
            trees.clear();
        }
        while (tree)
        {
            TreeProperties tmp;

            tmp.clumpMax            = loadFloat(tree, M3D_VEGETATION_XML_clumpMax           );
            tmp.clumpMin            = loadFloat(tree, M3D_VEGETATION_XML_clumpMin           );
            tmp.lengthFalloffFactor = loadFloat(tree, M3D_VEGETATION_XML_lengthFalloffFactor);
            tmp.branchFactor        = loadFloat(tree, M3D_VEGETATION_XML_branchFactor       );
            tmp.radiusFalloffRate   = loadFloat(tree, M3D_VEGETATION_XML_radiusFalloffRate  );
            tmp.climbRate           = loadFloat(tree, M3D_VEGETATION_XML_climbRate          );
            tmp.trunkKink           = loadFloat(tree, M3D_VEGETATION_XML_trunkKink          );
            tmp.maxRadius           = loadFloat(tree, M3D_VEGETATION_XML_maxRadius          );
            tmp.taperRate           = loadFloat(tree, M3D_VEGETATION_XML_taperRate          );
            tmp.initalBranchLength  = loadFloat(tree, M3D_VEGETATION_XML_initalBranchLength );
            tmp.trunkLength         = loadFloat(tree, M3D_VEGETATION_XML_trunkLength        );
            tmp.dropAmount          = loadFloat(tree, M3D_VEGETATION_XML_dropAmount         );
            tmp.growAmount          = loadFloat(tree, M3D_VEGETATION_XML_growAmount         );
            tmp.vMultiplier         = loadFloat(tree, M3D_VEGETATION_XML_vMultiplier        );
            tmp.twigScale           = loadFloat(tree, M3D_VEGETATION_XML_twigScale          );
            tmp.sweepAmount         = loadFloat(tree, M3D_VEGETATION_XML_sweepAmount        );
            tmp.lengthFalloffPower  = loadFloat(tree, M3D_VEGETATION_XML_lengthFalloffPower );
            tmp.twistRate           = loadFloat(tree, M3D_VEGETATION_XML_twistRate          );
            tmp.treeSteps           = loadInt(tree,   M3D_VEGETATION_XML_treeSteps          );
            tmp.segments            = loadInt(tree,   M3D_VEGETATION_XML_segments           );
            tmp.levels              = loadInt(tree,   M3D_VEGETATION_XML_levels             );
            tmp.seed                = loadInt(tree,   M3D_VEGETATION_XML_seed               );
            tmp.trunkIndex          = loadInt(tree,   M3D_VEGETATION_XML_trunkIndex         );
            tmp.twigIndex           = loadInt(tree,   M3D_VEGETATION_XML_twigIndex          );

            trees.push_back(tmp);

            tree = tree->NextSiblingElement(M3D_VEGETATION_XML_TREE);
        }

        if (!trees.empty())
        {
            setTreeProperties(trees[0]);
        }

        terrainName = loadString(vegetation, M3D_VEGETATION_XML_TERRAIN);
        if (terrainName.compare(M3D_XML_NULL) == 0)
        {
            terrainName.clear();
        }

        needUpdate = true;
        needTransform = true;
    }

    return root;
}

void Magic3D::Vegetation::addDefaultTrees()
{
    TreeProperties p;

    p.clumpMax            = 0.454f;
    p.clumpMin            = 0.404f;
    p.lengthFalloffFactor = 0.85f;
    p.branchFactor        = 2.45f;
    p.radiusFalloffRate   = 0.73f;
    p.climbRate           = 0.371f;
    p.trunkKink           = 0.093f;
    p.maxRadius           = 0.139f;
    p.taperRate           = 0.947f;
    p.initalBranchLength  = 0.49f;
    p.trunkLength         = 2.4f;
    p.dropAmount          = -0.1f;
    p.growAmount          = 0.235f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.39f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.99f;
    p.twistRate           = 3.02f;
    p.treeSteps           = 5;
    p.segments            = 6;
    p.levels              = 5;
    p.seed                = 262;
    p.trunkIndex          = 0;
    p.twigIndex           = 1;

    trees.push_back(p);

    p.clumpMax            = 0.449f;
    p.clumpMin            = 0.404f;
    p.lengthFalloffFactor = 0.85f;
    p.branchFactor        = 2.75f;
    p.radiusFalloffRate   = 0.66f;
    p.climbRate           = 0.626f;
    p.trunkKink           = 0.108f;
    p.maxRadius           = 0.269f;
    p.taperRate           = 0.876f;
    p.initalBranchLength  = 0.5f;
    p.trunkLength         = 1.55f;
    p.dropAmount          = 0.07f;
    p.growAmount          = -0.005f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.47f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.99f;
    p.twistRate           = 2.7f;
    p.treeSteps           = 4;
    p.segments            = 10;
    p.levels              = 5;
    p.seed                = 861;
    p.trunkIndex          = 10;
    p.twigIndex           = 2;

    trees.push_back(p);

    p.clumpMax            = 0.454f;
    p.clumpMin            = 0.246f;
    p.lengthFalloffFactor = 0.85f;
    p.branchFactor        = 3.2f;
    p.radiusFalloffRate   = 0.73;
    p.climbRate           = 0.41f;
    p.trunkKink           = 0.0f;
    p.maxRadius           = 0.111f;
    p.taperRate           = 0.835f;
    p.initalBranchLength  = 0.49f;
    p.trunkLength         = 2.45;
    p.dropAmount          = 0.09f;
    p.growAmount          = 0.235f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.44f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.99f;
    p.twistRate           = 2.06;
    p.treeSteps           = 5;
    p.segments            = 6;
    p.levels              = 5;
    p.seed                = 152;
    p.trunkIndex          = 2;
    p.twigIndex           = 7;

    trees.push_back(p);

    p.clumpMax            = 0.414f;
    p.clumpMin            = 0.282f;
    p.lengthFalloffFactor = 0.98f;
    p.branchFactor        = 2.2f;
    p.radiusFalloffRate   = 0.71f;
    p.climbRate           = 0.39f;
    p.trunkKink           = 0.0f;
    p.maxRadius           = 0.096f;
    p.taperRate           = 0.958f;
    p.initalBranchLength  = 0.5f;
    p.trunkLength         = 1.95f;
    p.dropAmount          = 0.24f;
    p.growAmount          = 0.044f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.28f;
    p.sweepAmount         = 0.0f;
    p.lengthFalloffPower  = 1.08f;
    p.twistRate           = 2.97f;
    p.treeSteps           = 5;
    p.segments            = 8;
    p.levels              = 5;
    p.seed                = 499;
    p.trunkIndex          = 12;
    p.twigIndex           = 14;

    trees.push_back(p);

    p.clumpMax            = 0.556f;
    p.clumpMin            = 0.404f;
    p.lengthFalloffFactor = 1.0f;
    p.branchFactor        = 3.5f;
    p.radiusFalloffRate   = 0.73f;
    p.climbRate           = 0.419f;
    p.trunkKink           = 0.093f;
    p.maxRadius           = 0.139f;
    p.taperRate           = 0.947f;
    p.initalBranchLength  = 0.12f;
    p.trunkLength         = 1.75;
    p.dropAmount          = 0.18f;
    p.growAmount          = -0.108f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.71f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.7f;
    p.twistRate           = 3.53f;
    p.treeSteps           = 5;
    p.segments            = 8;
    p.levels              = 4;
    p.seed                = 267;
    p.trunkIndex          = 5;
    p.twigIndex           = 7;

    trees.push_back(p);

    p.clumpMax            = 0.53f;
    p.clumpMin            = 0.419f;
    p.lengthFalloffFactor = 0.73f;
    p.branchFactor        = 3.4f;
    p.radiusFalloffRate   = 0.73f;
    p.climbRate           = 0.472f;
    p.trunkKink           = 0.06f;
    p.maxRadius           = 0.168f;
    p.taperRate           = 0.835f;
    p.initalBranchLength  = 0.65f;
    p.trunkLength         = 2.2f;
    p.dropAmount          = -0.16f;
    p.growAmount          = 0.128f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.52f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.76f;
    p.twistRate           = 1.29f;
    p.treeSteps           = 5;
    p.segments            = 6;
    p.levels              = 5;
    p.seed                = 519;
    p.trunkIndex          = 2;
    p.twigIndex           = 13;

    trees.push_back(p);

    p.clumpMax            = 0.454f;
    p.clumpMin            = 0.454f;
    p.lengthFalloffFactor = 0.85f;
    p.branchFactor        = 3.2f;
    p.radiusFalloffRate   = 0.73f;
    p.climbRate           = 0.322f;
    p.trunkKink           = 0.0f;
    p.maxRadius           = 0.105f;
    p.taperRate           = 0.964f;
    p.initalBranchLength  = 0.49f;
    p.trunkLength         = 2.25f;
    p.dropAmount          = 0.09f;
    p.growAmount          = 0.235f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.39f;
    p.sweepAmount         = 0.051f;
    p.lengthFalloffPower  = 0.99f;
    p.twistRate           = 1.5f;

    p.treeSteps           = 5;
    p.segments            = 8;
    p.levels              = 5;
    p.seed                = 152;
    p.trunkIndex          = 3;
    p.twigIndex           = 8;

    trees.push_back(p);

    p.clumpMax            = 0.556f;
    p.clumpMin            = 0.404f;
    p.lengthFalloffFactor = 0.94f;
    p.branchFactor        = 3.5f;
    p.radiusFalloffRate   = 0.73f;
    p.climbRate           = 0.419;
    p.trunkKink           = 0.093f;
    p.maxRadius           = 0.139f;
    p.taperRate           = 0.947f;
    p.initalBranchLength  = 0.26f;
    p.trunkLength         = 2.2;
    p.dropAmount          = -0.15f;
    p.growAmount          = 0.28f;
    p.vMultiplier         = 1.0f;
    p.twigScale           = 0.7f;
    p.sweepAmount         = 0.01f;
    p.lengthFalloffPower  = 0.7f;
    p.twistRate           = 3.32f;
    p.treeSteps           = 5;
    p.segments            = 8;
    p.levels              = 4;
    p.seed                = 267;
    p.trunkIndex          = 0;
    p.twigIndex           = 15;

    trees.push_back(p);
}
