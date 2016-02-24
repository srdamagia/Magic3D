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

Magic3D::Camera::Camera(const Camera& camera, std::string name) : Object(camera, name)
{
    this->view = camera.view;
    this->projection = camera.projection;

    this->leftView = camera.leftView;
    this->leftProjection = camera.leftProjection;

    this->rightView = camera.rightView;
    this->rightProjection = camera.rightProjection;

    this->zNear = camera.zNear;
    this->zFar = camera.zFar;
    this->fovy = camera.fovy;

    this->viewLeft = camera.viewLeft;
    this->viewRight = camera.viewRight;
    this->viewBottom = camera.viewBottom;
    this->viewTop = camera.viewTop;
    this->viewAspect = camera.viewAspect;

    this->interaxial = camera.interaxial;
    this->interpupillary = camera.interpupillary;
    this->neckDistance = camera.neckDistance;

    this->distortion = camera.distortion;
    this->distortionCube = camera.distortionCube;
    this->distortionX = camera.distortionX;
    this->distortionY = camera.distortionY;

    this->projectionType = camera.projectionType;

    this->needUpdateView = camera.needUpdateView;

    memcpy(this->frustum, camera.frustum, sizeof(this->frustum));
}

Magic3D::Camera::Camera(OBJECT type, std::string name) : Object(type, eRENDER_3D, name)
{
    init();
}

Magic3D::Camera::Camera(std::string name) : Object(eOBJECT_CAMERA, eRENDER_3D, name)
{
    init();
}

void Magic3D::Camera::init()
{
    fovy  = M3D_CAMERA_DEFAULT_FOVY;
    zNear = M3D_CAMERA_DEFAULT_NEAR;
    zFar  = M3D_CAMERA_DEFAULT_FAR;

    viewLeft   = 0.0f;
    viewRight  = 0.0f;
    viewBottom = 0.0f;
    viewTop    = 0.0f;
    viewAspect = 0.0f;

    interaxial     = 0;
    interpupillary = 65;
    neckDistance   = 0.1f;

    distortion     = 0.68f;
    distortionCube = 0.82f;
    distortionX    = 0.75f;
    distortionY    = 0.75f;

    needUpdateView = true;

    projectionType = ePROJECTION_PERSPECTIVE;

    memset(frustum, 0.0f, sizeof(frustum));
}

Magic3D::Camera::~Camera()
{

}

void* Magic3D::Camera::spawn(std::string name) const
{
    return (new Camera(*this, name));
}

void Magic3D::Camera::setView(float fovy, float zNear, float zFar)
{
    this->fovy  = fovy;
    this->zNear = zNear;
    this->zFar  = zFar;

    needUpdateView = true;
}

void Magic3D::Camera::setFovy(float fovy)
{
    this->fovy = fovy;

    needUpdateView = true;
}

float Magic3D::Camera::getFovy()
{
    return fovy;
}

void Magic3D::Camera::setNear(float zNear)
{
    this->zNear = zNear;

    needUpdateView = true;
}

float Magic3D::Camera::getNear()
{
    return zNear;
}

void Magic3D::Camera::setFar(float zFar)
{
    this->zFar = zFar;

    needUpdateView = true;
}

float Magic3D::Camera::getFar()
{
    return zFar;
}

void Magic3D::Camera::setInteraxial(int interaxial)
{
    this->interaxial = interaxial;

    needUpdateView = true;
}

int Magic3D::Camera::getInteraxial()
{
    return interaxial;
}

void Magic3D::Camera::setInterpupillary(int interpupillary)
{
    this->interpupillary = interpupillary;

    needTransform = true;
}

int Magic3D::Camera::getInterpupillary()
{
    return interpupillary;
}

void Magic3D::Camera::setNeckDistance(float distance)
{
    this->neckDistance = distance;

    needTransform = true;
}

float Magic3D::Camera::getNeckDistance()
{
    return neckDistance;
}

void Magic3D::Camera::setDistortion(float distortion)
{
    this->distortion = distortion;

    needUpdateView = true;
}

float Magic3D::Camera::getDistortion()
{
    return distortion;
}

void Magic3D::Camera::setDistortionCube(float distortion)
{
    this->distortionCube = distortion;

    needUpdateView = true;
}

float Magic3D::Camera::getDistortionCube()
{
    return distortionCube;
}

void Magic3D::Camera::setDistortionX(float distortion)
{
    this->distortionX = distortion;

    needUpdateView = true;
}

float Magic3D::Camera::getDistortionX()
{
    return distortionX;
}

void Magic3D::Camera::setDistortionY(float distortion)
{
    this->distortionY = distortion;

    needUpdateView = true;
}

float Magic3D::Camera::getDistortionY()
{
    return distortionY;
}

void Magic3D::Camera::setProjectionType(PROJECTION projection)
{
    projectionType = projection;
}

Magic3D::PROJECTION Magic3D::Camera::getProjectionType()
{
    return projectionType;
}

