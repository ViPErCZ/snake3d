#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inVel;
layout (location = 2) in float inLife;
layout (location = 3) in float inSeed;

out vec2 outPos;
out vec2 outVel;
out float outLife;
out float outSeed;

uniform float u_dt;
uniform float u_time;

// Config
uniform vec2 u_emitterPos;
uniform vec2 u_emitterSize;
uniform vec2 u_gravity;
uniform vec2 u_velMin;
uniform vec2 u_velMax;
uniform float u_drag;
uniform float u_turbulence;
uniform float u_lifeMin;
uniform float u_lifeMax;
uniform int u_spawnMode;

// Pomocná funkce pro náhodu
float rand(float n){ return fract(sin(n) * 43758.5453123); }

void main() {
    float newLife = inLife - u_dt;
    vec2 newPos = inPos;
    vec2 newVel = inVel;
    float newSeed = inSeed;

    // 2. Kontrola smrti (vypršel čas) nebo vypadnutí z obrazovky (y < -1.2)
    if (newLife <= 0.0 || newPos.y < -1.2) {

        // --- RESPAWN LOGIKA ---

        // Generujeme novou náhodu
        float seedBase = u_time + inSeed + gl_VertexID * 0.1;
        float rnd1 = rand(seedBase);
        float rnd2 = rand(seedBase + 1.0);
        float rnd3 = rand(seedBase + 2.0);

        // Reset života
        newLife = mix(u_lifeMin, u_lifeMax, rnd3);

        // Reset pozice
        if (u_spawnMode == 1) {
            // Spawn nahoře (déšť přicházející shora)
            newPos.x = (rnd1 * 2.0 - 1.0) * (u_emitterSize.x / 2.0) + u_emitterPos.x;
            newPos.y = 1.1;
        }
        else if (u_spawnMode == 2) {
            // Celá obrazovka (pro počáteční naplnění nebo statický šum)
            newPos.x = (rnd1 * 2.0 - 1.0) * (u_emitterSize.x / 2.0) + u_emitterPos.x;
            newPos.y = (rnd2 * 2.0 - 1.0) * (u_emitterSize.y / 2.0) + u_emitterPos.y;
        }
        else {
            // Bod (emitování z bodu)
            newPos = u_emitterPos;
        }

        // Reset rychlosti
        newVel = mix(u_velMin, u_velMax, rnd2);

        // Refresh seedu pro další cyklus
        newSeed = rnd1 * 100.0;
    }
    else {
        // --- FYZIKA POHYBU ---
        newVel += u_gravity * u_dt;
        newVel *= (1.0 - min(u_dt * u_drag, 1.0));
        float currentSpeed = length(newVel);
        float slideChance = rand(u_time * 10.0 + inSeed);

        if (currentSpeed < 0.1 && slideChance < (u_turbulence * u_dt * 10.0)) {
            newVel.y -= 0.8; // Impuls dolů
        }

        newPos += newVel * u_dt;
    }

    outPos = newPos;
    outVel = newVel;
    outLife = newLife;
    outSeed = newSeed;
}