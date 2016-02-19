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

Magic3D::Light::Light(const Light& light, std::string name) : Camera(light, name)
{
    this->occluder = light.occluder;

    this->box = light.box;

    this->ambient = light.ambient;
    this->diffuse = light.diffuse;
    this->specular = light.specular;

    this->cutoff = light.cutoff;
    this->exponent = light.exponent;

    attenuationConstant = light.attenuationConstant;
    attenuationLinear = light.attenuationLinear;
    attenuationQuadratic = light.attenuationQuadratic;

    this->lightType = light.lightType;

    this->shadows = light.shadows;
    this->flare = light.flare;
    this->lens = light.lens;

    this->updateLensFlare = light.updateLensFlare;

    memcpy(this->flares, light.flares, sizeof(this->flares));
}

Magic3D::Light::Light(std::string name) : Camera(eOBJECT_LIGHT, name)
{
    setFovy(M3D_LIGHT_DEFAULT_FOVY);
    setFar(M3D_LIGHT_DEFAULT_FAR);
    ColorRGBA color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    ambient   = color;
    diffuse   = color;
    specular  = color;

    cutoff    = 0.0f;
    exponent  = 0.0f;

    attenuationConstant = 1.0f;
    attenuationLinear = 0.0f;
    attenuationQuadratic = 0.0f;

    lightType = eLIGHT_TYPE_POINT;

    shadows   = false;
    flare     = false;
    lens      = false;
    updateLensFlare = false;

    box = Box(Vector3(-0.125f, -0.125f, -0.125f), Vector3(0.125f, 0.125f, 0.125f));

    occluder  = NULL;

    Mesh* mesh = new Mesh(eMESH_TRIANGLES, M3D_DEFAULT_MATERIAL_LENSFLARE, false);

    Material* material = ResourceManager::getMaterials()->get(M3D_DEFAULT_MATERIAL_LENSFLARE);
    material->setTexture(0, ResourceManager::getTextures()->get(M3D_DEFAULT_PARTICLES_NAME));
    material->setAlphaTest(0.001f);
    material->setBlend(eBLEND_ADD);

    mesh->addMaterial(material);

    for (int i = 0; i < M3D_LIGHT_FLARES; i++)
    {
        flares[i].scale = 0.0f;
        flares[i].position = 0.0f;


        mesh->getData()->addQuad(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, eAXIS_Z);

        float* buffer;
        switch( i )
        {
            case 0:
            {
                flares[i].scale = 2.0f;
                flares[i].position = 0.0f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.625f, 0.875f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 1:
            {
                flares[i].scale = 2.0f;
                flares[i].position = 0.0f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.125f, 1.0f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 2:
            {
                flares[i].scale = 0.5f;
                flares[i].position = -0.15f;
                flares[i].color = ColorRGBA(0.9f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.875f, 0.75f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 3:
            {
                flares[i].scale = 0.25f;
                flares[i].position = -0.25f;
                flares[i].color = ColorRGBA(0.4f, 0.3f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.25f, 0.625f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 4:
            {
                flares[i].scale = 0.9f;
                flares[i].position = -0.35f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.875f, 0.75f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 5:
            {
                flares[i].scale = 0.5f;
                flares[i].position = -0.45f;
                flares[i].color = ColorRGBA(0.4f, 1.0f, 0.6f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.25f, 0.625f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 6:
            {
                flares[i].scale = 1.0f;
                flares[i].position = -0.6f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.875f, 0.75f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 7:
            {
                flares[i].scale = 0.75f;
                flares[i].position = -0.75f;
                flares[i].color = ColorRGBA(1.0f, 0.3f, 0.3f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.25f, 0.625f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 8:
            {
                flares[i].scale = 1.0f;
                flares[i].position = -0.9f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 0.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.25f, 0.625f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
            case 9:
            {
                flares[i].scale = 1.5f;
                flares[i].position = -1.0f;
                flares[i].color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);

                buffer = mesh->getData()->mapBuffer();
                mesh->getData()->setQuadTexture(buffer, i, 0, 0.875f, 0.875f, 0.125f, 0.125f);
                mesh->getData()->unmapBuffer();
                break;
            }
        }
    }

    mesh->getData()->createVbo();
    addMesh(mesh);

    //updateBoundingBox();
}

Magic3D::Light::~Light()
{

}

Magic3D::Light* Magic3D::Light::spawn(std::string name) const
{
    return (new Light(*this, name));
}

bool Magic3D::Light::update()
{
    bool result = Camera::update();

    ViewPort* view = Renderer::getInstance()->getCurrentViewPort();
    Camera* camera = view->getPerspective();
    Window* window = Renderer::getInstance()->getWindow();

    if (isFrustumLensFlare() && camera && window && (isShowingFlare() || isShowingLens() || updateLensFlare))
    {
        updateLensFlare = false;

        Vector3 sun_cam_dir = getPosition() - camera->getPosition();
        float distance = length(sun_cam_dir);
        sun_cam_dir = normalize(sun_cam_dir);

        float dp = dot( sun_cam_dir, camera->getDirectionFront() );

        float ang = 0.0f;
        if( dp <= 0.999f )
        {
            ang = Math::degrees(acosf( dp ));
        }

        if( ang < camera->getFovy())
        {
            float aspectX = window->getWidth() > window->getHeight() ? 1.0f : (float)window->getWidth() / (float)window->getHeight();
            float aspectY = window->getWidth() > window->getHeight() ? (float)window->getHeight() / (float)window->getWidth() : 1.0f;

            float aspect = window->getHeight() > window->getWidth() ? (float)window->getHeight() / (float)window->getWidth() : 1.0f;

            Vector3 lensflare_screen_pos = camera->getPosition2D(getMatrixFromParent().getTranslation(), view);

            Vector3 flares_screen_pos = lensflare_screen_pos;
            flares_screen_pos.setX(lensflare_screen_pos.getX() * 2.0f - 1.0f);
            flares_screen_pos.setY(lensflare_screen_pos.getY() * 2.0f - 1.0f);
            flares_screen_pos.setZ(lensflare_screen_pos.getZ());

            for (int i = 0; i < M3D_LIGHT_FLARES; i++)
            {
                float alpha = 0.0f;
                float sizeX = 0.0f;
                float sizeY = 0.0f;
                Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);

                ColorRGBA color;

                if (i > 1)
                {
                    color = flares[i].color;
                }
                else
                {
                    color = getColorDiffuse();
                }

                MeshData* data = getMeshes()->at(0)->getData();
                float* buffer = data->mapBuffer();

                Vector3 a = Renderer::getInstance()->getWindow()->getWindowScreenAspect();
                if ( (i > 1 && isShowingLens()) || (((i == 0 && ang < 5) || i == 1) && isShowingFlare()))
                {                    
                    alpha = (1.0f - ( ang / camera->getFovy()));
                    sizeX = a.getX() /* 0.5f*/ * flares[i].scale * aspectY * aspect;
                    sizeY = a.getY() /* 0.5f*/ * flares[i].scale * aspectX * aspect;

                    color.a = alpha * (i > 1 ? 0.3f : 1.0f) ;

                    float factor = 1.0f;
                    float scale = maxElem(getScale());
                    float frac = 1.0f / scale;
                    factor = (scale - distance / getFar() * 0.75f) * frac;

                    if (factor < 0.0f)
                    {
                        factor = 0.0f;
                    }

                    sizeX *= alpha * 2.0f * factor;
                    sizeY *= alpha * 2.0f * factor;

                    pos = lensflare_screen_pos + flares_screen_pos * flares[i].position;
                }

                data->setQuad(buffer, i, pos.getX() * a.getX() - sizeX * 0.5f, pos.getY() * a.getY() - sizeY * 0.5f, -pos.getZ(), sizeX, sizeY, color);
                data->unmapBuffer();
            }
        }
    }

    return result;
}

void Magic3D::Light::setOccluder(Object* occluder)
{
    this->occluder = occluder;
}

Magic3D::Object* Magic3D::Light::getOccluder()
{
    return occluder;
}

void Magic3D::Light::setLightType(LIGHT_TYPE type)
{
    this->lightType = type;
}

Magic3D::LIGHT_TYPE Magic3D::Light::getLightType()
{
    return lightType;
}

void Magic3D::Light::setColorAmbient(ColorRGBA color)
{
    ambient = color;
}

const Magic3D::ColorRGBA& Magic3D::Light::getColorAmbient()
{
    return ambient;
}

void Magic3D::Light::setColorDiffuse(ColorRGBA color)
{
    diffuse = color;
}

const Magic3D::ColorRGBA& Magic3D::Light::getColorDiffuse()
{
    return diffuse;
}

void Magic3D::Light::setColorSpecular(ColorRGBA color)
{
    specular = color;
}

const Magic3D::ColorRGBA& Magic3D::Light::getColorSpecular()
{
    return specular;
}

void Magic3D::Light::setCutOff(float cutoff)
{
    this->cutoff = cutoff;
}

float Magic3D::Light::getCutOff()
{
    return cutoff;
}

void Magic3D::Light::setExponent(float exponent)
{
    this->exponent = exponent;
}

float Magic3D::Light::getExponent()
{
    return exponent;
}

void Magic3D::Light::setAttenuationConstant(float attenuation)
{
    attenuationConstant = attenuation;
}

float Magic3D::Light::getAttenuationConstant()
{
    return attenuationConstant;
}

void Magic3D::Light::setAttenuationLinear(float attenuation)
{
    attenuationLinear = attenuation;
}

float Magic3D::Light::getAttenuationLinear()
{
    return attenuationLinear;
}

void Magic3D::Light::setAttenuationQuadratic(float attenuation)
{
    attenuationQuadratic = attenuation;
}

float Magic3D::Light::getAttenuationQuadratic()
{
    return attenuationQuadratic;
}

void Magic3D::Light::setShadows(bool shadows)
{
    this->shadows = shadows;
}

bool Magic3D::Light::isShadows()
{
    return shadows;
}

void Magic3D::Light::setShowFlare(bool flare)
{
    this->flare = flare;
    updateLensFlare = true;
}

bool Magic3D::Light::isShowingFlare()
{
    return flare;
}

void Magic3D::Light::setShowLens(bool lens)
{
    this->lens = lens;
    updateLensFlare = true;
}

bool Magic3D::Light::isShowingLens()
{
    return lens;
}

void Magic3D::Light::setFrustumLensFlare(bool frustum)
{
    frustumLensFlare = frustum;
}

bool Magic3D::Light::isFrustumLensFlare()
{
    return frustumLensFlare;
}

void Magic3D::Light::setRenderMatrix(const Matrix4& matrix)
{
    renderMatrix = matrix;
}

void Magic3D::Light::updateRenderMatrix(Camera* camera)
{
    renderMatrix = camera->getView() * inverse(getView());
}

const Magic3D::Matrix4& Magic3D::Light::getRenderMatrix()
{
    return renderMatrix;
}

const Magic3D::Box& Magic3D::Light::getBoundingBox()
{
    return box;
}

Magic3D::Box Magic3D::Light::computeBox(const Matrix4& viewProjection)
{
    Matrix4 t = getView() * inverse(viewProjection);
    std::vector<Vector4> v;
    v.push_back(t * Vector4(-1.f, 1.f, -1.f, 1.f));
    v.push_back(t * Vector4(1.f, 1.f, -1.f, 1.f));
    v.push_back(t * Vector4(1.f, -1.f, -1.f, 1.f));
    v.push_back(t * Vector4(-1.f, -1.f, -1.f, 1.f));
    v.push_back(t * Vector4(-1.f, 1.f, 1.f, 1.f));
    v.push_back(t * Vector4(1.f, 1.f, 1.f, 1.f));
    v.push_back(t * Vector4(1.f, -1.f, 1.f, 1.f));
    v.push_back(t * Vector4(-1.f, -1.f, 1.f, 1.f));

    size_t vsize = v.size();
    for (size_t i = 0; i < vsize; i++)
    {
        v[i] = v[i] / v[i].getW();
    }

    float dist = 10000.0f;
    Vector3 bl = Vector3(dist, dist, dist);
    Vector3 tr = Vector3(-dist, -dist, -dist);

    for (size_t i = 0; i < vsize; i++)
    {
        if (v[i].getX() < bl.getX())
        {
            bl.setX(v[i].getX());
        }
        if (v[i].getX() > tr.getX())
        {
            tr.setX(v[i].getX());
        }
        if (v[i].getY() < bl.getY())
        {
            bl.setY(v[i].getY());
        }
        if (v[i].getY() > tr.getY())
        {
            tr.setY(v[i].getY());
        }
        if (v[i].getZ() < bl.getZ())
        {
            bl.setZ(v[i].getZ());
        }
        if (v[i].getZ() > tr.getZ())
        {
            tr.setZ(v[i].getX());
        }
    }

    return Box(bl, tr);
}

Magic3D::Matrix4 Magic3D::Light::computeShadowProjection()
{
    Camera* camera = Renderer::getInstance()->getCurrentViewPort()->getPerspective();

    Matrix4 projection = camera->getProjection();
    Matrix4 view = camera->getView();

    Matrix4 invProjection = inverse(camera->getProjection());
    std::vector<Vector4> v;
    v.push_back(invProjection * Vector4(-1.f,  1.f, -1.f, 1.f));
    v.push_back(invProjection * Vector4( 1.f,  1.f, -1.f, 1.f));
    v.push_back(invProjection * Vector4( 1.f, -1.f, -1.f, 1.f));
    v.push_back(invProjection * Vector4(-1.f, -1.f, -1.f, 1.f));
    v.push_back(invProjection * Vector4(-1.f,  1.f,  1.f, 1.f));
    v.push_back(invProjection * Vector4( 1.f,  1.f,  1.f, 1.f));
    v.push_back(invProjection * Vector4( 1.f, -1.f,  1.f, 1.f));
    v.push_back(invProjection * Vector4(-1.f, -1.f,  1.f, 1.f));

    float zNear = -(v[0] / v[0].getW()).getZ();
    float zFar = -(v[4] / v[4].getW()).getZ();
    float fov = atanf(1.f / projection[1][1]) * 2.0f;
    float ratio = projection[1][1] / projection[0][0];

    Vector4 splitFar = Vector4(zFar, zFar, zFar, zFar);
    Vector4 splitNear = Vector4(zNear, zNear, zNear, zNear);
    float lambda = 0.8f;
    float j = 1.0f;
    for (int i = 0; i < 4 - 1; i++, j += 1.0f)
    {
        splitFar[i] = Math::mix(zNear + (j / 4.0f) * (zFar - zNear), zNear * powf(zFar / zNear, j / 4.0f), lambda);
        splitNear[i + 1] = splitFar[i];
    }

    int i = 1;
    float zn = zNear;
    float zf = splitFar[i];
    //for (int i = 0; i < 4.0f; ++i)
    //{
         Matrix4 cameraViewProjection = Matrix4::perspective(fov, ratio, zn, zf) * view;
         Box box = computeBox(cameraViewProjection);
         Matrix4 result = Matrix4::orthographic(box.corners[0].getX(), box.corners[1].getX(), box.corners[0].getY(), box.corners[1].getY(), -box.corners[1].getZ(), -box.corners[0].getZ());

         //_shadowProjections[i] = projection;
         zNear = splitFar[i];
    //}

    return result;
}

Magic3D::XMLElement* Magic3D::Light::save(XMLElement* root)
{
    Camera::save(root);
    if (root)
    {
        XMLElement* light = root->GetDocument()->NewElement(M3D_LIGHT_XML);
        root->LinkEndChild(light);

        saveColorRGBA(light, M3D_LIGHT_XML_COLOR_AMBIENT,  ambient);
        saveColorRGBA(light, M3D_LIGHT_XML_COLOR_DIFFUSE,  diffuse);
        saveColorRGBA(light, M3D_LIGHT_XML_COLOR_SPECULAR, specular);

        saveInt(      light, M3D_LIGHT_XML_LIGHT_TYPE,     lightType);
        saveFloat(    light, M3D_LIGHT_XML_SPOT_CUTOFF,    cutoff);
        saveFloat(    light, M3D_LIGHT_XML_SPOT_EXPONENT,  exponent);

        saveFloat(    light, M3D_LIGHT_XML_ATTENUATION_CONSTANT,  attenuationConstant);
        saveFloat(    light, M3D_LIGHT_XML_ATTENUATION_LINEAR,    attenuationLinear);
        saveFloat(    light, M3D_LIGHT_XML_ATTENUATION_QUADRATIC, attenuationQuadratic);

        saveBool(      light, M3D_LIGHT_XML_SHADOWS, isShadows());
        saveBool(      light, M3D_LIGHT_XML_FLARE, isShowingFlare());
        saveBool(      light, M3D_LIGHT_XML_LENS,  isShowingLens());
    }

    return root;
}

Magic3D::XMLElement* Magic3D::Light::load(XMLElement* root)
{
    Camera::load(root);
    if (root)
    {
        XMLElement* light = root->FirstChildElement(M3D_LIGHT_XML);

        ambient   = loadColorRGBA(light, M3D_LIGHT_XML_COLOR_AMBIENT);
        diffuse   = loadColorRGBA(light, M3D_LIGHT_XML_COLOR_DIFFUSE);
        specular  = loadColorRGBA(light, M3D_LIGHT_XML_COLOR_SPECULAR);

        lightType = LIGHT_TYPE(loadInt(light, M3D_LIGHT_XML_LIGHT_TYPE));
        cutoff    = loadFloat(light, M3D_LIGHT_XML_SPOT_CUTOFF);
        exponent  = loadFloat(light, M3D_LIGHT_XML_SPOT_EXPONENT);

        attenuationConstant  = loadFloat(light, M3D_LIGHT_XML_ATTENUATION_CONSTANT);
        attenuationLinear    = loadFloat(light, M3D_LIGHT_XML_ATTENUATION_LINEAR);
        attenuationQuadratic = loadFloat(light, M3D_LIGHT_XML_ATTENUATION_QUADRATIC);

        shadows   = loadBool(light, M3D_LIGHT_XML_SHADOWS);
        flare     = loadBool(light, M3D_LIGHT_XML_FLARE);
        lens      = loadBool(light, M3D_LIGHT_XML_LENS);
    }

    return root;
}
