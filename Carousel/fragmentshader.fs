#version 330

uniform vec3 LightColor1;
uniform vec3 viewPos;

in vec3 fragpos;
in vec3 normal;
in vec3 lightpos;
in vec3 fragcol;

out vec4 color;

void main()
{
	float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * LightColor1;
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightpos - fragpos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor1;
    
    float specularStrength = 0.5f;
    
    vec3 viewDir = normalize(viewPos - fragpos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * LightColor1;
    
    vec3 result = (ambient + diffuse + specular) * fragcol;
    color = vec4(result, 1.0f);
}
