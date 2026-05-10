#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 textColor;

#include "functions/alpha.glsl"

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, TexCoords).r);
    FragColor = alphaBlending(textColor) * sampled;
}