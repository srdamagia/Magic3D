//#define SIMPLE

uniform bvec2 light_Hemi_Toon;
uniform float transparency;
uniform float amplitude;
uniform float height;
uniform bool pixel;
uniform vec2 tile;

void main()
{
#ifdef REFLECTIONS
    vec2 uv0 = fract(screenUV());
#else
    vec2 uv0 = uvs.zy;    
#endif
    vec2 uv1 = uvs.zw;

#ifndef SIMPLE    
    if (pixel)
    {
        vec2 movement = vec2(0.0, 0.0);
        vec2 dir = normalize(vec2(wind.x, -wind.z));
        vec2 p = (uv1 + dir) * dir;

        highp float variance = sin(length(p) * amplitude - timeSin * 1000.0 * length(wind));
        movement.y = variance * height;
        uv0 += movement;
        uv1 += movement;
    }
    else
#endif
    {
        uv0 += uvs.xy;
    }

    vec2 finalTile = vec2(1.0, 1.0) / tile;
    
    highp vec2 finalUV = (uv1 * finalTile) + (normalize(wind.yx) * length(wind) * time * 25.0);

    vec4 color = texture2D(texture_0, finalUV);

#ifdef REFLECTIONS
    float clipPos = dot (vec3(matrixCamera[3].xyz), water.xyz) - water.w;

    if (useTexture_Reflection > 0 && clipPos >= 0.0)
    {
        color = mix(texture2D(texture_Reflection, uv0), color, 0.5);
    }
#endif
    
#ifdef SIMPLE
    gl_FragColor = color = calcLight(vec4(color.rgb, transparency), light_Hemi_Toon.y, false, light_Hemi_Toon.x, vec3(0.0, 0.0, 0.0));
#else    
    gl_FragColor = color = calcLight(vec4(color.rgb, transparency), light_Hemi_Toon.y, useTexture_1 > 0, light_Hemi_Toon.x, texture2D(texture_1, uv1).xyz * 2.0 - 1.0);
#endif
}

