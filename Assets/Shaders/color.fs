#version 330 core

in vec2 vQuad;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;
out vec4 FragColor;

uniform vec3 color;
uniform float pulse;

void main() {
    float dist = length(vQuad); // 0 = střed, ~1 = okraj

    float outer = 1.0;
    float inner = 0.9; // tenká páska mezi inner a outer

    if (dist < inner || dist > outer) discard; // jen páska

    // pulzování průhlednosti
    float alpha = 1.0 * (0.5 + 0.5 * pulse); // můžeš zmenšit základní opačnost
    vec3 finalColor = color * (0.8 + 0.2 * pulse);
    BrightColor = gColor = FragColor = vec4(finalColor, alpha);
}