void main()
{  
    gl_FragColor = glow(light(getTexturesMix(0.0, vec2(1.0, 1.0), vec2(0.0, 0.0))));
}
