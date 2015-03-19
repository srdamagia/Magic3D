uniform float height;

void main()
{
    updateVertex();

    vec3 movement = vec3(0.0, 0.0, 0.0);

    if (length(wind) != 0.0)
    {
        vec3 dir = normalize(wind);

        float frequency = length(wind);
        highp float constant = timeSin * 1000.0 * frequency;
        movement = dir * cos(constant) * color.r * height * sin(length(position));
    }

    vertexPosition.xyz += movement;
    gl_Position = matrixProjection * vertexPosition;
}
