#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec3 baseColor;
uniform vec3 glowColor;
uniform vec3 glowColor2;
uniform float time;
#include "functions/alpha.glsl"

void main()
{
    float a = texture(textTexture, TexCoords).r;
    if (a <= 0.01)
        discard;

    vec2 texel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
    float glowMask = 0.0;
    glowMask += texture(textTexture, TexCoords + vec2(texel.x, 0.0)).r;
    glowMask += texture(textTexture, TexCoords - vec2(texel.x, 0.0)).r;
    glowMask += texture(textTexture, TexCoords + vec2(0.0, texel.y)).r;
    glowMask += texture(textTexture, TexCoords - vec2(0.0, texel.y)).r;
    glowMask += texture(textTexture, TexCoords + vec2(texel.x, texel.y)).r;
    glowMask += texture(textTexture, TexCoords + vec2(-texel.x, texel.y)).r;
    glowMask += texture(textTexture, TexCoords + vec2(texel.x, -texel.y)).r;
    glowMask += texture(textTexture, TexCoords + vec2(-texel.x, -texel.y)).r;
    glowMask = clamp(glowMask * 0.6, 0.0, 1.0);

    float gradient = smoothstep(0.0, 1.0, TexCoords.y);
    vec3 gradColor = mix(baseColor * 0.45, baseColor * 1.45 + glowColor * 0.35, gradient);

    float t = time * 0.9;
    float n1 = sin(TexCoords.x * 18.0 + t * 2.4 + sin(TexCoords.y * 8.0 - t * 1.3));
    float n2 = sin(TexCoords.x * 34.0 - t * 3.1 + cos(TexCoords.y * 18.0 + t * 1.9));
    float noise = (n1 * 0.6 + n2 * 0.4) * 0.5 + 0.5;

    float flameBand = smoothstep(0.35, 0.98, noise + (1.0 - TexCoords.y) * 0.35);
    float flicker = 0.72 + 0.28 * sin(time * 5.2 + TexCoords.x * 40.0);

    float edge = smoothstep(0.08, 0.85, glowMask);
    float glow = edge * (1.05 + flameBand * 1.1) * flicker;

    vec3 color = gradColor;
    color = mix(color, glowColor2, glow * 0.95);
    color += glowColor * (glow * 1.45 + glowMask * 0.45);
    color += vec3(2.0, 1.05, 0.25) * (flameBand * 0.45 * edge);
    color += vec3(1.2, 0.7, 0.18) * (edge * 0.35);
    color = clamp(color, 0.0, 3.5);

    FragColor = alphaBlending(color) * vec4(1.0, 1.0, 1.0, a);
}
