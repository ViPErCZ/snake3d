#version 330 core

layout(location = 0) in vec3 aPos;            // z PlaneMesh: XZ quad
// Pozor: UV u StandardMesh jsou na layoutu 3 (viz Mesh::initialize)
layout(location = 3) in vec2 aTex;

// Instanced stav (z TF ping‑pong VBO)
layout(location = 4) in vec3 iPos;
layout(location = 5) in vec3 iVel;
layout(location = 6) in float iLife;
layout(location = 7) in float iSeed;

out vec2 vTex;
out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

// Sdílené parametry jako v TF
uniform float u_lifeMin;
uniform float u_lifeMax;
uniform float u_sizeMin;
uniform float u_sizeMax;
uniform vec4  u_colorStart;
uniform vec4  u_colorEnd;
uniform float u_stretch; // natažení billboardu podél osy Up podle rychlosti

void main() {
    vTex = aTex;

    // Stejný výpočet t, velikosti a barvy jako v TF (vizuální shoda)
    float t = clamp(iLife / max(u_lifeMax, 0.0001), 0.0, 1.0);
    float s = mix(u_sizeMin, u_sizeMax, t);
    float vlen = length(iVel);
    float sx = s;
    float sy = s + vlen * u_stretch;
    vColor = mix(u_colorEnd, u_colorStart, t);

    // Camera-facing billboard: Right/Up z view matice
    vec2 quad = vec2(aPos.x, aPos.z);
    mat3 camRot = transpose(mat3(view));
    vec3 right = camRot[0];
    vec3 up    = camRot[1];

    vec3 worldPos = iPos + right * (quad.x * sx) + up * (quad.y * sy);
    gl_Position = projection * view * model * vec4(worldPos, 1.0);
}
