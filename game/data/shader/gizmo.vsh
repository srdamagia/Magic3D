uniform int isVertexColor;

void main()
{
    if (renderMode == 0)
    {
        gl_PointSize = uv_0.x;
        gl_Position = matrixProjection * matrixModelView * vec4(position, 1.0);
    }
    else
    {
        updateSkinning();
    }

    if (renderMode == 2)
    {
        vertexColor = vec4(normal.xyz, 1.0);
    }
    else if (renderMode == 3)
    {
        vertexColor = vec4(uv_0.x, 0.0, uv_0.y, 1.0);
    }
    else if (isVertexColor == 0)
    {
        vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else
    {
        vertexColor = color;
    }
}
