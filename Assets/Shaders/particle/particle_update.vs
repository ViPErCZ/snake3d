#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;
layout(location = 2) in float inLife;
layout(location = 3) in float inSeed;

uniform float u_dt;
uniform vec3  u_emitterPos;
uniform float u_emitterYOffset = 0.0;
// Fire params (konfigurovatelné přes GPUParticle3D)
uniform float u_lifeMin = 1.2;
uniform float u_lifeMax = 1.8;
uniform float u_sizeMin = 0.02;
uniform float u_sizeMax = 0.08;
uniform vec3  u_velMin  = vec3(-0.1, 0.6, -0.1);
uniform vec3  u_velMax  = vec3( 0.1, 1.6,  0.1);
uniform vec3  u_gravity = vec3(0.0, 0.0, -0.4);
uniform float u_emitterRadius = 0.05; // zkratka pro X/Z, použitá pokud nejsou specifické radii
uniform float u_emitterRadiusX = 0.05;
uniform float u_emitterRadiusZ = 0.05;
uniform float u_spawnPerFrame = 1.0;
// Akumulovaný čas od startu (pro plynulý rozběh, může zůstat nevyužitý)
uniform float u_timeAccum = 0.0;

out vec3 outPos;
out vec3 outVel;
out float outLife;
out float outSeed;

float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

mat4 compose(vec3 pos, vec3 scale) {
    return mat4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        pos.x, pos.y, pos.z, 1
    );
}

void main() {
    vec3 pos = inPos;
    vec3 vel = inVel;
    float life = inLife;

    if (life <= 0.0) {
        float r0 = rand(inSeed + u_dt * 11.37);
        float r1 = rand(inSeed + u_dt);
        float r2 = rand(inSeed * 2.3);

        float ang = r0 * 6.2831853; // 2*pi
        // generuj jednotkový disk s rovnoměrným rozdělením (sqrt pro plochu)
        float rad = sqrt(r1);
        vec2 unitDisk = vec2(cos(ang), sin(ang)) * rad;
        // použij specifické poloměry v XZ, případně fallback na u_emitterRadius
        float rx = u_emitterRadiusX > 0.0 ? u_emitterRadiusX : u_emitterRadius;
        float rz = u_emitterRadiusZ > 0.0 ? u_emitterRadiusZ : u_emitterRadius;
        vec2 disk = vec2(unitDisk.x * rx, unitDisk.y * rz);
        pos = u_emitterPos + vec3(disk.x, disk.y, u_emitterYOffset);

        vec3 rv = mix(u_velMin, u_velMax, vec3(r1, r0, r2));
        vel = rv;

        life = mix(u_lifeMin, u_lifeMax, r2);
    } else {
        // jednoduchá integrace bez uložení stavu (stateless TF)
        // pro skutečný stav by bylo nutné ping-pong TF pro pos/vel/life
        vel += u_gravity * u_dt;
        pos += vel * u_dt;
        life -= u_dt;
    }

    float t = clamp(life / max(u_lifeMax, 0.0001), 0.0, 1.0);
    // základní izotropická škála
    float s = mix(u_sizeMin, u_sizeMax, t);
    // "stretched billboard" – výška proporcionalní rychlosti částice
    float vlen = length(vel);
    vec3 scale = vec3(s, s + vlen * 0.15, s);

    // výstup stavů pro ping-pong TF
    outPos = pos;
    outVel = vel;
    outLife = life;
    outSeed = inSeed;

    gl_Position = vec4(0.0); // TF VS neukazuje na obrazovku
}