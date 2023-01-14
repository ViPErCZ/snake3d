#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

#include "pipeline/blending/alpha.glsl"

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = alphaBlending(vec3(1, 1, 1)) * sampled;
}