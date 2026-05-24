#version 330 core

in vec2 vUV;
in float vAlpha;

out vec4 FragColor;

uniform vec4 u_colorStart;
uniform vec4 u_colorEnd;

void main() {
    vec2 center = vUV * 2.0 - 1.0;
    float dist = length(center);

    // soft round dot
    float alpha = smoothstep(1.0, 0.0, dist);
    alpha = pow(alpha, 1.6);

    if (alpha < 0.01) discard;

    vec4 color = mix(u_colorEnd, u_colorStart, alpha);
    color.a *= alpha * vAlpha;

    FragColor = color;
}
