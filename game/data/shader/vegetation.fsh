uniform bvec2 light_Hemi_Toon;

void main()
{
    gl_FragColor = light(texture2D(texture_0, uvs.zw));
}
