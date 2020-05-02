#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec4 fragPosLightSpace;
out vec3 normal;

void main()
{
    FragPos = aPos;
    fragPosLightSpace = lightSpaceMatrix * vec4(aPos, 1.0);
    normal = aNormal;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