void Magic3D::Camera::updateMatrix()
{
    Object::updateMatrix();

    Matrix4 m;

    Vector3 p = getPosition();
    Vector3 f = getDirectionFront();
    Vector3 u = getDirectionUp();

    if (getParent())
    {
        m = getMatrixFromParent();
        p = m.getTranslation();
        f = m.getCol2().getXYZ();
        u = m.getCol1().getXYZ();
    }

    Point3 pos = Point3(p);

    view = Matrix4::lookAt(pos, Point3(pos + f), u);

    view = appendScale(view, getScale());

    if (Magic3D::getInstance()->getConfiguration().STEREOSCOPY)
    {
        leftView  = view * Matrix4(Quaternion::identity(), Vector3(-((float)interpupillary / 1000.0f) / 2, neckDistance, 0));
        rightView = view * Matrix4(Quaternion::identity(), Vector3(((float)interpupillary / 1000.0f) / 2, neckDistance, 0));
    }
}

void Magic3D::Camera::updateView()
{
    updateMatrix();
    needUpdateView = true;
}

const Magic3D::Matrix4& Magic3D::Camera::getView(STEREOSCOPY stereoscopy)
{
    switch (stereoscopy)
    {
        case eSTEREOSCOPY_LEFT: return leftView; break;
        case eSTEREOSCOPY_RIGHT: return rightView; break;
        default: return view;
    }
}

const Magic3D::Matrix4& Magic3D::Camera::getProjection(STEREOSCOPY stereoscopy)
{
    switch (stereoscopy)
    {
        case eSTEREOSCOPY_LEFT: return leftProjection; break;
        case eSTEREOSCOPY_RIGHT: return rightProjection; break;
        default: return projection;
    }
}

void Magic3D::Camera::reflection(Vector4 clipPlane, STEREOSCOPY stereoscopy)
{
    Matrix4* mp;
    Matrix4* mv;

    switch (stereoscopy)
    {
        case eSTEREOSCOPY_LEFT:
        {
            mp = &leftProjection;
            mv = &leftView;
            break;
        }
        case eSTEREOSCOPY_RIGHT:
        {
            mp = &rightProjection;
            mv = &rightView;
            break;
        }
        default:
        {
            mp = &projection;
            mv = &view;
            break;
        }
    }

    Matrix4 mirror = Matrix4::identity();
    mirror.setElem(0, 1, -clipPlane.getX());
    mirror.setElem(1, 1, -clipPlane.getY());
    mirror.setElem(2, 1, -clipPlane.getZ());
    mirror.setElem(3, 1, (2.0f * clipPlane.getW()));

    *mv = *mv * mirror;

    Vector4 vClipPlaneView = transpose(inverse(*mv)) * (clipPlane + Vector4(0.0f, 0.0f, 0.0f, -2.0f * clipPlane.getW()));	// put clip plane into view coords
    /*
        Calculate the clip-space corner point opposite the clipping plane
        and transform it into camera space by multiplying it by the inverse
        projection matrix.
    */
    Vector4 vClipSpaceCorner = Vector4(Math::sgn(vClipPlaneView.getX()), Math::sgn(vClipPlaneView.getY()), 1.0f, 1.0f);
    vClipSpaceCorner = transpose(inverse(*mp)) * vClipSpaceCorner;

    // Calculate the scaled plane vector
    Vector4 vScaledPlane = -vClipPlaneView * (2.0f / dot(vClipSpaceCorner, vClipPlaneView));

    // Replace the third row of the matrix
    mp->setElem(0, 2, vScaledPlane.getX());
    mp->setElem(1, 2, vScaledPlane.getY());
    mp->setElem(2, 2, vScaledPlane.getZ() + 1.0f);
    mp->setElem(3, 2, vScaledPlane.getW());
}

