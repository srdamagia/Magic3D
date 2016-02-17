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

#include <magic3d/renderer/renderer.h>
#include <magic3d/octree.h>

int Magic3D::Octree::nodesCount = 0;
int Magic3D::Octree::nodesRenderCount = 0;
std::list<Magic3D::Object*> Magic3D::Octree::log;

bool SortName(Magic3D::Object* lo, Magic3D::Object* ro)
{
  unsigned int i=0;
  std::string first = lo->getName();
  std::string second = ro->getName();
  while ( (i<first.length()) && (i<second.length()) )
  {
    if (tolower(first[i])<tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  return ( first.length() < second.length() );
}

Magic3D::Octree::Octree()
{
    Box b;
    init(b, NULL);
}

Magic3D::Octree::Octree(const Box& region)
{
    init(region, NULL);
}

Magic3D::Octree::Octree(const Box& region, std::list<Object*>* objList)
{
    init(region, objList);
}

void Magic3D::Octree::init(const Box& region, std::list<Object*>* objList)
{
    nodesCount++;

    if (objList)
    {
        std::list<Object*>::const_iterator it_o = objList->begin();
        while (it_o != objList->end())
        {
            addToList(*it_o++);
        }
    }

    this->region = region;
    for (int i = 0; i < 8; i++)
    {
        childs[i] = NULL;
    }
    parent = NULL;

    activeNodes = 0;

    maxLifespan = 8;
    curLife = -1;

    treeReady = false;
    treeBuilt = false;

    inFrustum = false;
}

Magic3D::Octree::~Octree()
{
    setInFrustum(false);
    nodesCount--;
    for (int i = 0; i < 8; i++)
    {
        if (childs[i])
        {
            delete childs[i];
            childs[i] = NULL;
        }
    }
}

void Magic3D::Octree::render()
{
    if (isInFrustum())
    {
        Vector3 verts[8];
        ColorRGBA lime = ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
        /*if (!isInFrustum())
        {
            lime = ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
        }*/
        ColorRGBA red = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);

        if (Renderer::getInstance()->isShowingOctree())
        {
            verts[0] = region.corners[0];
            verts[1] = Vector3(region.corners[0].getX(), region.corners[0].getY(), region.corners[1].getZ()); //Z
            verts[2] = Vector3(region.corners[0].getX(), region.corners[1].getY(), region.corners[0].getZ()); //Y
            verts[3] = Vector3(region.corners[1].getX(), region.corners[0].getY(), region.corners[0].getZ()); //X

            verts[7] = region.corners[1];
            verts[4] = Vector3(region.corners[1].getX(), region.corners[1].getY(), region.corners[0].getZ()); //Z
            verts[5] = Vector3(region.corners[1].getX(), region.corners[0].getY(), region.corners[1].getZ()); //Y
            verts[6] = Vector3(region.corners[0].getX(), region.corners[1].getY(), region.corners[1].getZ()); //X

            Renderer::getInstance()->drawLine(verts[0], verts[1], false, lime);
            Renderer::getInstance()->drawLine(verts[0], verts[2], false, lime);
            Renderer::getInstance()->drawLine(verts[0], verts[3], false, lime);
            Renderer::getInstance()->drawLine(verts[7], verts[4], false, lime);
            Renderer::getInstance()->drawLine(verts[7], verts[5], false, lime);
            Renderer::getInstance()->drawLine(verts[7], verts[6], false, lime);

            Renderer::getInstance()->drawLine(verts[1], verts[6], false, lime);
            Renderer::getInstance()->drawLine(verts[1], verts[5], false, lime);
            Renderer::getInstance()->drawLine(verts[4], verts[2], false, lime);
            Renderer::getInstance()->drawLine(verts[4], verts[3], false, lime);
            Renderer::getInstance()->drawLine(verts[2], verts[6], false, lime);
            Renderer::getInstance()->drawLine(verts[3], verts[5], false, lime);
        }

        for (int flags = activeNodes, index = 0; flags > 0; flags >>= 1, index++)
        {
            if ((flags & 1) == 1) childs[index]->render();
        }

        if (Renderer::getInstance()->isShowingOctreeObjects())
        {            
            std::list<Object*>::const_iterator it_o = objects.begin();
            while (it_o != objects.end())
            {
                Object* obj = *it_o++;

                Box b = obj->getBoundingBoxAlignedAxis();
                verts[0] = b.corners[0];
                verts[1] = Vector3(b.corners[0].getX(), b.corners[0].getY(), b.corners[1].getZ()); //Z
                verts[2] = Vector3(b.corners[0].getX(), b.corners[1].getY(), b.corners[0].getZ()); //Y
                verts[3] = Vector3(b.corners[1].getX(), b.corners[0].getY(), b.corners[0].getZ()); //X

                verts[7] = b.corners[1];
                verts[4] = Vector3(b.corners[1].getX(), b.corners[1].getY(), b.corners[0].getZ()); //Z
                verts[5] = Vector3(b.corners[1].getX(), b.corners[0].getY(), b.corners[1].getZ()); //Y
                verts[6] = Vector3(b.corners[0].getX(), b.corners[1].getY(), b.corners[1].getZ()); //X

                Renderer::getInstance()->drawLine(verts[0], verts[1], false, red);
                Renderer::getInstance()->drawLine(verts[0], verts[2], false, red);
                Renderer::getInstance()->drawLine(verts[0], verts[3], false, red);
                Renderer::getInstance()->drawLine(verts[7], verts[4], false, red);
                Renderer::getInstance()->drawLine(verts[7], verts[5], false, red);
                Renderer::getInstance()->drawLine(verts[7], verts[6], false, red);

                Renderer::getInstance()->drawLine(verts[1], verts[6], false, red);
                Renderer::getInstance()->drawLine(verts[1], verts[5], false, red);
                Renderer::getInstance()->drawLine(verts[4], verts[2], false, red);
                Renderer::getInstance()->drawLine(verts[4], verts[3], false, red);
                Renderer::getInstance()->drawLine(verts[2], verts[6], false, red);
                Renderer::getInstance()->drawLine(verts[3], verts[5], false, red);
            }
        }
    }
}

void Magic3D::Octree::update()
{
    if (treeBuilt)
    {
        if (objects.empty() && pendingInsertion.empty())
        {
            if (!hasChildren())
            {
                if (curLife == -1)
                {
                    curLife = maxLifespan;
                }
                else if (curLife > 0)
                {
                    curLife--;
                }
            }
        }
        else
        {
            if (curLife != -1)
            {
                if(maxLifespan <= 64)
                {
                    maxLifespan *= 2;
                }
                curLife = -1;
            }
        }

        std::list<Object*> movedObjects;

        std::list<Object*>::const_iterator it_o = objects.begin();
        while (it_o != objects.end())
        {
            Object* object = *it_o++;
            if (object->isNeedingUpdateOctree())
            {
                movedObjects.push_back(object);
            }
        }

        for (int flags = activeNodes, index = 0; flags > 0; flags >>= 1, index++)
        {
            if ((flags & 1) == 1) childs[index]->update();
        }

        std::list<Object*>::const_iterator it_m = movedObjects.begin();
        while (it_m != movedObjects.end())
        {
            Object* movedObj = *it_m++;
            Octree* current = this;

            if (movedObj->getOctree())
            {
                movedObj->getOctree()->remove(movedObj);
            }
            movedObj->updateOctree();

            Box b = movedObj->getBoundingBoxAlignedAxis();
            if (!current->region.contains(b))
            {
                if (current->parent != NULL)
                {
                    current = current->parent;
                }
            }

            current->insert(movedObj);
        }

        for (int flags = activeNodes, index = 0; flags > 0; flags >>= 1, index++)
        {
            if ((flags & 1) == 1 && childs[index]->curLife == 0)
            {
                if (childs[index]->isEmpty())
                {
                    delete childs[index];
                    childs[index] = NULL;
                    activeNodes ^= (byte)(1 << index);
                }
            }
        }
    }

    if (!treeBuilt || !pendingInsertion.empty())
    {
        updateTree();
    }

    render();
}

void Magic3D::Octree::add(std::list<Object*>* itemList)
{
    std::list<Object*>::const_iterator it_o = itemList->begin();
    while (it_o != itemList->end())
    {
        Object* object = *it_o++;
        pendingInsertion.push_back(object);
        treeReady = false;
    }
}

void Magic3D::Octree::add(Object* item, bool update)
{
    if (item->getType() != eOBJECT_CAMERA)
    {
        pendingInsertion.push_back(item);
        treeReady = false;
        if (update)
        {
            updateTree();
        }
    }
}

void Magic3D::Octree::addToList(Object* object)
{
    if (object && object->getOctree() != this)
    {
        if (object->getOctree())
        {            
            object->getOctree()->remove(object);
        }
        objects.push_back(object);
        object->setOctree(this);
    }
}

void Magic3D::Octree::remove(Object* item)
{
    if (item)
    {
        pendingInsertion.remove(item);
        objects.remove(item);
        item->setOctree(NULL);
    }
}

void Magic3D::Octree::clear()
{
    std::list<Object*>::const_iterator it_o = objects.begin();
    while (it_o != objects.end())
    {
        Object* obj = *it_o++;
        obj->setOctree(NULL);
    }
    objects.clear();
    pendingInsertion.clear();

    region.corners[0] = Vector3(0.0f, 0.0f, 0.0f);
    region.corners[1] = Vector3(0.0f, 0.0f, 0.0f);

    parent = NULL;

    activeNodes = 0;

    maxLifespan = 8;
    curLife = -1;

    treeReady = false;
    treeBuilt = false;

    for (int i = 0; i < 8; i++)
    {
        if (childs[i])
        {
            childs[i]->clear();
            delete childs[i];
            childs[i] = NULL;
        }
    }
}

void Magic3D::Octree::print(bool main)
{
    std::list<Object*>::const_iterator it_o = objects.begin();
    while (it_o != objects.end())
    {
        Object* obj = *it_o++;
        log.push_back(obj);
    }

    it_o = pendingInsertion.begin();
    while (it_o != pendingInsertion.end())
    {
        Object* obj = *it_o++;
        log.push_back(obj);
    }

    for (int i = 0; i < 8; i++)
    {
        if (childs[i])
        {
            childs[i]->print(false);
        }
    }

    if (main)
    {
        log.sort(SortName);
        std::list<Object*>::const_iterator it_o = log.begin();
        while (it_o != log.end())
        {
            Object* obj = *it_o++;
            Log::logFormat(eLOG_RENDERER, "%d - %s", obj->getOctree(), obj->getName().c_str());
        }
    }
}

void Magic3D::Octree::insert(Object* object)
{
    /*if (objects.size() <= 1 && activeNodes == 0)
    {
        addToList(object);
        return;
    }*/

    Vector3 dimensions = region.corners[1] - region.corners[0];
    if (dimensions.getX() <= M3D_OCTREE_NODE_MIN_SIZE && dimensions.getY() <= M3D_OCTREE_NODE_MIN_SIZE && dimensions.getZ() <= M3D_OCTREE_NODE_MIN_SIZE)
    {
        addToList(object);
        return;
    }

    Box b = object->getBoundingBoxAlignedAxis();
    if (region.contains(b))
    {
        Box octant[8];
        createNodesDivision(octant, region);

        bool found = false;
        for(int a = 0; a < 8; a++)
        {
            if (octant[a].contains(b))
            {
                if (childs[a])
                {
                    childs[a]->region = octant[a];
                    childs[a]->insert(object);
                }
                else
                {
                    childs[a] = createNode(octant[a], object);
                    activeNodes |= (byte)(1 << a);
                    childs[a]->buildTree();
                }
                found = true;
                break;
            }
        }

        if(!found)
        {
            addToList(object);
        }
    }
    else
    {
        if (!parent)
        {
            region.corners[0] = Vector3(0.0f, 0.0f, 0.0f);
            region.corners[1] = Vector3(0.0f, 0.0f, 0.0f);
            addToList(object);
            buildTree();
        }
        else
        {
            parent->insert(object);
        }
    }
}

void Magic3D::Octree::buildTree()
{
    /*if (objects.size() <= 1 && activeNodes == 0)
    {
        return;
    }*/

    Vector3 dimensions = region.corners[1] - region.corners[0];

    if (length(dimensions) == 0.0f)
    {
        findEnclosingCube();
        dimensions = region.corners[1] - region.corners[0];
    }

    if (dimensions.getX() <= M3D_OCTREE_NODE_MIN_SIZE && dimensions.getY() <= M3D_OCTREE_NODE_MIN_SIZE && dimensions.getZ() <= M3D_OCTREE_NODE_MIN_SIZE)
    {
        if (!parent)
        {
            region.corners[0] = Vector3(0.0f, 0.0f, 0.0f);
            region.corners[1] = Vector3(0.0f, 0.0f, 0.0f);
        }
        else
        {
            treeBuilt = true;
            treeReady = true;
        }

        return;
    }

    Box octant[8];
    createNodesDivision(octant, region);

    std::list<Object*> octList[8];
    std::list<Object*> delist;

    std::list<Object*>::const_iterator it_o = objects.begin();
    while (it_o != objects.end())
    {
        Object* obj = *it_o++;
        Box b = obj->getBoundingBoxAlignedAxis();

        for (int a = 0; a < 8; a++)
        {
            if (octant[a].contains(b))
            {
                octList[a].push_back(obj);
                delist.push_back(obj);
                break;
            }
        }
    }

    it_o = delist.begin();
    while (it_o != delist.end())
    {
        Object* obj = *it_o++;
        remove(obj);
    }

    for (int a = 0; a < 8; a++)
    {
        if (!octList[a].empty())
        {
            if (!childs[a])
            {
                childs[a] = createNode(octant[a], &octList[a]);
                activeNodes |= (byte)(1 << a);
                childs[a]->buildTree();
            }
            else
            {
                childs[a]->add(&octList[a]);
            }
        }

        if (childs[a] && childs[a]->region.getMaxSize() != octant[a].getMaxSize())
        {
            for (int o = 0; o < 8; o++)
            {
                if (octant[o].contains(childs[a]->region))
                {
                    childs[a]->region = octant[o];
                    childs[a]->buildTree();
                    break;
                }
            }
        }
    }

    treeBuilt = true;
    treeReady = true;
}

Magic3D::Octree* Magic3D::Octree::createNode(const Box& region, std::list<Object*>* objList)
{
    Octree* result = NULL;
    if (!objList->empty())
    {
        result = new Octree(region, objList);
        result->parent = this;
    }
    return result;
}

Magic3D::Octree* Magic3D::Octree::createNode(const Box& region, Object* obj)
{
    std::list<Object*> objList;
    objList.push_back(obj);
    Octree* result = new Octree(region, &objList);
    result->parent = this;
    return result;
}

void Magic3D::Octree::updateTree()
{
    if (!treeBuilt)
    {
        while (!pendingInsertion.empty())
        {
            addToList(pendingInsertion.back());
            pendingInsertion.pop_back();
        }

        buildTree();
    }
    else
    {
        while (!pendingInsertion.empty())
        {
            insert(pendingInsertion.back());
            pendingInsertion.pop_back();
        }
    }

    treeReady = true;
}

void Magic3D::Octree::findEnclosingBox()
{
    Vector3 global_min = region.corners[0];
    Vector3 global_max = region.corners[1];

    std::list<Object*>::const_iterator it_o = objects.begin();
    while (it_o != objects.end())
    {
        Object* obj = *it_o++;
        Box b = obj->getBoundingBoxAlignedAxis();
        Vector3 local_min = b.corners[0];
        Vector3 local_max = b.corners[1];

        if (local_min.getX() < global_min.getX()) global_min.setX(local_min.getX());
        if (local_min.getY() < global_min.getY()) global_min.setY(local_min.getY());
        if (local_min.getZ() < global_min.getZ()) global_min.setZ(local_min.getZ());

        if (local_max.getX() > global_max.getX()) global_max.setX(local_max.getX());
        if (local_max.getY() > global_max.getY()) global_max.setY(local_max.getY());
        if (local_max.getZ() > global_max.getZ()) global_max.setZ(local_max.getZ());
    }

    region.corners[0] = global_min;
    region.corners[1] = global_max;
}

void Magic3D::Octree::findEnclosingCube()
{
    findEnclosingBox();

    float max = Math::max(Math::max(fabsf(region.corners[0].getX()), fabsf(region.corners[0].getY())), fabsf(region.corners[0].getZ()));
    float min = Math::max(Math::max(fabsf(region.corners[1].getX()), fabsf(region.corners[1].getY())), fabsf(region.corners[1].getZ()));
    int x = (int)(Math::max(max, min) * 2.0f + 0.5f);

    if (x >= 0)
    {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x = x+1;
    }

    //int x = (int)powf(2, ceilf(log2f(region.getMaxSize())));

    x /= 2;

    region.corners[0] = Vector3(-x, -x, -x);
    region.corners[1] = Vector3(x, x, x);
}

bool Magic3D::Octree::isRoot()
{
    return parent == NULL;
}

bool Magic3D::Octree::hasChildren()
{
    return activeNodes != 0;
}

bool Magic3D::Octree::isEmpty()
{
    bool result = true;
    if (objects.empty() && pendingInsertion.empty())
    {
        for (int a = 0; a < 8; a++)
        {
            if (childs[a] != NULL && !childs[a]->isEmpty())
            {
                result = false;
                break;
            }
        }
    }
    else
    {
        result = false;
    }

    return result;
}

int Magic3D::Octree::getNodesCount()
{
    return nodesCount;
}

int Magic3D::Octree::getRenderedNodes()
{
    return nodesRenderCount;
}

void Magic3D::Octree::setInFrustum(bool frustum)
{
    if (inFrustum != frustum)
    {
        if (frustum)
        {
            nodesRenderCount++;
        }
        else
        {
            nodesRenderCount--;
        }
    }

    inFrustum = frustum;
}

bool Magic3D::Octree::isInFrustum()
{
    return inFrustum;
}

void Magic3D::Octree::resetChildFrustum()
{
    setInFrustum(false);
    for (int flags = activeNodes, index = 0; flags > 0; flags >>= 1, index++)
    {
        if ((flags & 1) == 1)
        {
            childs[index]->resetChildFrustum();
        }
    }
}

const Magic3D::Box& Magic3D::Octree::getBoundingBox()
{
    return region;
}

const std::list<Magic3D::Object*>* Magic3D::Octree::getObjects()
{
    return &objects;
}

Magic3D::Octree* Magic3D::Octree::getChild(int index)
{
    return childs[index];
}

byte Magic3D::Octree::getActiveNodes()
{
    return activeNodes;
}

void Magic3D::Octree::createNodesDivision(Box* nodes, const Box& box)
{
    Vector3 dimensions = box.corners[1] - box.corners[0];
    Vector3 half = dimensions / 2.0f;
    Vector3 center = box.corners[0] + half;

    nodes[0] = Box(box.corners[0], center);
    nodes[1] = Box(Vector3(center.getX(), box.corners[0].getY(), box.corners[0].getZ()), Vector3(box.corners[1].getX(), center.getY(), center.getZ()));
    nodes[2] = Box(Vector3(center.getX(), box.corners[0].getY(), center.getZ()), Vector3(box.corners[1].getX(), center.getY(), box.corners[1].getZ()));
    nodes[3] = Box(Vector3(box.corners[0].getX(), box.corners[0].getY(), center.getZ()), Vector3(center.getX(), center.getY(), box.corners[1].getZ()));
    nodes[4] = Box(Vector3(box.corners[0].getX(), center.getY(), box.corners[0].getZ()), Vector3(center.getX(), box.corners[1].getY(), center.getZ()));
    nodes[5] = Box(Vector3(center.getX(), center.getY(), box.corners[0].getZ()), Vector3(box.corners[1].getX(), box.corners[1].getY(), center.getZ()));
    nodes[6] = Box(center, box.corners[1]);
    nodes[7] = Box(Vector3(box.corners[0].getX(), center.getY(), center.getZ()), Vector3(center.getX(), box.corners[1].getY(), box.corners[1].getZ()));
}
