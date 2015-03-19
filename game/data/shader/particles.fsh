void main()
{
    gl_FragColor = softParticles(glow(unlit(texture2D(texture_0, uvs.xy) * vertexColor)));
}
