uniform vec2 mouse;
uniform float radius;
uniform float heights[9];
uniform bool square;

void main()
{  
    vec2 uv = uvs.xy;
    vec2 uvb = uvs.xy;
    float height = 0.0;

    float min = 0.0;
    float max = 0.5;
    float factor = 1.0 / (max - min);
    float inc = -1.0;

    for (int i = 1; i < 9; i++)
    {
        float pixel = 0.005;
        min = heights[i - 1] + pixel;
        max = heights[i] + pixel;
        factor = 1.0 / (max - min);
        inc += 1.0;
        if ((vertexPosition.w) >= min && (vertexPosition.w) <= max)
        {        
            uv.y += 0.125 * inc;
            uvb.y += 0.125 * (inc + 1.0);
            height = 1.0 - ((max - vertexPosition.w) * factor);
        }        
    }

    vec4 color = texture2D(texture_0, uv.xy);
    vec4 blend = texture2D(texture_0, uvb.xy);
    vec4 angle = texture2D(texture_0, vec2(uv.x, uvs.y + 0.75));
    vec4 final = mix(color, blend, height);
    final = mix(final, angle, 1.0 - vertexNormal.w);

    vec4 teste = texture2D(texture_0, vec2(uv.x, uvs.y));

    if (square && 
        vertexColor.x >= mouse.x - radius && vertexColor.x <= mouse.x + radius &&
        vertexColor.z >= mouse.y - radius && vertexColor.z <= mouse.y + radius)
    {
        final = mix(final, vec4(0.0, 0.0, 1.0, 1.0), 0.5);
    }
    else if (distance(vertexColor.xz, mouse) < radius)
    {        
        final = mix(final, vec4(0.0, 0.0, 1.0, 1.0), 0.5);
    }

    gl_FragColor = light(final);
}
