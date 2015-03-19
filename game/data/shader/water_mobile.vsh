uniform float amplitude;
uniform float height;
uniform vec2 tile;

void main()
{
    updateVertex();

    float wave = 0.0;
    if (length(wind) != 0.0)
    {
        vec3 dir = normalize(vec3(wind.x, 0.0, wind.z));
        vec3 p = (position.xyz + dir) * dir;

        highp float variance = cos(length(p) * radians(amplitude) - time * 1000.0 * length(wind));
        wave = variance * height;
    }

    vec2 finalTile = vec2(1.0, 1.0) / tile;
    
    uvs.xy = vec2(wave, wave);
    uvs.zw = (uvs.zw * finalTile) + fract(normalize(wind.yx) * length(wind) * (time * 1000.0) * 0.025);

    vertexModelPosition.y += wave;
    vertexPosition = matrixView * vec4(vertexModelPosition, 1.0);

    gl_Position = matrixProjection * vertexPosition;

}
