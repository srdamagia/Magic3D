uniform vec2 mouse;
uniform float radius;
uniform float heights[9];
uniform bool square;

void main()
{  
    vec2 uv = uvs.xy;
    vec2 uvb = uv;
    float height = 0.0;
    float inc = -1.0;
    float pixel = 0.005;
    float max = 0.0;
    float min = 0.0;
    float factor = 1.0;
    
    for (int i = 1; i < 9; i++)
    {
        max = heights[i] + pixel;
        min = heights[i - 1] + pixel;
        inc += 1.0;
        if (vertexPosition.w >= min && vertexPosition.w <= max)
        {
            uv.y += 0.125 * inc;
            uvb.y += 0.125 * (inc + 1.0);
            factor = 1.0 / (max - min);
            height = 1.0 - (vertexPosition.w - min) * factor;
            if (mod(float(i), 2.0) == 1.0)
            {
                vec2 tmp = uv;
                uv = uvb;
                uvb = tmp;
                height = 1.0 - height;
            }
        }
    }

    vec4 color = texture2D(texture_0, uv);
    vec4 blend = texture2D(texture_0, uvb);
    vec4 angle = texture2D(texture_0, vec2(uv.x, uvs.y + 0.75));
    vec4 final = mix(blend, color, height);    
    final = mix(final, angle, 1.0 - vertexNormal.w);
    //final = vec4(height, height, height, 1.0);

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
