#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 3) in vec3 iPos;
layout (location = 4) in vec3 iVel;
layout (location = 5) in float iLife;
layout (location = 6) in float iSeed;

uniform float u_aspectRatio;
uniform float u_sizeMin;
uniform float u_sizeMax;
uniform float u_lifeMax;

out vec2 vUV;
out vec2 vScreenUV;
out vec2 vVelocity;
out float vAlpha;

void main() {
    float speed = length(iVel);
    float sizeBase = mix(u_sizeMin, u_sizeMax, clamp(speed * 3.0, 0.0, 1.0));

    float headBoost = 0.7;
    vec2 scale = vec2(sizeBase);
    vec2 offsetPos = vec2(0.0);

    if (aPos.y < 0.0) {
        scale.y *= 4;
        offsetPos.y += 0.003;
    } else {
        scale *= headBoost;
    }

    vec2 vertexPos = iPos.xy + ((aPos + offsetPos) * scale * vec2(1.0, u_aspectRatio));
    gl_Position = vec4(vertexPos, 0.0, 1.0);

    vScreenUV = vertexPos * 0.5 + 0.5;
    vVelocity = iVel.xy;

    vUV = aUV;
    float lifeNorm = clamp(iLife / u_lifeMax, 0.0, 1.0);
    vAlpha = smoothstep(0.0, 0.2, lifeNorm);
}