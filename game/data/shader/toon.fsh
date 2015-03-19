void main()
{
    gl_FragColor = toon(texture2D(texture_0, uvs.xy));
}
