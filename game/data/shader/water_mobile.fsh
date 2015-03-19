uniform float transparency;

void main()
{    
#ifdef REFLECTIONS
    vec2 uv0 = fract(screenUV()); 
#else
    vec2 uv0 = uvs.xy;   
#endif
    vec2 uv1 = uvs.zw;

    uv0 += uvs.xy;    

    vec4 color = texture2D(texture_0, uv1);

#ifdef REFLECTIONS
    float clipPos = dot (vec3(matrixCamera[3].xyz), water.xyz) - water.w;

    if (useTexture_Reflection > 0 && clipPos >= 0.0)
    {
        color = mix(texture2D(texture_Reflection, uv0), color, 0.5);
    }
#endif

    gl_FragColor = light(vec4(color.rgb, transparency));
}

