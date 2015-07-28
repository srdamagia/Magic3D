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

#ifndef MAGIC3D_OCTREE_H
#define MAGIC3D_OCTREE_H

#define M3D_OCTREE_NODE_MIN_SIZE 2

#include <magic3d/object.h>

namespace Magic3D
{

class Octree
{
private:    
    static int nodesCount;
    static int nodesRenderCount;
    Box region;

    std::list<Object*> objects;
    std::list<Object*> pendingInsertion;
    static std::list<Object*> log;

    Octree* childs[8];

    byte activeNodes;

    int maxLifespan;
    int curLife;

    Octree* parent;

    bool treeReady;
    bool treeBuilt;

    bool inFrustum;

    Octree(const Box& region, std::list<Object*>* objList);
    void init(const Box& region, std::list<Object*>* objList);
    void insert(Object* object);
    void addToList(Object* object);
    void buildTree();
    Octree* createNode(const Box& region, std::list<Object*>* objList);
    Octree* createNode(const Box& region, Object* obj);
    void updateTree();
    void findEnclosingBox();
    void findEnclosingCube();

    //private List<IntersectionRecord> getIntersection(BoundingFrustum frustum, Physical.PhysicalType type = Physical.PhysicalType.ALL);

    bool isRoot();
    bool hasChildren();
    bool isEmpty();

    void createNodesDivision(Box* nodes, const Box& box);

public:
    Octree();
    Octree(const Box& region);
    virtual ~Octree();

    void render();
    void update();

    void add(std::list<Object*>* itemList);
    void add(Object* item, bool update = false);
    void remove(Object* item);
    void clear();
    void teste(bool main = true);

    int getNodesCount();
    int getRenderedNodes();

    void setInFrustum(bool frustum);
    bool isInFrustum();
    void resetChildFrustum();

    const Box& getBoundingBox();
    const std::list<Object*>* getObjects();
    Octree* getChild(int index);
    byte getActiveNodes();

    //std::list<IntersectionRecord> AllIntersections(BoundingFrustum region, Physical.PhysicalType type = Physical.PhysicalType.ALL)
};

}

#endif // MAGIC3D_OCTREE_H
