uniform vec2 blurOrientation;
uniform int blurAmount;
uniform float blurStrength;
uniform bool blurGaussian;

float gaussian (float x, float deviation)
{
    return (1.0 / sqrt(2.0 * PI * deviation)) * exp(-((x * x) / (2.0 * deviation)));
}

void main()
{
    float halfBlur = float(blurAmount) * 0.5;
    float deviation = halfBlur * 0.35;
    float strength = 1.0 - blurStrength;
    vec4 colour = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 texColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec2 texelSize = vec2(1.0 / sizeTexture_0.x, 1.0 / sizeTexture_0.y) * blurOrientation;

    for (int i = 0; i < blurAmount; ++i)
    {
        float offset = float(i) - halfBlur;

        // Blur horizontal / Blur vertical
        texColor = texture2D(texture_0, uvs.xy + vec2(offset, offset) * texelSize);

#ifdef GL_ES
        colour += texColor;
#else
        colour += blurGaussian ? texColor * gaussian(offset * strength, deviation) : texColor;
#endif
    }

// Calculate average
#ifdef GL_ES
    colour /= float(blurAmount) * 0.666666;
#else
    colour /= blurGaussian ? 1.0 : float(blurAmount);
#endif

    // Apply colour
    gl_FragColor = vec4(clamp(colour.rgb, 0.0, 1.0), 1.0);
}
