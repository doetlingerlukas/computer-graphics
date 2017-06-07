#version 330

uniform float AmbientFactor;
uniform float DiffuseFactor;
uniform float SpecularFactor;

uniform vec3 LightColor1;
uniform vec3 LightColor2;
uniform vec3 viewPos;

uniform sampler2D myTextureSampler;

in vec3 fragpos;
in vec3 normal;
in vec3 lightpos;
in vec3 lightpos2;
in vec3 fragcol;
in vec2 UVcoords;

out vec4 color;

void main()
{
	float ambientStrength = AmbientFactor;
    vec3 ambient = ambientStrength * LightColor1;
    vec3 ambient2 = ambientStrength * LightColor2;
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightpos - fragpos);
    vec3 lightDir2 = normalize(lightpos2 - fragpos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor1;
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * LightColor2;
    diffuse *= vec3(DiffuseFactor);
    diffuse2 *= vec3(DiffuseFactor);
    
    float specularStrength = SpecularFactor;
    
    vec3 viewDir = normalize(viewPos - fragpos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 64);
    vec3 specular = specularStrength * spec * LightColor1;
    vec3 specular2 = specularStrength * spec2 * LightColor2;
    
    vec3 result = (ambient + ambient2 + diffuse + diffuse2 + specular + specular2) * fragcol;
    color = texture2D(myTextureSampler, UVcoords) * vec4(result, 1.0f);
}
