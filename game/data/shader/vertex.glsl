attribute vec3 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec4 color;
attribute vec2 uv_0;
attribute vec2 uv_1;
attribute vec4 weights;
attribute vec4 bones;

uniform mat4 skin[50];
uniform bool skeleton;

vec3 vertexModelPosition;
vec3 vertexModelNormal;
vec3 vertexModelTangent;

const mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
                       0.0, 0.5, 0.0, 0.0,
                       0.0, 0.0, 0.5, 0.0,
                       0.5, 0.5, 0.5, 1.0);

float fogLinear(float distanceToEye)
{
    return clamp((fog.attenuation.y - distanceToEye) / (fog.attenuation.y - fog.attenuation.x), 0.0, 1.0);
}

float fogExp(float distanceToEye)
{
    return clamp(exp(-(distanceToEye * fog.attenuation.z)), 0.0, 1.0);
}

float fogExp2(float distanceToEye)
{
    float f = distanceToEye * fog.attenuation.z;
    return clamp(exp(-(f * f)), 0.0, 1.0);
}

void updateFog()
{
    vertexLightColor.a = fog.attenuation.y > fog.attenuation.x && fog.attenuation.y > 0.0 ? fogLinear(-vertexPosition.z) : 1.0;
}

void updateShadow()
{
    shadowDotNL = 0.0;

#ifdef SHADOWS
    if (useTexture_Shadows > 0)
    {
        vertexShadow = bias * (matrixShadows * vec4(vertexModelPosition, 1.0));

        vec3 N = vertexNormal.xyz;
        vec3 L = lightShadow.position.w == 0.0 ? lightShadow.direction.xyz : -normalize(vertexPosition.xyz - lightShadow.position.xyz);

        shadowDotNL = dot(N, L);
    }
    else
#endif
    {
        vertexShadow = vec4(0.0, 0.0, 0.0, 0.0);        
    }
}

void calcFastLight()
{
    light_info light = lights[0];
    vec3 N = vertexNormal.xyz;
    vec3 L = light.direction.xyz;
    vec3 E = -normalize(vertexPosition.xyz);

    float NdotL =  max(dot(L, N), 0.0);
    float spec = pow(max(0.0, dot(reflect(-L, N), E)), material.shininess);

    vertexLightColor.rgb = light.ambient.rgb + NdotL * light.diffuse.rgb;

    vertexLightColor.rgb += spec * mix(material.specular, light.specular, 0.5).rgb;
}

void calcVertexLight(light_info light)
{
    float dist = distance(vertexPosition.xyz, light.position.xyz);

    // 0 - directional
    // 1 - point
    // 2 - spot
    vec3 lightDir = -normalize(vertexPosition.xyz - light.position.xyz);

    vec3 N = vertexNormal.xyz;
    vec3 L = light.position.w == 0.0 ? light.direction.xyz : lightDir;
    vec3 E = -normalize(vertexPosition.xyz);

    float attenuation = 1.0;

    if (light.position.w == 2.0)
    {
        float spotDot = dot(lightDir, light.direction.xyz);
        attenuation = step(cos(light.cutoff.y), spotDot) * pow(spotDot, light.cutoff.x);
    }

    attenuation *= basicAttenuation(light, L);

    float NdotL =  max(dot(L, N), 0.0);
    float spec = pow(max(0.0, dot(reflect(-L, N), E)), material.shininess);

    vec3 lc = light.ambient.rgb + NdotL * light.diffuse.rgb;

    lc += spec * mix(material.specular, light.specular, 0.5).rgb;

    vertexLightColor.rgb += lc * attenuation * step(dist, light.cutoff.z);
}

void updateBones()
{
#ifdef SKINNING
    if (skeleton)
    {
        bool havebone = false;
        float w = 0.0;

        vertexModelPosition = vec3(0.0, 0.0, 0.0);
        vertexModelNormal   = vec3(0.0, 0.0, 0.0);
        #ifndef VERTEX_LIGHTS
        vertexModelTangent  = vec3(0.0, 0.0, 0.0);
        #endif

        for (int i = 0; i < 4; i++)
        {
            int boneIndex = int(bones[i]);
            if (boneIndex == -1)
            {
                break;
            }

            w += weights[i];

            vertexModelPosition += vec4(skin[boneIndex] * vec4(position, 1.0)).xyz * weights[i];
            vertexModelNormal   += vec4(skin[boneIndex] * vec4(normal,   0.0)).xyz * weights[i];
            #ifndef VERTEX_LIGHTS
            vertexModelTangent  += vec4(skin[boneIndex] * vec4(tangent,  0.0)).xyz * weights[i];
            #endif

            havebone = true;
        }

        if (havebone)
        {
            w = 1.0 / (w != 0.0 ? w : 1.0);

            vertexModelPosition *= w;
            vertexModelNormal   *= w;
            #ifndef VERTEX_LIGHTS
            vertexModelTangent  *= w;
            #endif
        }
    }
#endif
}

void updateVars()
{
    vertexModelPosition = position;
    vertexModelNormal   = normal;
    vertexModelTangent  = tangent;
}

void updateTransform()
{
    vertexModelPosition = vec4(matrixModel * vec4(vertexModelPosition,  1.0)).xyz;
    vertexModelNormal   = vec4(matrixModel * vec4(vertexModelNormal,    0.0)).xyz;
    #ifndef VERTEX_LIGHTS
    vertexModelTangent  = vec4(matrixModel * vec4(vertexModelTangent,   0.0)).xyz;
    #endif

    vertexPosition = matrixView           * vec4(vertexModelPosition, 1.0);
    vertexNormal   = normalize(matrixView * vec4(vertexModelNormal,   0.0));
    #ifndef VERTEX_LIGHTS
    vertexTangent  = normalize(matrixView * vec4(vertexModelTangent,  0.0));
    #endif

    uvs.xy = uv_0;
    uvs.zw = uv_1;
    vertexColor = color;

    gl_Position = matrixProjection * vertexPosition;
}

void updateVarying()
{
    updateTransform();

    vertexLightColor = vec4(0.0, 0.0, 0.0, 0.0);

#ifdef VERTEX_LIGHTS
    if (lightCount > 0)
    {
#ifdef FAST_LIGHTS
        calcFastLight();
#else
        for (int i = 0; i < lightCount; i++)
        {
            calcVertexLight(lights[i]);
        }
#endif
    }
    else
    {
        vertexLightColor = vec4(1.0, 1.0, 1.0, 0.0);
    }
#endif

    updateShadow();
    updateFog();
}

void updateVertex()
{
    updateVars();
    updateVarying();
}

void updateSkinning()
{
    updateVars();
    updateBones();
    updateVarying();
}

void updateSimple()
{
    updateVars();
    updateTransform();

    vertexLightColor = vec4(0.0, 0.0, 0.0, 1.0);

    shadowDotNL = 0.0;
    vertexShadow = vec4(0.0, 0.0, 0.0, 0.0);
}
