void main()
{  
    vec4 color = texture2D(texture_0, uvs.xy) * material.diffuse;
    checkAlpha(color);
    gl_FragColor = glow(color);
}
