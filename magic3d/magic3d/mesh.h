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

#ifndef MAGIC3D_MESH_H
#define MAGIC3D_MESH_H

#include <magic3d/xml.h>
#include <magic3d/material.h>
#include <magic3d/math.h>

namespace Magic3D
{

#define M3D_MESH_XML                 "Mesh"
#define M3D_MESH_XML_NAME            "name"
#define M3D_MESH_XML_TYPE            "type"
#define M3D_MESH_XML_MATERIAL        "material"
#define M3D_MESH_XML_VISIBLE         "visible"
#define M3D_MESH_XML_DOUBLE_SIDE     "double_side"
#define M3D_MESH_XML_ILLUMINATED     "illuminated"
#define M3D_MESH_XML_FOG             "fog"
#define M3D_MESH_XML_REFLECTIVE      "reflective"
#define M3D_MESH_XML_GLOW            "glow"
#define M3D_MESH_XML_CAST_SHADOWS    "cast_shadows"
#define M3D_MESH_XML_RECEIVE_SHADOWS "receive_shadows"

#define M3D_MESH_DATA                "mesh_data"

enum MESH
{
    eMESH_TRIANGLES,
    eMESH_TRIANGLES_STRIP
};

enum AXIS
{
    eAXIS_X,
    eAXIS_Y,
    eAXIS_Z
};

struct RENDER_ID
{
    uint id;
    uint data;
    int dataSize;
    uint index;
    int indexSize;
};

class Object;
class MeshData : public Xml
{
private:
    std::vector<Vertex3D> vertices;
    std::vector<vindex> indexes;
    std::vector<LineIndexes> lines;
    std::vector<vindex> points;

    std::string name;

    btCollisionShape* shape;
    btTriangleMesh* triangleMesh;    
    bool computedBox;

    RENDER_ID renderID;

    bool skinned;

protected:
    MESH type;
    Box box;

    MeshData(const MeshData& meshData);
public:
    MeshData(MESH type, std::string name);
    virtual ~MeshData();
    virtual void* spawn() const;

    virtual bool update(Object* object);

    void setType(MESH type);
    MESH getType();
    const std::string& getName();

    void setShape(btCollisionShape* shape);
    btCollisionShape* getShape();

    void setTriangleMesh(btTriangleMesh* triangleMesh);
    btTriangleMesh* getTriangleMesh();

    std::vector<Vertex3D>* getVertices();

    std::vector<vindex>* getIndexes();
    std::vector<LineIndexes>* getLines();
    std::vector<vindex>* getPoints();

    int getVerticesCount();
    int getIndexesCount();
    int getTrianglesCount();
    int getLinesCount();
    int getPointsCount();

    void addVertex(Vertex3D vertex);
    void addIndex(vindex index);
    void addTriangle(TriangleIndexes triangle);
    void updateTangent(vindex index0, vindex index1, vindex index2);
    int addQuad(float x, float y, float width, float height, AXIS axis, bool inverse);
    int addQuad(float x, float y, float width, float height, AXIS axis, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), bool inverse = false);
    int addQuad(float x, float y, float z, float width, float height, AXIS axis, bool inverse);
    int addQuad(float x, float y, float z, float width, float height, AXIS axis, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), bool inverse = false);
    void setQuad(float* buffer, int index, float x, float y, float width, float height, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    void setQuad(float* buffer, int index, float x, float y, float z, float width, float height, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    void setQuad(float* buffer, int index, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, const Vector3& vertex4, ColorRGBA& color);
    void setQuad(float* buffer, int index, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, const Vector3& vertex4, ColorRGBA& color1, ColorRGBA& color2, ColorRGBA& color3, ColorRGBA& color4);
    void setQuadTexture(float* buffer, int index, float x, float y, float width, float height);
    void setQuadTexture(float* buffer, int index, int uvIndex, float x, float y, float width, float height);
    void setQuadTexture(float* buffer, int index, const Texture2D& uv1, const Texture2D& uv2, const Texture2D& uv3, const Texture2D& uv4);
    void setQuadTexture(float* buffer, int index, int uvIndex, const Texture2D& uv1, const Texture2D& uv2, const Texture2D& uv3, const Texture2D& uv4);
    void moveQuad(float* buffer, int index, float x, float y, float z);
    void removeQuad(int index);
    void addLine(Vector3 start, Vector3 finish, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    void addPoint(Vector3 position, float size, ColorRGBA color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    void setSkinned(bool skinned);
    bool isSkinned();

    void setRenderID(RENDER_ID id);
    const RENDER_ID& getRenderID();
    bool isVBO();
    void createVbo();
    void deleteVbo();

    float* mapBuffer();
    void unmapBuffer();

    vindex* mapIndexes();
    void unmapIndexes();

    virtual const Box& getBoundingBox();

    void clear();

    static void createPlaneStrip(std::vector<vindex>* indexes, std::vector<LineIndexes>* lines, int length, int width);
    static void updatePlaneStripNormals(float* vertices, int vcount, vindex* indexes, int icount);
    static void updateTangent(float* vertices, int vcount, vindex index0, vindex index1, vindex index2);

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

class Mesh : public Xml
{
private:
    std::vector<Material*> materials;

    MeshData* data;

    bool owner;
    bool doubleSide;
    bool visible;
    bool illuminated;
    bool fog;
    bool castShadows;
    bool receiveShadows;
    bool reflective;
    bool glow;

    void init(MeshData* data, bool owner = false, bool doubleSide = false, MESH type = eMESH_TRIANGLES, std::string name = M3D_MESH_DATA);

protected:
    Mesh(const Mesh& mesh);

public:
    Mesh(MESH type, std::string name, bool doubleSide = false);
    Mesh(MeshData* data, bool doubleSide = false, bool owner = false);
    virtual ~Mesh();
    virtual Mesh* spawn() const;

    MeshData* getData();

    virtual bool update(Object* object);

    std::vector<Material*>* getMaterials();

    void addMaterial(Material* material);

    void setDoubleSide(bool doubleSide);
    bool isDoubleSide();

    void setIlluminated(bool illuminated);
    bool isIlluminated();

    void setFogged(bool fog);
    bool isFogged();

    void setReflective(bool Reflective);
    bool isReflective();

    void setGlow(bool glow);
    bool isGlow();

    void setVisible(bool visible);
    bool isVisible();

    void setCastShadows(bool cast);
    bool isCastingShadows();

    void setReceiveShadows(bool receive);
    bool isReceivingShadows();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // MAGIC3D_MESH_H
