#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;

uniform vec3 viewPos;

#include "pipeline/fog/fog.glsl"

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);

    if (fogEnable) {
        float d = distance(viewPos, fragPos);
        float alpha = getFogFactor(d);
        FragColor = mix(FragColor, vec4(0.6f, 0.6f, 0.7f, 1.f), alpha);
    }
}