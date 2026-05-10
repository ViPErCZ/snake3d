#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D textureMap;
uniform vec3 color;
uniform bool useMaterial = false;

#include "functions/alpha.glsl"

void main()
{
    if (useMaterial) {
        FragColor = alphaBlending(color);
    } else {
        FragColor = alphaBlending(texture(textureMap, TexCoords).rgb);
    }
}