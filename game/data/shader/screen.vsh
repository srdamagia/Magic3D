uniform vec4 distortion[64];
uniform int distortions;

void main()
{
    vec3 finalPos = position.xyz;

    float sx = window.x > window.y ? 1.0 : (window.x / window.y) * 2.0 - 1.0 - 0.02;
    float sy = window.y > window.x ? 1.0 : (window.y / window.x) * 2.0 - 1.0 - 0.02;

    for (int i = 0; i < distortions; i++)
    {
        int index = i * 2;
        int indexParams = index + 1;
        vec3 di = distortion[index].xyz;
        float d = distance(finalPos.xy, di.xy);
        float radius = distortion[index].z;
        if (d < radius && finalPos.x > -1.0 && finalPos.x < sx &&
                          finalPos.y > -1.0 && finalPos.y < sy)
        {
            int type = int(distortion[indexParams].x);
            float frequency = distortion[indexParams].y;
            float factor = distortion[indexParams].z;
            float wave = distortion[indexParams].w;
            highp float constant = PI2 * time * 1000.0 * frequency;
            float rx = (finalPos.x - di.x);
            float ry = (finalPos.y - di.y);
            float r  = (radius - d) * factor;

            vec2 s = vec2(rx, ry);
            float l = length(s);

            if (type == 0)
            {
                float t = sin(constant);
                finalPos += vec3(sin(rx) * r * radians(wave) * t, sin(ry) * r * radians(wave) * t, 0.0); 
            }
            else if (type == 1)
            {
                finalPos += vec3((s / l) * cos(l * radians(wave) - constant) * factor, 0.0);
            }
            else if (type == 2)
            {
                float f = 1.0 / factor;
                float t = constant;
                finalPos += vec3(sin(ry * f * radians(wave) + t) * factor, sin(rx * f * radians(wave) + t) * factor, 0.0); 
            }
        }
    }
    gl_Position = vec4(finalPos, 1.0);
    uvs.xy = uv_0;
    uvs.zw = uv_0;
}
