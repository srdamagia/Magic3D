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

#ifndef MAGIC3D_CAMERA_H
#define MAGIC3D_CAMERA_H

#include <magic3d/object.h>
#include <magic3d/renderer/window.h>

namespace Magic3D
{

#define M3D_CAMERA_XML                "Camera"
#define M3D_CAMERA_XML_VIEW           "view"
#define M3D_CAMERA_XML_FOVY           "fovy"
#define M3D_CAMERA_XML_NEAR           "near"
#define M3D_CAMERA_XML_FAR            "far"
#define M3D_CAMERA_XML_PROJECTION     "projection"

#define M3D_CAMERA_XML_INTERAXIAL     "interaxial"
#define M3D_CAMERA_XML_INTERPUPILLARY "interpupillary"
#define M3D_CAMERA_XML_NECKDISTANCE   "neckdistance"

#define M3D_CAMERA_XML_DISTORTION     "distortion"
#define M3D_CAMERA_XML_DISTORTIONCUBE "distortion_cube"
#define M3D_CAMERA_XML_DISTORTIONX    "distortion_x"
#define M3D_CAMERA_XML_DISTORTIONY    "distortion_y"

#define M3D_CAMERA_DEFAULT_FOVY  45.0f
#define M3D_CAMERA_DEFAULT_NEAR 0.1f
#define M3D_CAMERA_DEFAULT_FAR  10000.0f

struct ViewPort;

enum STEREOSCOPY
{
    eSTEREOSCOPY_LEFT,
    eSTEREOSCOPY_RIGHT,
    eSTEREOSCOPY_CENTER
};

enum FRUSTUM
{
    eFRUSTUM_RIGHT,
    eFRUSTUM_LEFT,
    eFRUSTUM_BOTTOM,
    eFRUSTUM_TOP,
    eFRUSTUM_FAR,
    eFRUSTUM_NEAR
};

enum PROJECTION
{
    ePROJECTION_ORTHOGRAPHIC,
    ePROJECTION_PERSPECTIVE
};

class Camera : public Object
{
private:
    Matrix4 view;
    Matrix4 projection;

    Matrix4 leftView;
    Matrix4 leftProjection;
    Matrix4 rightView;
    Matrix4 rightProjection;

    float zNear;
    float zFar;
    float fovy;
    float frustum[6][4];    

    float viewLeft;
    float viewRight;
    float viewBottom;
    float viewTop;
    float viewAspect;

    int interaxial;
    int interpupillary;
    float neckDistance;

    float distortion;
    float distortionCube;
    float distortionX;
    float distortionY;

    PROJECTION projectionType;

    bool needUpdateView;

private:
    void init();

    Matrix4 getEstereoscopyProjection(bool isLeftEye);
    Matrix4 perspectiveOffCenter(float left, float right, float bottom, float top, float zNear, float zFar);

protected:
    virtual void updateMatrix();

    Camera(const Camera& camera, std::string name);
public:
    Camera(OBJECT type, std::string name);
    Camera(std::string name);
    virtual ~Camera();
    virtual Camera* spawn(std::string name) const;

    void setView(float fovy, float zNear, float zFar);
    void setFovy(float fovy);
    float getFovy();
    void setNear(float zNear);
    float getNear();
    void setFar(float zFar);
    float getFar();

    void setInteraxial(int interaxial);
    int getInteraxial();
    void setInterpupillary(int interpupillary);
    int getInterpupillary();
    void setNeckDistance(float distance);
    float getNeckDistance();

    void setDistortion(float distortion);
    float getDistortion();
    void setDistortionCube(float distortion);
    float getDistortionCube();
    void setDistortionX(float distortion);
    float getDistortionX();
    void setDistortionY(float distortion);
    float getDistortionY();

    void setProjectionType(PROJECTION projection);
    PROJECTION getProjectionType();

    virtual void updateView();
    virtual const Matrix4& getView(STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);
    virtual const Matrix4& getProjection(STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);
    virtual void reflection(Vector4 clipPlane, STEREOSCOPY stereoscopy);

    virtual const Matrix4& view2D(float left, float right, float bottom, float top);
    virtual const Matrix4& view3D(float aspect);
    virtual void updateFrustum(const Matrix4& matrix, Matrix4& view);

    bool frustumVertex(Vector3 pos, bool testLR, bool testTB, bool testNF);
    bool frustumBox(Vector3 pos, float width, float height, float depth, bool ignoreNear = false);
    bool frustumSphere(Vector3 pos, float radius, bool ignoreNear);

    static Vector3 project(Vector3 pos, Matrix4 modelview, Matrix4 projection, ViewPort* viewport);
    static Vector3 unproject(Vector3 pos, Matrix4 modelview, Matrix4 projection, ViewPort* viewport);

    Vector3 getPosition2D(Vector3 pos, ViewPort* viewport, STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);
    Vector3 getPosition3D(float x, float y, float depth, ViewPort* viewport, STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);
    Vector3 getPosition3DOnPlane(float x, float y, Vector3 planeNormal, Vector3 planePosition, ViewPort* viewport, STEREOSCOPY stereoscopy = eSTEREOSCOPY_CENTER);
    Object* pick(float x, float y, int viewport, bool all);
    Vector3 getOrientedPosition(Vector3 pos);

    float getBoxMinZ(const Matrix4& model, const Box& box);

    void render();

    XMLElement* save(XMLElement* root);
    XMLElement* load(XMLElement* root);
};
}

#endif // MAGIC3D_CAMERA_H
