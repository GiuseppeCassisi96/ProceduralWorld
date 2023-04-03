#version 460 core

// ambient, diffusive and specular components (passed from the application)
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
// weight of the components
uniform float Ka;
uniform float Kd;
uniform float Ks;
//Alpha component of specular
uniform float shininess;
//Intensity of light
uniform float lightIntensity;
uniform sampler2D ElevationMap;
uniform sampler2D BiomeMap;
uniform sampler2D Grass;
uniform sampler2D Desert;
uniform sampler2D Snow;
uniform sampler2D Rock;


in vec3 vNormal;
in vec3 viewDir;
in vec3 vlightDir;
in vec2 vUVCoord;
out vec4 fragColor;

void main()
{
//Texture
    vec3 initialColor;
    float heightValue = texture(ElevationMap, vUVCoord).r;    
    float biomeValue = texture(BiomeMap, vUVCoord).r;    
    heightValue = clamp(heightValue, -2.0f, 2.0f);
    biomeValue = clamp(biomeValue, -2.0f, 2.0f);
    if (heightValue <= 0.5f)
    {
        if (biomeValue < -1.0f)
        {
            initialColor = texture(Desert, vUVCoord * 100).rgb;
        }
        else if (biomeValue < 1.0f)
        {
            initialColor = texture(Grass, vUVCoord * 100).rgb;
        }
        else if (biomeValue <= 2.0f)
        {
            initialColor = texture(Snow, vUVCoord * 100).rgb;
        }
    }
    else
    {
        if (biomeValue < 0.0f)
        {
            initialColor = texture(Snow, vUVCoord * 100).rgb;
        }
        if (biomeValue <= 2.0f)
        {
            initialColor = texture(Rock, vUVCoord * 100).rgb;
        }
    }
    

//LIGHT COMPUTATION 
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

        // we use H to calculate the specular component
        float specAngle = max(dot(H, N), 0.0);
        // shininess application to the specular component
        float specular = pow(specAngle, shininess);

        // We add diffusive and specular components to the final color
        // N.B. ): in this implementation, the sum of the components can be different than 1
        color += lightIntensity * vec3( Kd * lambertian * initialColor +
                        Ks * specular * specularColor);
    }
	fragColor = vec4(color, 1.0);
}
