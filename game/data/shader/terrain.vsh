void main()
{
    updateVertex();
    vertexPosition.w = position.y;
    vertexNormal.w = dot(normal.xyz, vec3(0.0, 1.0, 0.0));
    vertexColor.xyz = position.xyz;
    if (vertexPosition.w < -0.5)
    {
        vertexColor.w = clamp((UNDERWATER_FOG * 0.3 - (-vertexPosition.w)) / (UNDERWATER_FOG * 0.3 - 0.0), 0.0, 1.0);
    }
}
