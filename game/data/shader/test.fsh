uniform bool  b_boolean;

uniform sampler2D s_sampler2D;
uniform sampler2D s_samplerCube;

void main()
{
    vec4 texel = texture2D(s_sampler2D, uvs.xy);
    if (checkAlpha(texel))
    {
        vec4 normal = texture2D(s_samplerCube, uvs.xy);
        vec4 color = material.diffuse * vec4(texel.rgb, texel.a);
        if (b_boolean)
        {
            color = bump(color, normal);
        }
        else
        {
            color = mix(color, normal, 0.5);
        }
        gl_FragColor = calcFog(color);
    }
}
