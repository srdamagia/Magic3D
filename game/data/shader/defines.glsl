#define FOG
#define LIGHTS

#ifdef LIGHTS
    //#define VERTEX_LIGHTS
    //#define FAST_LIGHTS
#endif

//#define ESM
#define SHADOW_COLOR 0.666666
#ifdef ESM
    #define SHADOW_DEPTH 4.0
#else

    #define SHADOW_DEPTH 0.0005
#endif

#define SHADOW_POISON 2048.0
#define SHADOW_INTENSITY 0.15

#define PI  3.14159265
#define PI2 6.28318530

#ifdef GL_ES
#define MAX_LIGHTS 8
#else
#define MAX_LIGHTS 16
#endif

#define UNDERWATER
#define UNDERWATER_Y 24.0
#define UNDERWATER_COLOR vec4(0.333, 0.666, 1.0, 1.0)
#define UNDERWATER_FOG 50.0

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#else
#define highp
#endif

uniform int renderMode;

uniform mat4 matrixProjection;
uniform mat4 matrixModelView;
uniform mat4 matrixModel;
uniform mat4 matrixView;
uniform mat4 matrixShadows;
uniform mat4 matrixCamera;

uniform float zNear;
uniform float zFar;

uniform vec4 window;
uniform vec4 water;
uniform vec3 wind;

uniform highp float time;
uniform highp float timeSin;
uniform int random;

uniform int useTexture_0;
uniform int useTexture_1;
uniform int useTexture_Shadows;
uniform int useTexture_Reflection;
uniform int useTexture_Depth;

uniform vec2 sizeTexture_0;
uniform vec2 sizeTexture_1;
uniform bool renderToTexture;

uniform vec4 ambientColor;

uniform vec4 barrelDistortion;
uniform vec4 screenAspect;

// Structs
struct fog_info
{
    vec4 color;
    vec3 attenuation;
};

struct light_info {
    vec4 position; // if directional light, this must be normalized (so that we dont have to normalize it here) w = type
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation; //constantAttenuation, linearAttenuation, quadraticAttenuation, 0.0;
    vec4 cutoff; //spotExponent, spotCutoff, radius, near;
};

struct material_info {
    vec4 ambient;
    vec4 diffuse;
    vec4 emission;
    vec4 specular;
    float shininess;
    float alphaTest;
};

uniform light_info lights[MAX_LIGHTS];
uniform int lightCount;
uniform light_info lightShadow;

uniform fog_info fog;
uniform material_info material;

varying vec4 vertexPosition;
varying vec4 vertexNormal;
varying vec4 vertexTangent;
varying vec4 vertexShadow;
varying vec4 vertexColor;
varying vec4 uvs;
varying vec4 vertexLightColor;
varying float shadowDotNL;

float basicAttenuation(light_info light, vec3 lightVector)
{
    float z = dot(lightVector, lightVector);
    vec3 attenuationDistance = vec3(1.0, sqrt(z), z);
    return 1.0 / dot(attenuationDistance, light.attenuation.xyz);
}
