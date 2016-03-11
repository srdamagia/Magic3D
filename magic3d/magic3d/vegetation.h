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

#ifndef MAGIC3D_VEGETATION_H
#define MAGIC3D_VEGETATION_H

#include <magic3d/object.h>

namespace Magic3D
{

#define TRUNK_MESH_NAME            "trunk"
#define TWIG_MESH_NAME             "twig"
#define BUSH_MESH_NAME             "bush"
#define GRASS_MESH_NAME            "grass"

#define M3D_VEGETATION_XML         "Vegetation"
#define M3D_VEGETATION_XML_TERRAIN    "terrain"
#define M3D_VEGETATION_XML_MIN_HEIGHT "min_height"
#define M3D_VEGETATION_XML_MAX_HEIGHT "max_height"
#define M3D_VEGETATION_XML_DENSITY    "density"
#define M3D_VEGETATION_XML_TREE       "Tree"

#define M3D_VEGETATION_XML_clumpMax            "clumpMax"
#define M3D_VEGETATION_XML_clumpMin            "clumpMin"
#define M3D_VEGETATION_XML_lengthFalloffFactor "lengthFalloffFactor"
#define M3D_VEGETATION_XML_branchFactor        "branchFactor"
#define M3D_VEGETATION_XML_radiusFalloffRate   "radiusFalloffRate"
#define M3D_VEGETATION_XML_climbRate           "climbRate"
#define M3D_VEGETATION_XML_trunkKink           "trunkKink"
#define M3D_VEGETATION_XML_maxRadius           "maxRadius"
#define M3D_VEGETATION_XML_taperRate           "taperRate"
#define M3D_VEGETATION_XML_initalBranchLength  "initalBranchLength"
#define M3D_VEGETATION_XML_trunkLength         "trunkLength"
#define M3D_VEGETATION_XML_dropAmount          "dropAmount"
#define M3D_VEGETATION_XML_growAmount          "growAmount"
#define M3D_VEGETATION_XML_vMultiplier         "vMultiplier"
#define M3D_VEGETATION_XML_twigScale           "twigScale"
#define M3D_VEGETATION_XML_sweepAmount         "sweepAmount"
#define M3D_VEGETATION_XML_lengthFalloffPower  "lengthFalloffPower"
#define M3D_VEGETATION_XML_twistRate           "twistRate"
#define M3D_VEGETATION_XML_treeSteps           "treeSteps"
#define M3D_VEGETATION_XML_segments            "segments"
#define M3D_VEGETATION_XML_levels              "levels"
#define M3D_VEGETATION_XML_seed                "seed"
#define M3D_VEGETATION_XML_trunkIndex          "trunkIndex"
#define M3D_VEGETATION_XML_twigIndex           "twigIndex"

struct TreeProperties
{
    float clumpMax;
    float clumpMin;
    float lengthFalloffFactor;
    float branchFactor;
    float radiusFalloffRate;
    float climbRate;
    float trunkKink;
    float maxRadius;
    float taperRate;
    float initalBranchLength;
    float trunkLength;
    float dropAmount;
    float growAmount;
    float vMultiplier;
    float twigScale;
    float sweepAmount;
    float lengthFalloffPower;
    float twistRate;
    int treeSteps;    
    int segments;
    int levels;    
    int seed;
    int trunkIndex;
    int twigIndex;
};

class Branch
{
private:
    void init(Vector3 head, Branch* parent);
public:
    Branch* child0;
    Branch* child1;
    Branch* parent;
    Vector3 head;
    Vector3 tangent;
    float length;
    bool trunk;
    std::vector<vindex> ring0;
    std::vector<vindex> ring1;
    std::vector<vindex> ring2;
    std::vector<vindex> root;
    float radius;
    int end;

    ~Branch();
    Branch();
    Branch(Vector3 head, Branch* parent);

    static float random(float a);
    Vector3 mirrorBranch(Vector3 vec, Vector3 norm, float branchFactor);
    void split(int level, int steps, TreeProperties& properties, int l1 = 1, int l2 = 1);
};

class Terrain;
class Vegetation : public Object
{
protected:
    std::vector<TreeProperties> trees;
    Mesh* vegetationMesh;
    Mesh* twigMesh;
    Mesh* grassMesh;
    std::string terrainName;

    bool needUpdate;
    float minHeight;
    float maxHeight;
    float density;

    void init();

    Texture* getVegetationTexture();

    Vegetation(const Vegetation& vegetation, std::string name);
private:
    TreeProperties properties;
    void addDefaultTrees();

public:
    Vegetation(std::string name);
    Vegetation(OBJECT type, std::string name);
    virtual ~Vegetation();
    virtual void* spawn(std::string name) const;

    virtual bool updateMeshes();

    void generateVegetation();

    void calcNormals(int startVertex = 0, int startIndex = 0);
    void doFaces(Branch* branch, int startIndex = 0, bool invert = true);
    void createTwigs(Vector3 position,Branch* branch);
    void createForks(Branch* branch, float radius);
    void updateForks(Vector3 position, int startIndex = 0);

    void createGrass(Vector3 position, float tileLength = 1.0f, float tileWidth = 1.0f);

    void addMaterial(Material* material);

    void setTreeProperties(TreeProperties properties);
    TreeProperties& getTreeProperties();

    void setTerrain(std::string terrain);
    const std::string& getTerrain();

    void setMinHeight(float min);
    float getMinHeight();
    void setMaxHeight(float max);
    float getMaxHeight();
    void setDensity(float density);
    float getDensity();

    std::vector<TreeProperties>* getTreesList();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_VEGETATION_H
