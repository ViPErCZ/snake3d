#version 450

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}