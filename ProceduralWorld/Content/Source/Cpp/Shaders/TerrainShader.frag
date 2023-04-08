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
// Subroutine Uniform (it is conceptually similar to a C pointer function)
subroutine uniform  light illumination;



in vec3 vNormal;
in vec3 viewDir;
in vec3 vlightDir;
in vec2 vUVCoord;
in float vHeight;
in vec3 vColor;
in vec3 skyVUVCoord;
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
    float biomeValue = texture(BiomeMap, vUVCoord).r;    
    biomeValue = clamp(biomeValue, -2.0f, 2.0f);
    if (vHeight <= 2.0f)
    {
        if (biomeValue < -1.0f)
        {
            initialColor = texture(Forest, vUVCoord * 100).rgb;
        }
        else if (biomeValue < 1.0f)
        {
           initialColor = texture(Lawn, vUVCoord * 100).rgb;
        }
        else if (biomeValue <= 2.0f)
        {
            initialColor = vec3(0.9, 0.9, 0.9);
        }
    }
    else
    {
        if (biomeValue < 0.0f)
        {
            initialColor = vec3(0.9, 0.9, 0.9);
        }
        else if (biomeValue <= 2.0f)
        {
            initialColor = texture(Mountain, vUVCoord * 250).rgb;
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
    return vec4(color, 1.0);
}


subroutine(light)
vec4 illuminationForModels()
{
//texture
    vec3 initialColor = vColor;
   
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
    return vec4(color, 1.0);
}


void main()
{
	fragColor = illumination();
}
