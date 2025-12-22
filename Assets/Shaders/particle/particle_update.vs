#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;
layout(location = 2) in float inLife;
layout(location = 3) in float inSeed;

uniform float u_dt;
uniform vec3  u_emitterPos;
uniform float u_timeAccum;

uniform int   u_spawnShape;  // 0 = Local (Fire/Smoke), 1 = Environment (Snow/Rain)
uniform int   u_respawnMode; // 0 = Die (Fire), 1 = Infinite Wrap (Snow)

// --- (u_spawnShape = 1) ---
uniform vec2  u_turbulence;
uniform float u_minRadius;
uniform float u_maxRadius;
uniform float u_spawnHeight;

// --- (u_spawnShape = 0) ---
uniform float u_emitterRadius;
uniform float u_emitterRadiusX;
uniform float u_emitterRadiusZ;
uniform float u_emitterYOffset;

// --- base physic ---
uniform float u_lifeMin;
uniform float u_lifeMax;
uniform vec3  u_velMin;
uniform vec3  u_velMax;
uniform vec3  u_gravity;

out vec3 outPos;
out vec3 outVel;
out float outLife;
out float outSeed;

float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

vec3 spawnRing(float seed, float minR, float maxR) {
    float a = rand(seed) * 6.2831853;
    float r = sqrt(mix(minR * minR, maxR * maxR, rand(seed * 1.37)));
    return vec3(cos(a) * r, sin(a) * r, 0.0);
}

void main() {
    vec3 pos = inPos;
    vec3 vel = inVel;
    float life = inLife;
    bool respawn = false;

    if (life <= 0.0) {
        respawn = true;
    }

    if (length(vel) < 0.001 && length(u_gravity) > 0.001) {
        respawn = true;
    }

    if (!respawn) {

        if (u_turbulence.x > 0.0) {
            float sway = sin(u_timeAccum * u_turbulence.y + inSeed) * u_turbulence.x;
            pos.x += sway * u_dt;
            pos.y += cos(u_timeAccum * (u_turbulence.y * 0.8) + inSeed) * (u_turbulence.x * 0.5) * u_dt;
        }

        vel += u_gravity * u_dt;
        pos += vel * u_dt;

        if (u_respawnMode == 1) {
            float floorLevel = u_emitterPos.z - u_spawnHeight;
            if (pos.z < floorLevel) {
                pos.z += u_spawnHeight * 1.5;
                vec3 offset = spawnRing(inSeed + u_timeAccum, 0.0, u_maxRadius * 0.2);
                pos.x += offset.x;
                pos.y += offset.y;
            }

            float dist = distance(pos.xy, u_emitterPos.xy);
            if (dist > u_maxRadius * 1.2 || (dist < u_minRadius && pos.z < u_emitterPos.z)) {
                respawn = true;
            }

            life = u_lifeMax;
        } else {
            life -= u_dt;
        }
    }

    // 3. SPAWN / RESPAWN
    if (respawn) {
        float r0 = rand(inSeed + u_timeAccum);
        float r1 = rand(inSeed * 1.45 + u_dt);
        float r2 = rand(inSeed * 2.11);

        if (u_spawnShape == 1) {
            vec3 ring = spawnRing(inSeed + u_timeAccum, u_minRadius, u_maxRadius);
            pos.x = u_emitterPos.x + ring.x;
            pos.y = u_emitterPos.y + ring.y;
            pos.z = u_emitterPos.z + mix(-u_spawnHeight * 0.5, u_spawnHeight, r2);

            vel = vec3(
                mix(u_velMin.x, u_velMax.x, r0),
                mix(u_velMin.y, u_velMax.y, r1),
                mix(u_velMin.z, u_velMax.z, r2)
            );
            life = u_lifeMax;
        }
        else {
            float ang = r0 * 6.2831853;
            float rad = sqrt(r1); // Normalized radius 0..1
            float rx = u_emitterRadiusX > 0.0 ? u_emitterRadiusX : u_emitterRadius;
            float rz = u_emitterRadiusZ > 0.0 ? u_emitterRadiusZ : u_emitterRadius;
            vec2 disk = vec2(cos(ang), sin(ang)) * rad;

            disk.x *= rx;
            disk.y *= rz;

            pos = u_emitterPos + vec3(disk.x, disk.y, u_emitterYOffset);
            vel = mix(u_velMin, u_velMax, vec3(r0, r1, r2));
            life = mix(u_lifeMin, u_lifeMax, r2);
        }
    }

    outPos = pos;
    outVel = vel;
    outLife = life;
    outSeed = inSeed;
    gl_Position = vec4(0.0);
}