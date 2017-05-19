#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec3 LightPosition1;
uniform vec3 LightPosition2;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out vec3 fragpos;
out vec3 normal;
out vec3 lightpos;
out vec3 lightpos2;
out vec3 fragcol;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0f);
    fragpos = vec3(ViewMatrix * ModelMatrix * vec4(Position, 1.0f));
    normal = mat3(transpose(inverse(ViewMatrix * ModelMatrix))) * Normal; 
    lightpos = vec3(ViewMatrix * vec4(LightPosition1, 1.0));
    lightpos2 = vec3(ViewMatrix * vec4(LightPosition2, 1.0));
    fragcol = Color;
}
