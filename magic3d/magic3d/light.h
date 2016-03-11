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

#ifndef MAGIC3D_LIGHT_H
#define MAGIC3D_LIGHT_H

#include <magic3d/object.h>

namespace Magic3D
{
#define M3D_LIGHT_XML                        "Light"
#define M3D_LIGHT_XML_COLOR_AMBIENT          "color_ambient"
#define M3D_LIGHT_XML_COLOR_DIFFUSE          "color_diffuse"
#define M3D_LIGHT_XML_COLOR_SPECULAR         "color_specular"
#define M3D_LIGHT_XML_LIGHT_TYPE             "light_type"
#define M3D_LIGHT_XML_SPOT_CUTOFF            "spot_cutoff"
#define M3D_LIGHT_XML_SPOT_EXPONENT          "spot_exponent"
#define M3D_LIGHT_XML_ATTENUATION_CONSTANT   "attenuation_constant"
#define M3D_LIGHT_XML_ATTENUATION_LINEAR     "attenuation_linear"
#define M3D_LIGHT_XML_ATTENUATION_QUADRATIC  "attenuation_quadratic"
#define M3D_LIGHT_XML_RADIUS                 "radius"
#define M3D_LIGHT_XML_SHADOWS                "shadows"
#define M3D_LIGHT_XML_FLARE                  "flare"
#define M3D_LIGHT_XML_LENS                   "lens"

#define M3D_LIGHT_FLARES 10
#define M3D_LIGHT_DEFAULT_FOVY  90.0f
#define M3D_LIGHT_DEFAULT_FAR  100.0f

enum LIGHT_TYPE
{
    eLIGHT_TYPE_DIRECTIONAL,
    eLIGHT_TYPE_POINT,
    eLIGHT_TYPE_SPOT
};

struct LensFlare
{
    float scale;
    float position;
    ColorRGBA color;
};

class Light : public Camera
{
private:
    Object* occluder;

    Matrix4 renderMatrix;

    Box box;

    ColorRGBA ambient;
    ColorRGBA diffuse;
    ColorRGBA specular;

    LensFlare flares[M3D_LIGHT_FLARES];

    float cutoff;
    float exponent;

    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;

    LIGHT_TYPE lightType;

    bool shadows;
    bool flare;
    bool lens;

    bool frustumLensFlare;
    bool updateLensFlare;

protected:
    Light(const Light& light, std::string name);

public:
    Light(std::string name);
    ~Light();
    virtual void* spawn(std::string name) const;

    virtual bool update();

    void setOccluder(Object* occluder);
    Object* getOccluder();

    void setLightType(LIGHT_TYPE type);
    LIGHT_TYPE getLightType();

    void setColorAmbient(ColorRGBA color);
    const ColorRGBA& getColorAmbient();

    void setColorDiffuse(ColorRGBA color);
    const ColorRGBA& getColorDiffuse();

    void setColorSpecular(ColorRGBA color);
    const ColorRGBA& getColorSpecular();

    void setCutOff(float cutoff);
    float getCutOff();

    void setExponent(float exponent);
    float getExponent();

    void setAttenuationConstant(float attenuation);
    float getAttenuationConstant();
    void setAttenuationLinear(float attenuation);
    float getAttenuationLinear();
    void setAttenuationQuadratic(float attenuation);
    float getAttenuationQuadratic();

    void setShadows(bool shadows);
    bool isShadows();

    void setShowFlare(bool flare);
    bool isShowingFlare();
    void setShowLens(bool lens);
    bool isShowingLens();

    void setFrustumLensFlare(bool frustum);
    bool isFrustumLensFlare();

    void setRenderMatrix(const Matrix4& matrix);
    void updateRenderMatrix(Camera* camera);
    const Matrix4& getRenderMatrix();

    virtual const Box& getBoundingBox();

    Box computeBox(const Matrix4& viewProjection);
    Matrix4 computeShadowProjection();

    virtual XMLElement* save(XMLElement* root);
    virtual XMLElement* load(XMLElement* root);
};

}

#endif // LIGHT_H
