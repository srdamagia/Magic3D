uniform bvec2 light_Hemi_Toon;

void main()
{
    vec4 color = texture2D(texture_0, uvs.zw);
    checkAlpha(color);
    gl_FragColor = light(color);
}
