#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

in vec2 vQuad;

uniform vec3 color;
uniform float pulse;

void main() {
    float dist = length(vQuad);

    float outer = 1.0;
    float inner = 0.8;

    if (dist > outer || dist < inner) discard;

    float edgeAlpha = smoothstep(outer, outer - 0.05, dist) * smoothstep(inner, inner + 0.05, dist);

    float alpha = 0.8 * (0.5 + 0.5 * pulse) * edgeAlpha;

    gColor = vec4(color, alpha);
    BrightColor = vec4(color * alpha, 1.0);
}