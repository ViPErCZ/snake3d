#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textureMap;
uniform vec3 faceColor;
uniform vec3 glowColor;
uniform vec3 glowColor2;
uniform vec3 depthColor;
uniform vec2 depthOffset;
uniform float time;

#include "functions/alpha.glsl"

void main()
{
    vec2 warpUV = TexCoords;
    float heat = sin(TexCoords.y * 18.0 + time * 2.1) * 0.5 + 0.5;
    float shimmer = sin(TexCoords.x * 10.0 - time * 1.7) * 0.5 + 0.5;
    float warp = (heat * shimmer) * 0.004;
    warpUV += vec2(warp, -warp * 0.6);

    vec4 texSample = texture(textureMap, warpUV);
    float a = texSample.a;

    vec2 texel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
    float edgeMask = 0.0;
    edgeMask += texture(textureMap, TexCoords + vec2(texel.x, 0.0)).a;
    edgeMask += texture(textureMap, TexCoords - vec2(texel.x, 0.0)).a;
    edgeMask += texture(textureMap, TexCoords + vec2(0.0, texel.y)).a;
    edgeMask += texture(textureMap, TexCoords - vec2(0.0, texel.y)).a;
    edgeMask += texture(textureMap, TexCoords + texel).a;
    edgeMask += texture(textureMap, TexCoords - texel).a;
    edgeMask = clamp(edgeMask * 0.8, 0.0, 1.0);

    float depthAlpha = 0.0;
    for (int i = 1; i <= 4; ++i) {
        vec2 offset = depthOffset * float(i);
        depthAlpha = max(depthAlpha, texture(textureMap, TexCoords - offset).a);
    }
    float depthOnly = clamp(depthAlpha - a, 0.0, 1.0) * 0.35;

    float gradient = smoothstep(0.0, 1.0, TexCoords.y);
    vec3 face = mix(faceColor * 0.75, faceColor * 1.35, gradient);

    float t = time * 0.9;
    float n1 = sin(TexCoords.x * 22.0 + t * 2.3 + sin(TexCoords.y * 9.0 - t * 1.4));
    float n2 = sin(TexCoords.x * 38.0 - t * 3.1 + cos(TexCoords.y * 17.0 + t * 1.9));
    float noise = (n1 * 0.6 + n2 * 0.4) * 0.5 + 0.5;
    float flameBand = smoothstep(0.35, 0.98, noise + (1.0 - TexCoords.y) * 0.35);
    float flicker = 0.65 + 0.35 * sin(time * 5.3 + TexCoords.x * 40.0);

    float edgeOutline = clamp(edgeMask - a, 0.0, 1.0);
    float edge = smoothstep(0.05, 0.6, edgeOutline);
    float glow = edge * (2.4 + flameBand * 2.9) * flicker;

    vec3 faceTex = texSample.rgb * faceColor;
    vec3 color = depthColor * depthOnly + faceTex * (a * 1.35);

    float outer = smoothstep(0.05, 0.5, depthAlpha) * (1.0 - a);
    float glowBoost = glow + outer * 1.1;

    vec3 glowBase = glowColor * (glowBoost * 2.4);
    vec3 glowRim = glowColor2 * (glowBoost * 1.4 + edge * 0.55);
    vec3 flame = vec3(2.5, 1.05, 0.12) * (flameBand * 1.0 * edge);
    vec3 rim = vec3(1.8, 0.7, 0.08) * (edge * 0.75);

    color += glowBase + glowRim + flame + rim;
    color = clamp(color, 0.0, 1.1);

    float outAlpha = clamp(a + depthOnly * 0.35 + glow * 0.45, 0.0, 1.0);
    if (outAlpha <= 0.01)
        discard;

    FragColor = alphaBlending(color) * vec4(1.0, 1.0, 1.0, outAlpha);
}
