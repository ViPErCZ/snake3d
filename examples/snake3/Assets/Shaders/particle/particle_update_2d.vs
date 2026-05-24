OutputData particle_update_2d(vec3 inPos, vec3 inVel, float inLife, float inSeed) {
    OutputData data;
    vec2 pos = inPos.xy;
    vec2 vel = inVel.xy;
    float newSeed = inSeed;
    float newLife = inLife - u_dt;

    // 2. Kontrola smrti (vypršel čas) nebo vypadnutí z obrazovky (y < -1.2)
    if (newLife <= 0.0 || pos.y < -1.2) {

        // Pokud je respawn vypnuty nebo neni spawn zadany, zhasni castici.
        if (u_respawnMode == 0 && u_spawnPerFrame <= 0.0) {
            newLife = 0.0;
            pos = vec2(-2.0, -2.0);
            vel = vec2(0.0);
            data.outPos = vec3(pos.xy, 0);
            data.outVel = vec3(vel.xy, 0);
            data.outLife = newLife;
            data.outSeed = newSeed;
            return data;
        }

        // --- RESPAWN LOGIKA ---

        // Generujeme novou náhodu
        float seedBase = u_timeAccum + inSeed + gl_VertexID * 0.1;
        float rnd1 = rand(seedBase);
        float rnd2 = rand(seedBase + 1.0);
        float rnd3 = rand(seedBase + 2.0);

        // Spawn rate gate: pouze cast z nich se znovu zrodí
        float spawnChance = u_spawnPerFrame * u_dt;
        if (u_respawnMode == 0 && (u_spawnPerFrame <= 0.0 || rand(seedBase + 3.0) > spawnChance)) {
            newLife = 0.0;
            pos = vec2(-2.0, -2.0);
            vel = vec2(0.0);
            data.outPos = vec3(pos.xy, 0);
            data.outVel = vec3(vel.xy, 0);
            data.outLife = newLife;
            data.outSeed = newSeed;
            return data;
        }

        // Reset života
        newLife = mix(u_lifeMin, u_lifeMax, rnd3);

        // Reset pozice
        if (u_respawnMode == 1) {
            // Spawn nahoře (déšť přicházející shora)
            pos.x = (rnd1 * 2.0 - 1.0) * (u_emitterSize.x / 2.0) + u_emitterPos.x;
            pos.y = 1.1;
        }
        else if (u_respawnMode == 2) {
            // Celá obrazovka (pro počáteční naplnění nebo statický šum)
            pos.x = (rnd1 * 2.0 - 1.0) * (u_emitterSize.x / 2.0) + u_emitterPos.x;
            pos.y = (rnd2 * 2.0 - 1.0) * (u_emitterSize.y / 2.0) + u_emitterPos.y;
        }
        else {
            // Bod (emitování z bodu)
            float ang = rnd1 * 6.2831853;
            float rad = sqrt(rnd2);
            vec2 disk = vec2(cos(ang), sin(ang)) * rad * u_emitterRadius;
            pos = u_emitterPos.xy + disk;
        }

        // Reset rychlosti
        vel = mix(u_velMin.xy, u_velMax.xy, rnd2);

        // Refresh seedu pro další cyklus
        newSeed = rnd1 * 100.0;
    }
    else {
        // --- FYZIKA POHYBU ---
        vel += u_gravityVec3.xy * u_dt;
        vel *= (1.0 - min(u_dt * u_drag, 1.0));
        float currentSpeed = length(vel);
        float slideChance = rand(u_timeAccum * 10.0 + inSeed);

        if (currentSpeed < 0.1 && slideChance < (u_turbulence.x * u_dt * 10.0)) {
            vel.y -= 0.8;// Impuls dolů
        }

        pos += vel * u_dt;
    }

    data.outPos = vec3(pos.xy, 0);
    data.outVel = vec3(vel.xy, 0);
    data.outLife = newLife;
    data.outSeed = newSeed;

    return data;
}
