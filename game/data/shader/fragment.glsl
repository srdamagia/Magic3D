uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_Shadows;
uniform sampler2D texture_Reflection;
uniform sampler2D texture_Depth;

vec3 lightColor;

float ld(float zn, float zf, float d)
{
    return (2.0 * zn) / (zf + zn - d * (zf - zn));
}

float linearDepth(float z)
{    
    return ld(zNear, zFar, z);
}

float linearShadowsDepth(float z)
{
    return ld(lightShadow.cutoff.w, lightShadow.cutoff.z, z);
}

float linearDepth(vec2 uv)
{
    return linearDepth(texture2D(texture_Depth, uv).z);
}

vec2 screenUV()
{
    float x = window.x > window.y ? 1.0 : window.x / window.y;
    float y = window.x > window.y ? window.y / window.x : 1.0;

    vec2 result = vec2(gl_FragCoord.x / window.x, gl_FragCoord.y / window.y) * vec2(x, y) * vec2(1.0 / window.z, 1.0 / window.w);
#ifdef STEREOSCOPY
    if (result.x > x)
    {
        result.x -= x;
    }
#endif
    return result;
}

vec4 calcFog(in vec4 color)
{
    return vec4(mix(fog.color.rgb, color.rgb, vertexLightColor.a), color.a);
}

float linearDepthFactor()
{
    return 1.0 / (lightShadow.cutoff.z - lightShadow.cutoff.w);
}


const float packColor = 256.0;
const float packFactor = 1.0 / packColor;
vec4 pack (float depth)
{
    const vec4 bias = vec4(packFactor, packFactor, packFactor, 0.0);

    float r = depth;
    float g = fract(r * packColor);
    float b = fract(g * packColor);
    float a = fract(b * packColor);
    vec4 color = vec4(r, g, b, a);

    return color - (color.yzww * bias);
}

float unpack (vec4 color)
{
    const vec4 bitShifts = vec4(1.0,
                                1.0 / packColor,
                                1.0 / (packColor * packColor),
                                1.0 / (packColor * packColor * packColor));
    return dot(color, bitShifts);
}

bool checkAlpha(vec4 color)
{
    bool a = color.a < material.alphaTest;

    if (a)
    {
        discard;
    }

    return !a;
}

vec4 glow(vec4 color)
{
    vec4 result = color;

#ifdef GLOW
    if (useTexture_Depth == -1)
    {
        float z = gl_FragCoord.z;
        float d = texture2D(texture_Depth, gl_FragCoord.xy / GLOW_SIZE).z;

        result = z <= d + 0.00005 ? color : vec4(0.0, 0.0, 0.0, 1.0);
    }
#endif

    return result;
}

vec4 softParticles(vec4 color)
{
#ifndef GL_ES
    float z = gl_FragCoord.z;
    float d = texture2D(texture_Depth, gl_FragCoord.xy / DEPTH_SIZE).z;
#endif

    return color;
}

bool checkShadow(inout vec4 color)
{
#ifdef SOFT_SHADOWS
    bool result = false;

    if (useTexture_Shadows == -1)
    {
        color = pack(length(vertexPosition.xyz) * linearDepthFactor());
        result = true;
    }
#else
    bool result = true;
#endif
    return !result;
}

vec4 shadow(vec4 color)
{
    if (useTexture_Shadows > 0 && vertexShadow.w > 0.0 && shadowDotNL > 0.0)
    {
#ifdef SOFT_SHADOWS
        float z = unpack(texture2DProj(texture_Shadows, vertexShadow.xyw));
#else
        float z = texture2DProj(texture_Shadows, vertexShadow.xyw).z;
#endif
        float depthZ = vertexShadow.z;
        float c = SHADOW_DEPTH;

#ifndef FAST_LIGHTS
        #ifdef SOFT_SHADOWS
            depthZ = distance(vertexPosition.xyz, lightShadow.position.xyz) * linearDepthFactor();
            #ifdef ESM
            c = lightShadow.position.w == 0.0 ? lightShadow.cutoff.z / SHADOW_DEPTH * 2.0 : distance(vertexPosition.xyz, lightShadow.position.xyz);
            #endif
        #else
            depthZ = lightShadow.position.w == 0.0 ? vertexShadow.z : vertexShadow.z / vertexShadow.w;
            #ifdef ESM
            c = lightShadow.position.w == 0.0 ? lightShadow.cutoff.z / SHADOW_DEPTH * 2.0 : distance(vertexPosition.xyz, lightShadow.position.xyz) * lightShadow.cutoff.z * SHADOW_DEPTH;
            #endif
        #endif
#endif

#ifdef ESM
        float factor = clamp(exp(c * (z - depthZ)), 0.0, 1.0);
#else
        float factor = depthZ > z + c ? SHADOW_COLOR : 1.0;
#endif
        color.rgb *= factor;
    }

    return color;
}

