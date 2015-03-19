void main()
{  
    gl_FragColor = texture2D(texture_0, uvs.xy) * material.diffuse;
}
