#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aColor;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

#include "functions/expansion.glsl"

void main()
{
    TexCoords = aTexCoords;
    vec2 expanded = compute_2d_expansion(aPos.xy);
    gl_Position = projection * model * vec4(expanded.x, expanded.y, 0.0, 1.0);
}