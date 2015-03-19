uniform float amplitude;
uniform float height;
uniform bool pixel;

void main()
{
    updateVertex();

    float wave = 0.0;
    if (length(wind) != 0.0)
    {
        vec3 dir = normalize(vec3(wind.x, 0.0, wind.z));
        vec3 p = (position.xyz + dir) * dir;

        highp float variance = cos(length(p) * amplitude - timeSin * 1000.0 * length(wind));
        wave = variance * height;
    }

    uvs.xy = uv_0;
    uvs.zw = uv_0;

    if (!pixel)
    {
        vertexModelPosition.y += wave;
        vertexModelNormal.xyz = normalize(vec3(wave, 1.0, wave));
        vertexModelTangent.xyz *= wave;

        vertexPosition = matrixView * vec4(vertexModelPosition, 1.0);
        vertexNormal = vec4((matrixView * vec4(vertexModelNormal.xyz, 0.0)).xyz, vertexModelPosition.y);
        vertexTangent = vec4((matrixView * vec4(vertexModelTangent.xyz, 0.0)).xyz, vertexModelPosition.z);
        gl_Position = matrixProjection * vertexPosition;

        uvs.xy = vec2(wave, wave);
        uvs.zw += vec2(wave, wave);
    }
}
