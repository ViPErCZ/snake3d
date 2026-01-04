#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;
layout(location = 2) in float inLife;
layout(location = 3) in float inSeed;

// UNIFORM BUFFER (Material data z C++)
//layout (std140) uniform ParticleData {
//    vec4 u_emitterPos;      // xyz = center, w = spawnShape (0=Point, 1=Box, 2=Sphere)
//    vec4 u_emitterSize;     // xyz = rozměry (pro 2D nech z=0), w = unused
//    vec4 u_gravity;         // xyz = vector, w = drag
//    vec4 u_velRange;        // x=minSpd, y=maxSpd, z=turbulence, w=stickiness
//    vec4 u_lifeParams;      // x=minLife, y=maxLife
//    vec4 u_randoms;         // x=time, y=dt
//};

uniform float u_dt;
uniform vec3  u_emitterPos;
uniform float u_timeAccum;

uniform int   u_spawnShape;
uniform int   u_respawnMode;

uniform vec2  u_turbulence;
uniform float u_minRadius;
uniform float u_maxRadius;
uniform float u_spawnHeight;

uniform float u_emitterRadius;
uniform float u_emitterRadiusX;
uniform float u_emitterRadiusZ;
uniform float u_emitterYOffset;

uniform float u_lifeMin;
uniform float u_lifeMax;
uniform vec3  u_velMin;
uniform vec3  u_velMax;
uniform vec3  u_gravity;

uniform float u_burstInterval = 3.0;
uniform float u_spawnWindow = 0.1;

// 2D
uniform vec2 u_emitterSize;
uniform float u_drag;

uniform bool u_is2D;

out vec3 outPos;
out vec3 outVel;
out float outLife;
out float outSeed;

float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

struct OutputData {
    vec3 outPos;
    vec3 outVel;
    float outLife;
    float outSeed;
};

#include "particle_update_3d.vs"
#include "particle_update_2d.vs"

OutputData particle_update_3d(vec3 inPos, vec3 inVel, float inLife, float inSeed);
OutputData particle_update_2d(vec3 inPos, vec3 inVel, float inLife, float inSeed);

void main() {
    OutputData data;
    data.outPos = inPos;
    data.outVel = inVel;
    data.outLife = inLife;
    data.outSeed = inSeed;

    if (u_is2D) {
        data = particle_update_2d(inPos, inVel, inLife, inSeed);
    } else {
        data = particle_update_3d(inPos, inVel, inLife, inSeed);
    }

    outPos  = data.outPos;
    outVel  = data.outVel;
    outLife = data.outLife;
    outSeed = data.outSeed;

    gl_Position = vec4(0.0);
}