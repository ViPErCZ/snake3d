#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;
layout(location = 2) in float inLife;
layout(location = 3) in float inSeed;

// UNIFORM BUFFER
layout (std140) uniform ParticleParams {
    vec4 u_lifeSizeStretch;    // x=minLife, y=maxLife, z=minSize, w=maxSize
    vec4 u_velMinStretch;      // xyz=velMin, w=stretch
    vec4 u_velMaxDrag;         // xyz=velMax, w=drag
    vec4 u_gravity;            // xyz=gravity, w=colorSensitivity
    vec4 u_emitterPosShape;    // xyz=pos, w=spawnShape (0,1,2)
    vec4 u_emitterSizeRadius;  // xy=size, z=radius, w=yOffset
    vec4 u_spawnArea;          // x=minRad, y=maxRad, z=height, w=spawnPerFrame
    vec4 u_turbulenceTime;     // xy=turb, z=timeOffset, w=respawnMode
    vec4 u_colorStart;
    vec4 u_colorEnd;
};

uniform float u_dt;
uniform float u_timeAccum;
uniform float u_burstInterval = 3.0;
uniform float u_spawnWindow = 0.1;
uniform bool u_is2D;

#define u_lifeMin       u_lifeSizeStretch.x
#define u_lifeMax       u_lifeSizeStretch.y
#define u_minRadius     u_spawnArea.x
#define u_maxRadius     u_spawnArea.y
#define u_spawnHeight   u_spawnArea.z
#define u_spawnPerFrame u_spawnArea.w

#define u_velMin        u_velMinStretch.xyz
#define u_velMax        u_velMaxDrag.xyz
#define u_drag          u_velMaxDrag.w
#define u_gravityVec3   u_gravity.xyz

#define u_emitterPos    u_emitterPosShape.xyz
#define u_emitterSize   u_emitterSizeRadius.xy
#define u_emitterRadius u_emitterSizeRadius.z
#define u_emitterYOffset u_emitterSizeRadius.w

#define u_turbulence    u_turbulenceTime.xy
#define u_spawnShape    int(u_emitterPosShape.w + 0.1)
#define u_respawnMode   int(u_turbulenceTime.w + 0.1)

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