const Magic3D::Matrix4& Magic3D::Camera::view2D(float left, float right, float bottom, float top)
{
    Window* window = Renderer::getInstance()->getWindow();
    if (window && window->isNeedingUpdateView())
    {
        needUpdateView = true;
        window->setNeedingUpdateView(false);
    }

    if (getType() == eOBJECT_LIGHT || needUpdateView || viewLeft != left || viewRight != right || viewBottom != bottom || viewTop != top)
    {
        viewLeft   = left;
        viewRight  = right;
        viewBottom = bottom;
        viewTop    = top;

        float vr = right;
        float vb = bottom;
        viewAspect = 0.0f;

        if (window)
        {
            switch (window->getOrientation())
            {
                case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
                {
                    projection = Matrix4::rotationZ(Math::radians(180.0f));
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
                {
                    vr = bottom;
                    vb = right;
                    projection = Matrix4::rotationZ(Math::radians(-90.0f));
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
                {
                    vr = bottom;
                    vb = right;
                    projection = Matrix4::rotationZ(Math::radians(90.0f));
                    break;
                }
                default: projection = Matrix4::identity();
            }
        }

        if (getType() == eOBJECT_LIGHT)
        {
            //projection *= Matrix4::orthographic(viewLeft, vr, vb, viewTop, getNear(), getFar());
            projection *= static_cast<Light*>(this)->computeShadowProjection();
        }
        else
        {
            projection *= Matrix4::orthographic(viewLeft, vr, vb, viewTop, -2.0f, 2.0f);
        }

        needUpdateView = false;
    }
    return projection;
}

const Magic3D::Matrix4& Magic3D::Camera::view3D(float aspect)
{
    Window* window = Renderer::getInstance()->getWindow();
    if (window && window->isNeedingUpdateView())
    {
        needUpdateView = true;
        window->setNeedingUpdateView(false);
    }

    if (needUpdateView || viewAspect != aspect)
    {
        viewAspect = aspect;

        viewLeft   = 0.0f;
        viewRight  = 0.0f;
        viewBottom = 0.0f;
        viewTop    = 0.0f;

        if (window)
        {
            switch (window->getOrientation())
            {
                case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
                {
                    projection = Matrix4::rotationZ(Math::radians(180.0f));
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
                {
                    projection = Matrix4::rotationZ(Math::radians(-90.0f));
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
                {
                    projection = Matrix4::rotationZ(Math::radians(90.0f));
                    break;
                }
                default: projection = Matrix4::identity();
            }
        }

        if (Magic3D::getInstance()->getConfiguration().STEREOSCOPY)
        {
            leftProjection = projection * getEstereoscopyProjection(true);
            rightProjection = projection * getEstereoscopyProjection(false);
        }

        projection *= Matrix4::perspective(Math::radians(getFovy()), viewAspect, getNear(), getFar());

        needUpdateView = false;
    }
    return projection;
}

Magic3D::Matrix4 Magic3D::Camera::getEstereoscopyProjection(bool isLeftEye)
{
    float left = 0.0f;
    float right = 0.0f;    
    float a = getNear() * tan(Math::radians(getFovy()) * 0.5f);
    float b = getNear();
    if (isLeftEye)
    {
        left = (-viewAspect * a) + ((float)interaxial / 2000.0f * b);
        right = (viewAspect * a) + ((float)interaxial / 2000.0f * b);
    }
    else
    {
        left = (-viewAspect * a) - ((float)interaxial / 2000.0f * b);
        right = (viewAspect * a) - ((float)interaxial / 2000.0f * b);
    }
    return perspectiveOffCenter(left, right, -a, a, getNear(), getFar());
}

Magic3D::Matrix4 Magic3D::Camera::perspectiveOffCenter(float left, float right, float bottom, float top, float zNear, float zFar)
{
    float x = (2.0f * zNear) / (right - left);
    float y = (2.0f * zNear) / (top - bottom);
    float a = (right + left) / (right - left);
    float b = (top + bottom) / (top - bottom);
    float c = -(zFar + zNear) / (zFar - zNear);
    float d = -(2.0f * zFar * zNear) / (zFar - zNear);
    float e = -1.0f;

    Matrix4 m;
    m.setElem(0, 0, x);
    m.setElem(0, 1, 0.0f);
    m.setElem(0, 2, a);
    m.setElem(0, 3, 0.0f);
    m.setElem(1, 0, 0.0f);
    m.setElem(1, 1, y);
    m.setElem(1, 2, b);
    m.setElem(1, 3, 0.0f);
    m.setElem(2, 0, 0.0f);
    m.setElem(2, 1, 0.0f);
    m.setElem(2, 2, c);
    m.setElem(2, 3, d);
    m.setElem(3, 0, 0.0f);
    m.setElem(3, 1, 0.0f);
    m.setElem(3, 2, e);
    m.setElem(3, 3, 0.0f);
    return transpose(m);
}

void Magic3D::Camera::updateFrustum(const Matrix4& matrix, Matrix4& view)
{
    float* clip;
    float norm;

    view = getView() * matrix;
    Matrix4 objMatrix = projection * view;

    clip = (float*)&objMatrix;

    /* Extract the numbers for the RIGHT plane */
    frustum[eFRUSTUM_RIGHT][0] = clip[ 3] - clip[ 0];
    frustum[eFRUSTUM_RIGHT][1] = clip[ 7] - clip[ 4];
    frustum[eFRUSTUM_RIGHT][2] = clip[11] - clip[ 8];
    frustum[eFRUSTUM_RIGHT][3] = clip[15] - clip[12];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_RIGHT][0]) +
                 Math::sqr(frustum[eFRUSTUM_RIGHT][1]) +
                 Math::sqr(frustum[eFRUSTUM_RIGHT][2]));
    frustum[eFRUSTUM_RIGHT][0] /= norm;
    frustum[eFRUSTUM_RIGHT][1] /= norm;
    frustum[eFRUSTUM_RIGHT][2] /= norm;
    frustum[eFRUSTUM_RIGHT][3] /= norm;

    /* Extract the numbers for the LEFT plane */
    frustum[eFRUSTUM_LEFT][0] = clip[ 3] + clip[ 0];
    frustum[eFRUSTUM_LEFT][1] = clip[ 7] + clip[ 4];
    frustum[eFRUSTUM_LEFT][2] = clip[11] + clip[ 8];
    frustum[eFRUSTUM_LEFT][3] = clip[15] + clip[12];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_LEFT][0]) +
                 Math::sqr(frustum[eFRUSTUM_LEFT][1]) +
                 Math::sqr(frustum[eFRUSTUM_LEFT][2]));
    frustum[eFRUSTUM_LEFT][0] /= norm;
    frustum[eFRUSTUM_LEFT][1] /= norm;
    frustum[eFRUSTUM_LEFT][2] /= norm;
    frustum[eFRUSTUM_LEFT][3] /= norm;

    /* Extract the BOTTOM plane */
    frustum[eFRUSTUM_BOTTOM][0] = clip[ 3] + clip[ 1];
    frustum[eFRUSTUM_BOTTOM][1] = clip[ 7] + clip[ 5];
    frustum[eFRUSTUM_BOTTOM][2] = clip[11] + clip[ 9];
    frustum[eFRUSTUM_BOTTOM][3] = clip[15] + clip[13];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_BOTTOM][0]) +
                 Math::sqr(frustum[eFRUSTUM_BOTTOM][1]) +
                 Math::sqr(frustum[eFRUSTUM_BOTTOM][2]));
    frustum[eFRUSTUM_BOTTOM][0] /= norm;
    frustum[eFRUSTUM_BOTTOM][1] /= norm;
    frustum[eFRUSTUM_BOTTOM][2] /= norm;
    frustum[eFRUSTUM_BOTTOM][3] /= norm;

    /* Extract the TOP plane */
    frustum[eFRUSTUM_TOP][0] = clip[ 3] - clip[ 1];
    frustum[eFRUSTUM_TOP][1] = clip[ 7] - clip[ 5];
    frustum[eFRUSTUM_TOP][2] = clip[11] - clip[ 9];
    frustum[eFRUSTUM_TOP][3] = clip[15] - clip[13];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_TOP][0]) +
                 Math::sqr(frustum[eFRUSTUM_TOP][1]) +
                 Math::sqr(frustum[eFRUSTUM_TOP][2]));
    frustum[eFRUSTUM_TOP][0] /= norm;
    frustum[eFRUSTUM_TOP][1] /= norm;
    frustum[eFRUSTUM_TOP][2] /= norm;
    frustum[eFRUSTUM_TOP][3] /= norm;

    /* Extract the FAR plane */
    frustum[eFRUSTUM_FAR][0] = clip[ 3] - clip[ 2];
    frustum[eFRUSTUM_FAR][1] = clip[ 7] - clip[ 6];
    frustum[eFRUSTUM_FAR][2] = clip[11] - clip[10];
    frustum[eFRUSTUM_FAR][3] = clip[15] - clip[14];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_FAR][0]) +
                 Math::sqr(frustum[eFRUSTUM_FAR][1]) +
                 Math::sqr(frustum[eFRUSTUM_FAR][2]));
    frustum[eFRUSTUM_FAR][0] /= norm;
    frustum[eFRUSTUM_FAR][1] /= norm;
    frustum[eFRUSTUM_FAR][2] /= norm;
    frustum[eFRUSTUM_FAR][3] /= norm;

    /* Extract the NEAR plane */
    frustum[eFRUSTUM_NEAR][0] = clip[ 3] + clip[ 2];
    frustum[eFRUSTUM_NEAR][1] = clip[ 7] + clip[ 6];
    frustum[eFRUSTUM_NEAR][2] = clip[11] + clip[10];
    frustum[eFRUSTUM_NEAR][3] = clip[15] + clip[14];

    //normalize the plane
    norm = sqrtf(Math::sqr(frustum[eFRUSTUM_NEAR][0]) +
                 Math::sqr(frustum[eFRUSTUM_NEAR][1]) +
                 Math::sqr(frustum[eFRUSTUM_NEAR][2]));
    frustum[eFRUSTUM_NEAR][0] /= norm;
    frustum[eFRUSTUM_NEAR][1] /= norm;
    frustum[eFRUSTUM_NEAR][2] /= norm;
    frustum[eFRUSTUM_NEAR][3] /= norm;
}

