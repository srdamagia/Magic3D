void main()
{  
    gl_FragColor = glow(unlit(texture2D(texture_0, uvs.zw)));
}
