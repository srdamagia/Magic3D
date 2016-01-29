uniform float amplitude;
uniform float height;
uniform bool pixel;

void main()
{
    updateVertex();

    float wave = 0.0;
    float x = 0.0;
    float z = 0.0;
    if (length(wind) != 0.0)
    {
        vec3 dir = normalize(vec3(wind.x, 0.0, wind.z));
        vec3 p = (position.xyz + dir) * dir;

        x = sin((p.x + color.g * amplitude) / amplitude + time * 1000.0);
        z = cos((p.z + color.g * amplitude) / amplitude + time * 1000.0);
        wave = x * z * height;
    }

    uvs.xy = vec2(0.0, 0.0);
    uvs.zw = uv_0;

    if (!pixel)
    {
        vec3 n = normalize(vec3(x, 3.0, z));
        vertexModelPosition.y += wave;
        vertexModelNormal.xyz = n;
        //vertexModelTangent.xyz *= n;

        vertexPosition = matrixModelView * vec4(vertexModelPosition, 1.0);
        vertexNormal = vec4((matrixModelView * vec4(vertexModelNormal.xyz, 0.0)).xyz, vertexModelPosition.y);
        //vertexTangent = vec4((matrixView * vec4(vertexModelTangent.xyz, 0.0)).xyz, vertexModelPosition.z);
        gl_Position = matrixProjection * vertexPosition;

        //uvs.xy = vec2(wave, wave);
        //uvs.zw += vec2(wave, wave);
    }
}
