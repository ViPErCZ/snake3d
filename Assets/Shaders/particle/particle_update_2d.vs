#version 330 core

// VSTUPY (z minulého framu - Locations 0, 1, 2, 3)
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inVel;
layout (location = 2) in float inLife;
layout (location = 3) in float inSeed;

// VÝSTUPY (do dalšího framu - Transform Feedback)
// Tyto názvy musí sedět s tím, co máš v poli `varyings` v C++
out vec2 outPos;
out vec2 outVel;
out float outLife;
out float outSeed;

// PARAMETRY
uniform float u_dt;
uniform float u_time;
uniform float u_seed_iter; // Random offset (např. frameIndex / 1000.0)

// Config
uniform vec2 u_emitterPos;
uniform vec2 u_emitterSize; // x = width, y = height
uniform vec2 u_gravity;
uniform vec2 u_velMin;
uniform vec2 u_velMax;
uniform float u_drag;       // Tření (vysoké pro kapky na skle)
uniform float u_turbulence; // Šance na "uklouznutí"
uniform float u_lifeMin;
uniform float u_lifeMax;
uniform int u_spawnMode;    // 0 = Bod, 1 = Obdélník (vršek), 2 = Celá plocha

// Pomocná funkce pro náhodu
float rand(float n){ return fract(sin(n) * 43758.5453123); }

void main() {
    // 1. Zestárnutí
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

        // A) Gravitace
        newVel += u_gravity * u_dt;

        // B) Odpor (Drag) - Klíčové pro efekt skla!
        // Kapka se snaží zastavit o sklo.
        // Pokud je u_drag např. 2.0, kapka rychle zpomalí.
        newVel *= (1.0 - min(u_dt * u_drag, 1.0));

        // C) Turbulence (Uklouznutí)
        // Občas kapka dostane impuls a sjede dolů (překoná povrchové napětí)
        float currentSpeed = length(newVel);
        // Generujeme náhodu pro tento frame
        float slideChance = rand(u_time * 10.0 + inSeed);

        // Pokud kapka skoro stojí A padne "šestka", tak ji postrčíme
        if (currentSpeed < 0.1 && slideChance < (u_turbulence * u_dt * 10.0)) {
            newVel.y -= 0.8; // Impuls dolů
        }

        // D) Aplikace rychlosti
        newPos += newVel * u_dt;
    }

    // Zápis do Transform Feedback bufferu
    outPos = newPos;
    outVel = newVel;
    outLife = newLife;
    outSeed = newSeed;
}