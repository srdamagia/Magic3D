//#define DETECT_BORDER
//#define SOBEL

uniform vec4 edgeColor;
uniform float edgeTest;

uniform float threshold_0;
uniform float threshold_1;
uniform float threshold_value;
uniform bool stereoscopy;

float threshold(in float thr1, in float thr2 , in float val) 
{
    return val < thr1 ? 0.0 : val > thr2 ? 1.0 : val;
}

// averaged pixel intensity from 3 color channels
float avg_intensity(in vec4 pix) 
{
    return (pix.r + pix.g + pix.b) / 3.0;
}

vec4 get_pixel(in vec2 coords, in float dx, in float dy) 
{
    return texture2D(texture_Depth, uvs.xy + vec2(dx, dy));
}

// returns pixel color
float EdgeSobel(in vec2 coords)
{
    float dxtex = 1.0 / sizeTexture_0.x;
    float dytex = 1.0 / sizeTexture_0.y;
    float pix[9];
    int k = -1;
    float delta;

    // read neighboring pixel intensities
    for (int i= -1; i < 2; i++) 
    {
        for(int j = -1; j < 2; j++) 
        {
            k++;
            pix[k] = avg_intensity(get_pixel(coords, float(i) * dxtex, float(j) * dytex));
        }
    }

    // average color differences around neighboring pixels
    delta = (abs(pix[1]-pix[7])+
             abs(pix[5]-pix[3])+
             abs(pix[0]-pix[8])+
             abs(pix[2]-pix[6])) / 4.0;

    return threshold(threshold_0, threshold_1, clamp(threshold_value * delta, 0.0, 1.0));
}

float EdgeDepth(in vec2 coords)
{
    float w = (1.0 / sizeTexture_0.x);
    float h = (1.0 / sizeTexture_0.y);

    float depth = abs(texture2D(texture_Depth, coords.xy + vec2(0.0, -h) ).z
                    + texture2D(texture_Depth, coords.xy + vec2(-w,  0.0)).z
              - 8.0 * texture2D(texture_Depth, coords.xy + vec2(0.0, 0.0)).z
                    + texture2D(texture_Depth, coords.xy + vec2( w,  0.0)).z
                    + texture2D(texture_Depth, coords.xy + vec2(0.0,  h) ).z
                    + texture2D(texture_Depth, coords.xy + vec2(-w,  -h) ).z
                    + texture2D(texture_Depth, coords.xy + vec2(-w,   h) ).z
                    + texture2D(texture_Depth, coords.xy + vec2( w,  -h) ).z
                    + texture2D(texture_Depth, coords.xy + vec2( w,   h) ).z);
    return depth;
}

void main()
{
    vec4 finalColor = texture2D(texture_0, uvs.xy);

#ifndef GL_ES
    if (renderMode == 4)
    {
        float color = linearDepth(uvs.xy);
        finalColor = vec4(color, color, color, 1.0);
    }
#endif

#ifdef DETECT_BORDER
    float fXIndex = uvs.x * sizeTexture_0.x;
    float fYIndex = uvs.y * sizeTexture_0.y;

    // image boundaries Top, Bottom, Left, Right pixels
    if(renderMode == 0 && !( fYIndex < 1.0 || fYIndex > sizeTexture_0.x - 1.0 || 
                             fXIndex < 1.0 || fXIndex > sizeTexture_0.y - 1.0 ))
    {
#ifdef SOBEL
        bool isEdge = EdgeSobel(uvs.xy) != 0.0;
#else
        bool isEdge = edgeTest <= EdgeDepth(uvs.xy);
#endif
    
        if (isEdge)
        {
            //finalColor = useTexture_0 > 0 ? texture2D(texture_0, uvs.xy) : vec4(1.0, 1.0, 1.0, 1.0);        
            finalColor = edgeColor;//mix(texture2D(texture_0, uvs.xy), edgeColor, 0.5); 
        }
    }
#endif

    if (stereoscopy)
    {
        vec2 eyeUV;
        if (uvs.x > screenAspect.x * 0.5)
        {
            eyeUV = uvs.xy * vec2(2.0 * screenAspect.z, screenAspect.w) - vec2(1.0, 0.0);
        }
        else
        {
            eyeUV = uvs.xy * vec2(2.0 * screenAspect.z, screenAspect.w);
        }

        float r2 = (eyeUV.x - 0.5) * (eyeUV.x - 0.5) + (eyeUV.y - 0.5) * (eyeUV.y - 0.5);
        float fx = barrelDistortion.x + (r2 * (barrelDistortion.z + barrelDistortion.w * sqrt(r2)));
        float fy = barrelDistortion.y + (r2 * (barrelDistortion.z + barrelDistortion.w * sqrt(r2)));
        vec2 realCoordOffs;
        realCoordOffs.x = fx * (eyeUV.x - 0.5) + 0.5;
        realCoordOffs.y = fy * (eyeUV.y - 0.5) + 0.5;

        if (realCoordOffs.x >= 0.0 && realCoordOffs.x <= 1.0 && realCoordOffs.y >= 0.0 && realCoordOffs.y <= 1.0)
        {
            //finalColor = texture2D(texture_0, realCoordOffs * vec2(screenAspect.x * 0.5, screenAspect.y));
            realCoordOffs *= vec2(screenAspect.x * 0.5, screenAspect.y);
            if (uvs.x > screenAspect.x * 0.5)
            {
                realCoordOffs += vec2(screenAspect.x * 0.5, 0.0);
            }

            finalColor = texture2D(texture_0, realCoordOffs);
        }
        else
        {
            finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }

    gl_FragColor.rgb = finalColor.rgb;
    gl_FragColor.a = 1.0;
#ifdef GLOW
    vec4 glow = useTexture_Depth > 0 ? texture2D(texture_1, uvs.xy) : vec4(0.0, 0.0, 0.0, 1.0);
    gl_FragColor = clamp((glow + finalColor) - (glow * finalColor), 0.0, 1.0);
    gl_FragColor.a = 1.0;
#endif
}
