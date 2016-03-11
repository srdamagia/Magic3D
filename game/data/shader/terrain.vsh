void main()
{
    updateVertex();
    vertexPosition.w = position.y;
    vertexNormal.w = dot(normal.xyz, vec3(0.0, 1.0, 0.0));
    vertexColor.xyz = position.xyz;
}
