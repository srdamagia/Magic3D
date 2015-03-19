void main()
{
    vec4 color = texture2D(texture_0, uvs.xy);
#ifndef SOFT_SHADOWS
    float z = linearDepth(color.z);
    color = vec4(z, z, z, 1.0);
#endif
    gl_FragColor = color;
}
