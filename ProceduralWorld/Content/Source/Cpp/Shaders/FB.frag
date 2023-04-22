#version 460 core
subroutine vec4 PostProcessingEffect();
uniform sampler2D frameTexture;
uniform float pixel;
subroutine uniform PostProcessingEffect Effect;
uniform float screenWidth, screenHeight;

//Here I define the dimension of each pixel and offsets for the neighborhoods of current pixel
const float offsetX = 1.0 / screenWidth;
const float offsetY = 1.0 / screenHeight;


in vec2 vTexCoords;
out vec4 fragColor;

subroutine(PostProcessingEffect)
vec4 NOEffect()
{
    return texture(frameTexture, vTexCoords);
}

subroutine(PostProcessingEffect)
vec4 OutlineEffect()
{
    vec2 offsets[9] = vec2[]
    (
	    vec2(-offsetX,  offsetY), vec2( 0.0f,    offsetY), vec2( offsetX,  offsetY),
        vec2(-offsetX,  0.0f),    vec2( 0.0f,    0.0f),    vec2( offsetX,  0.0f),
        vec2(-offsetX, -offsetY), vec2( 0.0f,   -offsetY), vec2( offsetX, -offsetY) 
    );

    float kernel[9] = float[]
    (
	    -1, -1,  -1,
        -1,  8.2,-1,
        -1, -1,  -1
    );
	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(frameTexture, vTexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
}
subroutine(PostProcessingEffect)
vec4 GrayScaleEffect()
{
    vec4 Color = texture(frameTexture, vTexCoords);
    float average = 0.2126 * Color.r + 0.7152 * Color.g + 0.0722 * Color.b;
    return vec4(average, average, average, 1.0);
}

subroutine(PostProcessingEffect)
vec4 PixelEffect()
{
    /*Less is the value of pixel more is the dimension of 'dx' and 'dy' and consequently  
    the screen will appear in a pixelated way. 'dx' and 'dy' represents the dimension of 
    each pixel.
    */
    float dx = 15.0 * (1.0 / pixel);
    float dy = 10.0 * (1.0 / pixel);
    /*Now I sample the color from the lower left corner of each pixel, and I fill that whole 
    pixel with that color*/
    vec2 Coord = vec2(dx * ceil(floor(vTexCoords.x / dx)), dy * ceil(floor(vTexCoords.y / dy)));
    return texture(frameTexture, Coord);
}

subroutine(PostProcessingEffect)
vec4 NightVisionEffect()
{
    vec2 offsets[9] = vec2[]
    (
	    vec2(-offsetX,  offsetY), vec2( 0.0f,    offsetY), vec2( offsetX,  offsetY),
        vec2(-offsetX,  0.0f),    vec2( 0.0f,    0.0f),    vec2( offsetX,  0.0f),
        vec2(-offsetX, -offsetY), vec2( 0.0f,   -offsetY), vec2( offsetX, -offsetY) 
    );

    float kernel[9] = float[]
    (
	    -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(frameTexture, vTexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(0.0, col.g, 0.0, 1.0);
}

void main()
{
    fragColor = Effect();
}