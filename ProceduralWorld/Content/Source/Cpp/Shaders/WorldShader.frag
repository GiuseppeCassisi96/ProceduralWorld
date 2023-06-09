#version 460 core

// the "type" of the Subroutine
subroutine vec4 light();

// ambient, diffusive and specular components (passed from the application)
uniform vec3 albedo;
uniform vec3 specularColor;
// weight of the components
uniform float Ka;
uniform float Kd;
uniform float Ks;
//Alpha component of specular
uniform float shininess;
//Intensity of light
uniform float lightIntensity;
uniform sampler2D BiomeMap;
uniform sampler2D Lawn;
uniform sampler2D Forest;
uniform sampler2D Mountain;
uniform samplerCube skybox;
uniform bool toonShadingIsEnabled;
// Subroutine Uniform (it is conceptually similar to a C pointer function)
subroutine uniform  light illumination;

const float boundaryLineWidth = 0.3;
//Number of transitions between color levels.
const int toonLevels = 4;
//Reciprocal of toonLevels  
const float toonScaleFactor = 1.0 / toonLevels;
/* The standard lambertian factor is calcolated using the cosine function and provides value
between 0 and 1. I'm going to replace the cosine wave with several stairs, where every stair 
is the size of this reciprocal 

|      --|
|    --  |
|  --    |
|--      |
4 Color levels 
*/

in vec3 vNormal;
in vec3 viewDir;
in vec3 vlightDir;
in vec2 vUVCoord;
in vec3 vColor;
in vec3 skyVUVCoord;
in float vHeight;
out vec4 fragColor;

subroutine(light)
vec4 SkyBoxFrag()
{
    return texture(skybox, skyVUVCoord);
}


subroutine(light)
vec4 illuminationForTerrain()
{
    //Texture
    vec3 initialColor;
    float biomeValue = texture(BiomeMap, vUVCoord).r  ;   
    biomeValue = clamp(biomeValue, -3.0f, 3.0f);
    /*I subdivide the biome space in 4 sub-space, each for every biomes. When the biomeValue is near 
    to boundary line between two biomes, the two colors of the two biomes will be interpolated
        firstLimit
    --|-----||-----|--
     LAWN        FOREST
         SecondLimit
    --|-----||-----|--
    FOREST       SNOW
         ThirdLimit
    --|-----||-----|--
     SNOW        MOUNTAIN
     
        1�     2�     3�     4�        SPACES
     ------|------|------|------
    */
    float firstBiomeLimit = -1.0;
    float secondBiomeLimit = 0.0;
    float thirdBiomeLimit =  1.5;
    if(biomeValue <= secondBiomeLimit - boundaryLineWidth)
    {
        vec3 color1 = texture(Lawn, vUVCoord * 300.0).rgb;
        vec3 color2 = texture(Forest, vUVCoord * 300.0).rgb;
        float minLimit = firstBiomeLimit - boundaryLineWidth; 
        float maxLimit = firstBiomeLimit + boundaryLineWidth; 
       
        float interpValue = (biomeValue - minLimit) / (maxLimit - minLimit);
        interpValue = smoothstep(0.0, 1.0, interpValue);
        initialColor = mix(color1, color2, interpValue);
    }
    else if(biomeValue <= thirdBiomeLimit - boundaryLineWidth)
    {
        vec3 color1 = texture(Forest, vUVCoord * 300.0).rgb;
        vec3 color2 = vec3(0.9, 0.9, 0.9);
        float minLimit = secondBiomeLimit - boundaryLineWidth; 
        float maxLimit = secondBiomeLimit + boundaryLineWidth; 
        float interpValue = (biomeValue - minLimit) / (maxLimit - minLimit);
        interpValue = smoothstep(0.0, 1.0, interpValue);
        initialColor = mix(color1, color2, interpValue);
    }
    else
    {
        vec3 color1 = vec3(0.9, 0.9, 0.9);
        vec3 color2 = texture(Mountain, vUVCoord * 300.0).rgb;
        float minLimit = thirdBiomeLimit - boundaryLineWidth; 
        float maxLimit = thirdBiomeLimit + boundaryLineWidth; 
        float interpValue = (biomeValue - minLimit) / (maxLimit - minLimit);
        interpValue = smoothstep(0.0, 1.0, interpValue);
        initialColor = mix(color1, color2, interpValue);
    }
    
//LIGHT COMPUTATION 
    float specAngle = 0.0;
    float specular = 0.0;
	vec3 N = normalize(vNormal);
	vec3 L = normalize(vlightDir);
	// ambient component can be calculated at the beginning
    vec3 color = Ka*initialColor;
    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);
    // if the lambert coefficient is positive, then I can calculate the specular component
    if(lambertian > 0.0)
    {
        // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
        vec3 V = normalize(viewDir);

        // in the Blinn-Phong model we do not use the reflection vector, but the half vector
        vec3 H = normalize(L + V);
        if(toonShadingIsEnabled)
        {
            /*We multiply the lambertian factor with the toonLevels, this provides a fraction
            between zero and number of toonLevels. We use the 'ceil' function in order to obtain 
            the nearest integer greater or equal to 'lambertian * toonLevels'. Then, we multiply the 
            result with 'toonScaleFactor' in order to get one stair between 0 and 1*/
            lambertian = ceil(lambertian * toonLevels) * toonScaleFactor;
        }
        else
        {
            // we use H to calculate the specular component
            specAngle = max(dot(H, N), 0.0);
            // shininess application to the specular component
            specular = pow(specAngle, shininess);
        }
        // We add diffusive and specular components to the final color
        color += lightIntensity * vec3( Kd * lambertian * initialColor +
                        Ks * specular * specularColor);
    }
    return vec4(color, 1.0);
}


subroutine(light)
vec4 illuminationForModels()
{
//TREE COLOR
    vec3 initialColor = vColor;
   
//LIGHT COMPUTATION 
    float specAngle = 0.0;
    float specular = 0.0;
	vec3 N = normalize(vNormal);
	vec3 L = normalize(vlightDir);
	// ambient component can be calculated at the beginning
    vec3 color = Ka*initialColor;
    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);
    // if the lambert coefficient is positive, then I can calculate the specular component
    if(lambertian > 0.0)
    {
        // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
        vec3 V = normalize(viewDir);

        // in the Blinn-Phong model we do not use the reflection vector, but the half vector
        vec3 H = normalize(L + V);
        if(toonShadingIsEnabled)
        {
            lambertian = ceil(lambertian * toonLevels) * toonScaleFactor;
        }
        else
        {
            // we use H to calculate the specular component
            specAngle = max(dot(H, N), 0.0);
            // shininess application to the specular component
            specular = pow(specAngle, shininess);
        }
        // We add diffusive and specular components to the final color
        // N.B. ): in this implementation, the sum of the components can be different than 1
        color += lightIntensity * vec3( Kd * lambertian * initialColor +
                        Ks * specular * specularColor);
    }
    return vec4(color, 1.0);
}


void main()
{
	fragColor = illumination();
}
