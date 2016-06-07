void main()
{  
    gl_FragColor = glow(texture2D(texture_0, uvs.xy) * material.diffuse);
}
