void main()
{
    gl_FragColor = vertexColor + (renderMode == 2 || renderMode == 3 ? vec4(0.0, 0.0, 0.0, 0.0) : material.diffuse);
}
