#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec3 LightPosition1;
uniform vec3 LightColor1;
uniform float DiffuseFactor;
uniform float SpecularFactor;
uniform float AmbientFactor;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out vec4 vColor;

void main()
{
	mat4 normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
	vec3 normal = normalize((normalMatrix * vec4(normalize(Normal), 1.0)).xyz);

	vec3 lightPosition1 = (ViewMatrix * vec4(LightPosition1, 1.0)).xyz;
	vec4 vertexPositionModelSpace = (ViewMatrix * ModelMatrix) * vec4(Position,1.0);
	vec3 vertexNormalized = normalize(-vertexPositionModelSpace.xyz);
	
	vec3 lightVector1 = normalize(lightPosition1 - vertexPositionModelSpace.xyz);
	vec3 halfVector1 = normalize(lightVector1 + vertexNormalized); 
	
	vec3 diffusePart = (clamp(dot(normal,lightVector1), 0.0, 1.0)*LightColor1);
	vec3 specularPart = (pow(clamp(dot(normal,halfVector1),0.0,1.0),127.0 )*LightColor1);
	vec3 ambientPart = vec3(Color * AmbientFactor);
	diffusePart *= vec3(DiffuseFactor);
	specularPart *= vec3(SpecularFactor);
	
	vColor = vec4(Color*diffusePart + specularPart + ambientPart, 1.0);
	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);
}
