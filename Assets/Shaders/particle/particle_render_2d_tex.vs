#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 3) in vec2 iPos;
layout (location = 4) in vec2 iVel;
layout (location = 5) in float iLife;
layout (location = 6) in float iSeed;

uniform float u_aspectRatio;
uniform float u_sizeMin;
uniform float u_sizeMax;

out vec2 vUV;
out vec2 vScreenUV;
out float vAlpha;

void main() {
    float speed = length(iVel);
    float sizeBase = mix(u_sizeMin, u_sizeMax, clamp(speed * 2.0, 0.0, 1.0));
    vec2 scale = vec2(sizeBase);
    if (speed > 0.01) {
        scale.y *= (1.0 + speed * 4.0);
        scale.x *= (1.0 - clamp(speed * 0.5, 0.0, 0.4));
    }

    vec2 vertexPos = iPos + (aPos * scale * vec2(1.0, u_aspectRatio));
    gl_Position = vec4(vertexPos, 0.0, 1.0);

    vScreenUV = vertexPos * 0.5 + 0.5;

    vUV = aUV;
    vAlpha = min(iLife, 1.0);
}