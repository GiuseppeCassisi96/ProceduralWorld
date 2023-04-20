#version 460 core
subroutine vec4 PostProcessingEffect();
uniform sampler2D frameTexture;
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
	     -1, -1, -1,
        -1,  8.2, -1,
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
    
    return vec4(col, 1.0);
}
subroutine(PostProcessingEffect)
vec4 GrayScaleEffect()
{
    vec4 Color = texture(frameTexture, vTexCoords);
    float average = 0.2126 * Color.r + 0.7152 * Color.g + 0.0722 * Color.b;
    return vec4(average, average, average, 1.0);
}

void main()
{
    fragColor = Effect();
}