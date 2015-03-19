void main()
{
    gl_FragColor = bump(texture2D(texture_0, uvs.xy), texture2D(texture_1, uvs.xy));
}