bool Magic3D::Camera::frustumVertex(Vector3 pos, bool testLR, bool testTB, bool testNF)
{
    //loop through the frustum planes
    for (int i = 0; i < 6; i++)
    {
        if ((i == eFRUSTUM_RIGHT || i == eFRUSTUM_LEFT) && testLR)
        {
            if (frustum[i][0] * pos.getX() + frustum[i][1] * pos.getY() + frustum[i][2] * pos.getZ() + frustum[i][3] <= 0)
            {
                return false;
            }
        }

        if ((i == eFRUSTUM_TOP || i == eFRUSTUM_BOTTOM ) && testTB)
        {
            if (frustum[i][0] * pos.getX() + frustum[i][1] * pos.getY() + frustum[i][2] * pos.getZ() + frustum[i][3] <= 0)
            {
                return false;
            }
        }

        if ((i == eFRUSTUM_NEAR || i == eFRUSTUM_FAR) && testNF)
        {
            if (frustum[i][0] * pos.getX() + frustum[i][1] * pos.getY() + frustum[i][2] * pos.getZ() + frustum[i][3] <= 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Magic3D::Camera::frustumBox(Vector3 pos, float width, float height, float depth, bool ignoreNear)
{
    float halfWidth  = width  * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth  = depth  * 0.5f;

    //loop through and test the six vertices of the bounding box against the viewing frustum
    int faces = ignoreNear ? 5 : 6;
    for (int i = 0; i < faces; i++)
    {
        if (frustum[i][0] * (pos.getX() - halfWidth) + frustum[i][1] * (pos.getY() - halfHeight) + frustum[i][2] * (pos.getZ() - halfDepth) + frustum[i][3] > 0)
            continue;
        if (frustum[i][0] * (pos.getX() + halfWidth) + frustum[i][1] * (pos.getY() - halfHeight) + frustum[i][2] * (pos.getZ() - halfDepth) + frustum[i][3] > 0)
            continue;
        if (frustum[i][0] * (pos.getX() - halfWidth) + frustum[i][1] * (pos.getY() + halfHeight) + frustum[i][2] * (pos.getZ() - halfDepth) + frustum[i][3] > 0 )
            continue;
        if (frustum[i][0] * (pos.getX() + halfWidth) + frustum[i][1] * (pos.getY() + halfHeight) + frustum[i][2] * (pos.getZ() - halfDepth) + frustum[i][3] > 0 )
            continue;
        if (frustum[i][0] * (pos.getX() - halfWidth) + frustum[i][1] * (pos.getY() - halfHeight) + frustum[i][2] * (pos.getZ() + halfDepth) + frustum[i][3] > 0 )
            continue;
        if (frustum[i][0] * (pos.getX() + halfWidth) + frustum[i][1] * (pos.getY() - halfHeight) + frustum[i][2] * (pos.getZ() + halfDepth) + frustum[i][3] > 0 )
            continue;
        if (frustum[i][0] * (pos.getX() - halfWidth) + frustum[i][1] * (pos.getY() + halfHeight) + frustum[i][2] * (pos.getZ() + halfDepth) + frustum[i][3] > 0 )
            continue;
        if (frustum[i][0] * (pos.getX() + halfWidth) + frustum[i][1] * (pos.getY() + halfHeight) + frustum[i][2] * (pos.getZ() + halfDepth) + frustum[i][3] > 0 )
            continue;

        return false;
    }
    return true;
}

bool Magic3D::Camera::frustumSphere(Vector3 pos, float radius, bool ignoreNear)
{
    int c = 0;
    int faces = ignoreNear ? 5 : 6;
    for( int i = 0; i < faces; i++ )
    {
        float d = frustum[i][0] * pos.getX() + frustum[i][1] * pos.getY() + frustum[i][2] * pos.getZ() + frustum[i][3];
        if( d <= -radius )
            return false;
        if( d > radius )
            c++;
    }
    return (c == 6) ? 2 : 1;
}

Magic3D::Vector3 Magic3D::Camera::project(Vector3 pos, Matrix4 modelview, Matrix4 projection, ViewPort* viewport)
{
    //Transformation vectors
    float fTempo[8];

    float* v;
    float* p;

    v = (float*)&modelview;
    p = (float*)&projection;

    //Modelview transform
    fTempo[0] = v[0] * pos.getX() + v[4] * pos.getY() + v[8]  * pos.getZ() + v[12];  //w is always 1
    fTempo[1] = v[1] * pos.getX() + v[5] * pos.getY() + v[9]  * pos.getZ() + v[13];
    fTempo[2] = v[2] * pos.getX() + v[6] * pos.getY() + v[10] * pos.getZ() + v[14];
    fTempo[3] = v[3] * pos.getX() + v[7] * pos.getY() + v[11] * pos.getZ() + v[15];

    //Projection transform, the final row of projection matrix is always [0 0 -1 0]
    //so we optimize for that .
    fTempo[4] = p[0] * fTempo[0] + p[4] * fTempo[1] + p[8]  * fTempo[2] + p[12] * fTempo[3];
    fTempo[5] = p[1] * fTempo[0] + p[5] * fTempo[1] + p[9]  * fTempo[2] + p[13] * fTempo[3];
    fTempo[6] = p[2] * fTempo[0] + p[6] * fTempo[1] + p[10] * fTempo[2] + p[14] * fTempo[3];
    fTempo[7] = -fTempo[2];

    //The result normalizes between -1 and 1
    if(fTempo[7] == 0.0f)	//The w value
    {
        return Vector3(fTempo[0], fTempo[1], 0.0f);
    }
    fTempo[7] = 1.0f / fTempo[7];

    //Perspective division
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];

    //Window coordinates
    //Map x, y to range 0-1
    const Vector4& area = viewport->getArea();
    float sx = (fTempo[4] * 0.5f + 0.5f) * area.getZ() + area.getX();
    float sy = (fTempo[5] * 0.5f + 0.5f) * area.getW() + area.getY();
    //This is only correct when glDepthRange(0.0, 1.0)
    float sz = (1.0f + fTempo[6]) * 0.5f;	//Between 0 and 1

    return Vector3(sx, 1.0f - sy, sz);
}

Magic3D::Vector3 Magic3D::Camera::unproject(Vector3 pos, Matrix4 modelview, Matrix4 projection, ViewPort* viewport)
{
    //Calculation for inverting a matrix, compute projection x modelview
    Matrix4 matrix = projection * modelview;
    //Now compute the inverse of matrix
    matrix = inverse(matrix);

    //Transformation of normalized coordinates between -1 and 1
    const Vector4& area = viewport->getArea();
    float x = (pos.getX() - area.getX()) / area.getZ() * 2.0f - 1.0f;
    float y = (pos.getY() - area.getY()) / area.getW() * 2.0f - 1.0f;
    float z = 2.0f * pos.getZ() - 1.0f;
    float w = 1.0f;

    //Objects coordinates
    Vector4 result = matrix * Vector4(x, y, z, w);

    if(result.getW() == 0.0f)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    result.setW(1.0f / result.getW());

    x = result.getX() * result.getW();
    y = result.getY() * result.getW();
    z = result.getZ() * result.getW();

    return Vector3(x, y, z);
}

Magic3D::Vector3 Magic3D::Camera::getPosition2D(Vector3 pos, ViewPort* viewport, STEREOSCOPY stereoscopy)
{
    Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

    Window* window = Renderer::getInstance()->getWindow();

    if (viewport && window)
    {
        view3D((float)window->getWidth() / (float)window->getHeight());

        Matrix4 projection;

        switch (stereoscopy)
        {
            case eSTEREOSCOPY_LEFT: projection = leftProjection; break;
            case eSTEREOSCOPY_RIGHT: projection = rightProjection; break;
            default: projection = this->projection; break;
        }

        result = project(pos, getView(stereoscopy), projection, viewport);
    }

    return getOrientedPosition(result);
}

Magic3D::Vector3 Magic3D::Camera::getPosition3D(float x, float y, float depth, ViewPort* viewport, STEREOSCOPY stereoscopy)
{
    Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

    Window* window = Renderer::getInstance()->getWindow();

    if (viewport && window)
    {
        view3D((float)window->getWidth() / (float)window->getHeight());

        Matrix4 projection;

        switch (stereoscopy)
        {
            case eSTEREOSCOPY_LEFT: projection = leftProjection; break;
            case eSTEREOSCOPY_RIGHT: projection = rightProjection; break;
            default: projection = this->projection; break;
        }

        float wx = (float)x / (float)window->getWidth();
        float wy = 1.0f - (float)y / (float)window->getHeight();

        Matrix4 view = getView(stereoscopy);
        Vector3 n = unproject(Vector3(wx, wy, 0.0f), view, projection, viewport);
        Vector3 f = unproject(Vector3(wx, wy, 1.0f), view, projection, viewport);

        Vector3 dir = normalize(f - n);

        result = n + dir * depth;
    }

    return result;
}

Magic3D::Vector3 Magic3D::Camera::getPosition3DOnPlane(float x, float y, Vector3 planeNormal, Vector3 planePosition, ViewPort* viewport, STEREOSCOPY stereoscopy)
{
    Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

    Window* window = Renderer::getInstance()->getWindow();

    if (viewport && window)
    {
        view3D((float)window->getWidth() / (float)window->getHeight());

        Matrix4 projection;

        switch (stereoscopy)
        {
            case eSTEREOSCOPY_LEFT: projection = leftProjection; break;
            case eSTEREOSCOPY_RIGHT: projection = rightProjection; break;
            default: projection = this->projection; break;
        }

        float wx = (float)x / (float)window->getWidth();
        float wy = 1.0f - (float)y / (float)window->getHeight();

        Matrix4 view = getView(stereoscopy);
        Vector3 n = unproject(Vector3(wx, wy, 0.0f), view, projection, viewport);
        Vector3 f = unproject(Vector3(wx, wy, 1.0f), view, projection, viewport);

        Vector3 dir = normalize(f - n);

        result = Math::intersectLinePlane(n, dir, planeNormal, planePosition);
    }

    return result;
}

Magic3D::Object* Magic3D::Camera::pick(float x, float y, int viewport, bool all)
{
    Object* result = NULL;

    ViewPort* view = Renderer::getInstance()->getViewPort(viewport);
    Window* window = Renderer::getInstance()->getWindow();

    if (window)
    {
        int w = window->getWidth();
        int h = window->getHeight();

        float fx = x;
        float fy = y;

        float mx = fx;
        float my = -fy;

        std::list<RenderObject>* objs;
        Matrix4 projection;

        if (getProjectionType() == ePROJECTION_PERSPECTIVE)
        {
            switch (window->getOrientation())
            {
                case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
                {
                    mx = 1.0f - fx / w;
                    my = fy / h;
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
                {
                    mx = 1.0f - fy / h;
                    my = 1.0f - fx / w;
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
                {
                    mx = fy / h;
                    my = fx / w;
                    break;
                }
                default:
                {
                    mx = fx / w;
                    my = 1.0f - fy / h;
                }
            }

            projection = view3D((float)w / (float)h);
            Scene::getInstance()->updateVisibleObjectsOctree(this, true, false, true);
            objs = Scene::getInstance()->getVisibleObjects3D();
        }
        else
        {            
            Vector3 aspect = Renderer::getInstance()->getWindow()->getWindowScreenAspect();
            fx = (x / float(w)) * aspect.getX();
            fy = (y / float(h)) * aspect.getY();
            mx = fx;
            my = -fy;

            projection = view2D(0.0f, aspect.getX(), aspect.getY(), 0.0f);
            Scene::getInstance()->updateVisibleObjects2D(this, false);
            objs = Scene::getInstance()->getVisibleObjects2D();
        }

        std::list<RenderObject>::const_iterator it_o = objs->end();

        while (it_o != objs->begin() && !result)
        {
            const RenderObject* object = &(*--it_o);

            if (object->object->isPickable() || all)
            {
                Matrix4 matrix = getView() * object->object->getMatrixFromParent();

                Vector3 n = unproject(Vector3(mx, my, 0.0f), matrix, projection, view);
                Vector3 f = unproject(Vector3(mx, my, 1.0f), matrix, projection, view);

                Vector3 dir = f - n;
                dir = normalize(dir);

                Vector4 mouse = inverse(object->modelView) * Vector4(mx, -my, object->object->getMaxSizeFromParent(), 1.0f);
                Vector3 pos = Vector3(mouse.getX(), mouse.getY(), mouse.getZ());
                float cNear = getNear();
                float cFar = getFar();

                if (getProjectionType() == ePROJECTION_PERSPECTIVE)
                {
                    pos = n;
                }
                else
                {
                    cNear = -2.0f;
                    cFar = 2.0f;
                }

                Ray ray = Ray(pos, dir);

                if (Math::collide(ray, cNear, cFar, object->object->getBoundingBox()))
                {
                    result = object->object;
                }
            }
        }
    }

    return result;
}

Magic3D::Vector3 Magic3D::Camera::getOrientedPosition(Vector3 pos)
{
    Window* window = Renderer::getInstance()->getWindow();

    float fx = pos.getX();
    float fy = pos.getY();

    float mx = fx;
    float my = fy;

    if (window)
    {
        //int w = window->getWidth();
        //int h = window->getHeight();


        if (getProjectionType() == ePROJECTION_PERSPECTIVE)
        {
            switch (window->getOrientation())
            {
                case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
                {
                    mx = 1.0f - mx;
                    my = 1.0f - my;
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
                {
                    mx = fy;
                    my = 1.0f - fx;
                    break;
                }
                case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
                {
                    mx = 1.0f - fy;
                    my = fx;
                    break;
                }
                default:
                {
                }
            }
        }
        else
        {
        }
    }

    return Vector3(mx, my, pos.getZ());
}

float Magic3D::Camera::getBoxMinZ(const Matrix4& model, const Box& box)
{
    Vector3 cMin = box.corners[0];
    Vector3 cMax = box.corners[1];

    Matrix4 modelView = getView() * model;

    float cornersZ[8];
    //left up front
    cornersZ[0] = (modelView * Vector4(cMin, 1.0)).getZ();
    //right up front
    cornersZ[1] = (modelView * Vector4(Vector3(cMax.getX(), cMin.getY(), cMin.getZ()), 1.0)).getZ();
    //left down front
    cornersZ[2] = (modelView * Vector4(Vector3(cMin.getX(), cMax.getY(), cMin.getZ()), 1.0)).getZ();
    //right down front
    cornersZ[3] = (modelView * Vector4(Vector3(cMax.getX(), cMax.getY(), cMin.getZ()), 1.0)).getZ();

    //left up back
    cornersZ[4] = (modelView * Vector4(Vector3(cMin.getX(), cMin.getY(), cMax.getZ()), 1.0)).getZ();
    //right up back
    cornersZ[5] = (modelView * Vector4(Vector3(cMax.getX(), cMin.getY(), cMax.getZ()), 1.0)).getZ();
    //left down back
    cornersZ[6] = (modelView * Vector4(Vector3(cMin.getX(), cMax.getY(), cMax.getZ()), 1.0)).getZ();
    //right down front
    cornersZ[7] = (modelView * Vector4(cMax, 1.0)).getZ();

    float z = Math::min(cornersZ[0], cornersZ[1]);
    z = Math::min(z, cornersZ[2]);
    z = Math::min(z, cornersZ[3]);
    z = Math::min(z, cornersZ[4]);
    z = Math::min(z, cornersZ[5]);
    z = Math::min(z, cornersZ[6]);
    z = Math::min(z, cornersZ[7]);

    return z;
}

void Magic3D::Camera::render()
{
    Window* window = Renderer::getInstance()->getWindow();
    float ratio = window->getWidth() / window->getHeight();
    float tang = (float)tan(Math::radians(fovy) * 0.5f) ;
    float nh = getNear() * tang;
    float nw = nh * ratio;
    float fh = getFar()  * tang;
    float fw = fh * ratio;

    Matrix4 mat = getMatrixFromParent();
    Vector3 nc = mat.getCol3().getXYZ() + mat.getCol2().getXYZ() * getNear();
    Vector3 fc = mat.getCol3().getXYZ() + mat.getCol2().getXYZ() * getFar();

    Vector3 ftl = fc + (getDirectionUp() * fh * 0.5f) - (getDirectionRight() * fw * 0.5f);
    Vector3 ftr = fc + (getDirectionUp() * fh * 0.5f) + (getDirectionRight() * fw * 0.5f);
    Vector3 fbl = fc - (getDirectionUp() * fh * 0.5f) - (getDirectionRight() * fw * 0.5f);
    Vector3 fbr = fc - (getDirectionUp() * fh * 0.5f) + (getDirectionRight() * fw * 0.5f);

    Vector3 ntl = nc + (getDirectionUp() * nh * 0.5f) - (getDirectionRight() * nw * 0.5f);
    Vector3 ntr = nc + (getDirectionUp() * nh * 0.5f) + (getDirectionRight() * nw * 0.5f);
    Vector3 nbl = nc - (getDirectionUp() * nh * 0.5f) - (getDirectionRight() * nw * 0.5f);
    Vector3 nbr = nc - (getDirectionUp() * nh * 0.5f) + (getDirectionRight() * nw * 0.5f);

    ColorRGBA color = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
    Renderer::getInstance()->addLine(ntl, ntr, false, color);
    Renderer::getInstance()->addLine(ntr, nbr, false, color);
    Renderer::getInstance()->addLine(nbr, nbl, false, color);
    Renderer::getInstance()->addLine(nbl, ntl, false, color);

    Renderer::getInstance()->addLine(ftl, ftr, false, color);
    Renderer::getInstance()->addLine(ftr, fbr, false, color);
    Renderer::getInstance()->addLine(fbr, fbl, false, color);
    Renderer::getInstance()->addLine(fbl, ftl, false, color);

    Renderer::getInstance()->addLine(ntl, ftl, false, color);
    Renderer::getInstance()->addLine(ntr, ftr, false, color);
    Renderer::getInstance()->addLine(nbl, fbl, false, color);
    Renderer::getInstance()->addLine(nbr, fbr, false, color);
}

Magic3D::XMLElement* Magic3D::Camera::save(XMLElement* root)
{
    Object::save(root);
    if (root)
    {
        XMLElement* camera = root->GetDocument()->NewElement(M3D_CAMERA_XML);
        root->LinkEndChild(camera);

        XMLElement* attribute = camera->GetDocument()->NewElement(M3D_CAMERA_XML_VIEW);
        attribute->SetAttribute(M3D_CAMERA_XML_FOVY, getFovy());
        attribute->SetAttribute(M3D_CAMERA_XML_NEAR, getNear());
        attribute->SetAttribute(M3D_CAMERA_XML_FAR,  getFar());
        camera->LinkEndChild(attribute);

        saveInt(camera, M3D_CAMERA_XML_PROJECTION, getProjectionType());

        if (getType() == eOBJECT_CAMERA)
        {
            saveInt(camera, M3D_CAMERA_XML_INTERAXIAL,     getInteraxial());
            saveInt(camera, M3D_CAMERA_XML_INTERPUPILLARY, getInterpupillary());
            saveFloat(camera, M3D_CAMERA_XML_NECKDISTANCE, getNeckDistance());

            saveFloat(camera, M3D_CAMERA_XML_DISTORTION,     getDistortion());
            saveFloat(camera, M3D_CAMERA_XML_DISTORTIONCUBE, getDistortionCube());
            saveFloat(camera, M3D_CAMERA_XML_DISTORTIONX,    getDistortionX());
            saveFloat(camera, M3D_CAMERA_XML_DISTORTIONY,    getDistortionY());
        }
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Camera::load(XMLElement* root)
{
    Object::load(root);
    if (root)
    {
        XMLElement* camera = root->FirstChildElement( M3D_CAMERA_XML );

        if (camera)
        {
            XMLElement* xml = camera->FirstChildElement(M3D_CAMERA_XML_VIEW);
            if (xml)
            {
                float vFovy = getFovy();
                float vNear = getNear();
                float vFar  = getFar();

                xml->QueryFloatAttribute(M3D_CAMERA_XML_FOVY, &vFovy);
                xml->QueryFloatAttribute(M3D_CAMERA_XML_NEAR, &vNear);
                xml->QueryFloatAttribute(M3D_CAMERA_XML_FAR,  &vFar);

                setView(vFovy, vNear, vFar);
            }
            setProjectionType(PROJECTION(loadInt(camera, M3D_CAMERA_XML_PROJECTION)));

            if (getType() == eOBJECT_CAMERA)
            {
                setInteraxial(loadInt(camera, M3D_CAMERA_XML_INTERAXIAL));
                setInterpupillary(loadInt(camera, M3D_CAMERA_XML_INTERPUPILLARY));
                setNeckDistance(loadFloat(camera, M3D_CAMERA_XML_NECKDISTANCE));

                setDistortion(loadFloat(camera, M3D_CAMERA_XML_DISTORTION));
                setDistortionCube(loadFloat(camera, M3D_CAMERA_XML_DISTORTIONCUBE));
                setDistortionX(loadFloat(camera, M3D_CAMERA_XML_DISTORTIONX));
                setDistortionY(loadFloat(camera, M3D_CAMERA_XML_DISTORTIONY));
            }
        }
    }

    return root;
}
