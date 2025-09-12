#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 vPos;
out vec3 vNormal;
out vec3 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vPos = vec3(model * vec4(aPos, 1.0));   // světové souřadnice
    vNormal = mat3(transpose(inverse(model))) * aNormal; // transformovaná normála
    fColor = aColor;
}