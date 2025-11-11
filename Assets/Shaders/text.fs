#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

#include "functions/alpha.glsl"

void main()
{
    float a = texture(textTexture, TexCoords).r;
    FragColor = vec4(textColor * a, a);
}