void calcLight(light_info light, bool toon, bool bump, bool hemi, vec3 normal)
{
    float dist = distance(vertexPosition.xyz, light.position.xyz);

    // 0 - directional
    // 1 - point
    // 2 - spot
    vec3 lightDir = -normalize(vertexPosition.xyz - light.position.xyz);

    vec3 N = normalize(gl_FrontFacing ? vertexNormal.xyz : -vertexNormal.xyz);
    vec3 L = light.position.w == 0.0 ? light.direction.xyz : lightDir;
    vec3 E = -normalize(vertexPosition.xyz);

    if (bump)
    {
        vec3 n = N;
        vec3 t = vertexTangent.xyz;
        vec3 b = abs(normalize(cross(n, t)));
        mat3 basis = mat3(t, b, n);

        N = normalize(normal);
        L = L * basis;
        E = E * basis;
    }

    float attenuation = 1.0;

    if (light.position.w == 2.0)
    {
        float spotDot = dot(lightDir, light.direction.xyz);
        attenuation = step(cos(light.cutoff.y), spotDot) * pow(spotDot, light.cutoff.x);
    }

#ifndef GL_ES
    attenuation *= basicAttenuation(light, L);
#endif

    float NdotL =  hemi ? 0.5 * max(dot(L, N), 0.0) + 0.5 : max(dot(L, N), 0.0);
    float spec = pow(max(0.0, dot(reflect(-L, N), E)), material.shininess);

    vec3 lc = light.ambient.rgb + NdotL * light.diffuse.rgb;

#ifndef GL_ES
    if (light.position.w != 2.0 && light.cutoff.x > 0.0)
    {
        float att = 0.75 + (0.25 - (light.cutoff.x / 128.0) * 0.25);
        if (dist > light.cutoff.z * att)
        {
            float factor = (light.cutoff.z - light.cutoff.z * att);
            lc *= pow((light.cutoff.z - dist) / factor, light.cutoff.x / 64.0);
        }
    }
#endif

    lc += step(toon ? 0.5 : 0.0, spec) * spec * mix(material.specular, light.specular, 0.5).rgb;

    if (toon)
    {
        vec3 final  = step(0.95, NdotL) * vec3(0.5, 0.5, 0.5);
             final += step(0.5,  NdotL) * vec3(0.3, 0.3, 0.3);
             final += step(0.25, NdotL) * vec3(0.2, 0.2, 0.2);
             final += step(0.05, NdotL) * vec3(0.1, 0.1, 0.1);

        lc = mix(final, lc, NdotL);
    }

    lightColor += lc * attenuation * step(dist, light.cutoff.z);
}

vec4 unlit(vec4 color)
{
#ifdef SHADOWS
    checkShadow(color);
#endif
    return calcFog(color);
}

vec4 calcLight(vec4 color, bool toon, bool bump, bool hemi, vec3 normal)
{
    lightColor = vec3(0.0, 0.0, 0.0);

#ifdef SHADOWS
#ifdef SOFT_SHADOWS
    if (checkShadow(color))
#endif
#endif
    {
        vec4 result = vec4((material.ambient.rgb * ambientColor.rgb) + material.emission.rgb, color.a);
#ifdef LIGHTS
    #ifdef VERTEX_LIGHTS
        lightColor = vertexLightColor.rgb;
    #else
        if (lightCount > 0)
        {
            for (int i = 0; i < lightCount; i++)
            {
                calcLight(lights[i], toon, bump, hemi, normal);
            }
        }
        else
        {
            lightColor = vec3(1.0, 1.0, 1.0);
        }
    #endif

        result.rgb = material.diffuse.rgb * color.rgb * (result.rgb + lightColor);
#else
        result.rgb = result.rgb * color.rgb;
#endif
#ifdef SHADOWS
        result = shadow(result);
#endif
        return calcFog(clamp(result, 0.0, 1.0));
    }

    return color;
}

vec4 light(vec4 color)
{
    return calcLight(color, false, false, false, vec3(0.0, 0.0, 0.0));
}

vec4 hemi(vec4 color)
{
    return calcLight(color, false, false, true, vec3(0.0, 0.0, 0.0));
}

vec4 bump(vec4 color, vec4 normal)
{
    return calcLight(color, false, true, false, normal.xyz * 2.0 - 1.0);
}

vec4 hemiBump(vec4 color, vec4 normal)
{
    return calcLight(color, false, true, true, normal.xyz * 2.0 - 1.0);
}

vec4 toon(vec4 color)
{
    return calcLight(color, true, false, false, vec3(0.0, 0.0, 0.0));
}

vec4 hemiToon(vec4 color)
{
    return calcLight(color, true, false, true, vec3(0.0, 0.0, 0.0));
}

vec4 getTexturesMix(float factor, vec2 tile, vec2 scroll, vec2 speed)
{
    vec2 finalTile = vec2(1.0, 1.0) / tile;
    vec2 finalScroll = vec2(scroll.x * finalTile.x, (tile.y * finalTile.y - finalTile.y) - scroll.y * finalTile.y) + (time * speed);
    vec2 finalUV0 = (uvs.xy * finalTile) + finalScroll;
    vec2 finalUV1 = (uvs.zw * finalTile) + finalScroll;

    return mix(texture2D(texture_0, finalUV0), texture2D(texture_1, finalUV1), factor);
}

vec4 getTexturesMix(float factor, vec2 tile, vec2 scroll)
{
    return getTexturesMix(factor, tile, scroll, vec2(0.0, 0.0));
}

vec4 getOcclusion(vec4 color, float factor)
{
    float occlusion = 1.0 - clamp(1.0 - texture2D(texture_1, uvs.zw).a, 0.0, 1.0) * factor;
    return vec4(color.rgb * occlusion, color.a);
}
