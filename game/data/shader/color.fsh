uniform bvec2 light_Hemi_Toon;

uniform float factor0;
uniform float factor1;
uniform float factor2;

uniform vec3 color0;
uniform vec3 color1;
uniform vec3 color2;

void main()
{
    vec4 color = texture2D(texture_0, uvs.xy);

    //checkAlpha(color);
    float len = 1.0 - ((color.r + color.g + color.b) / 3.0);

    color.rgb += (len >= factor0 && len <= 0.89)    ? color0 :
                 (len >= factor1 && len <= factor0) ? color1 :
                 (len >= factor2 && len <= factor1) ? color2 : vec3(0.0, 0.0, 0.0);

    gl_FragColor = light(color);
}
