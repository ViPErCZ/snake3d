vec3 spawnRing(float seed, float minR, float maxR) {
    float a = rand(seed) * 6.2831853;
    float r = sqrt(mix(minR * minR, maxR * maxR, rand(seed * 1.37)));
    return vec3(cos(a) * r, sin(a) * r, 0.0);
}

OutputData particle_update_3d(vec3 inPos, vec3 inVel, float inLife, float inSeed) {
    OutputData data;
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

    // SPECIÁLNÍ LOGIKA PRO ČEKÁNÍ EXPLOZE
    if (u_spawnShape == 2 && respawn) {
        float cycleTime = mod(u_timeAccum, u_burstInterval);
        if (cycleTime > u_spawnWindow) {
            outPos = u_emitterPos; outVel = vec3(0.0); outLife = -1.0; outSeed = inSeed;
            data.outPos = inPos;
            data.outVel = inVel;
            data.outLife = inLife;
            data.outSeed = inSeed;

            return data;
        }
    }

    // 2. FYZIKA
    if (!respawn) {
        if (u_turbulence.x > 0.0) {
            float sway = sin(u_timeAccum * u_turbulence.y + inSeed) * u_turbulence.x;
            pos.x += sway * u_dt;
            pos.y += cos(u_timeAccum * (u_turbulence.y * 0.8) + inSeed) * (u_turbulence.x * 0.5) * u_dt;
        }

        if (u_spawnShape == 2) vel *= pow(0.8, u_dt);

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
        else if (u_spawnShape == 2) {
            float theta = r0 * 6.2831853;
            float phi = r1 * 3.14159 * 0.6;
            float sinPhi = sin(phi);
            vec3 dir = normalize(vec3(sinPhi * cos(theta), sinPhi * sin(theta), cos(phi)));
            pos = u_emitterPos + dir * (u_emitterRadius * r2);
            vel = dir * mix(u_velMin.x, u_velMax.x, r2);
            life = mix(u_lifeMin, u_lifeMax, r0);
        }
        else {
            float ang = r0 * 6.2831853;
            float rad = sqrt(r1);
            vec2 disk = vec2(cos(ang), sin(ang)) * rad;
            pos = u_emitterPos + vec3(disk.x * u_emitterRadius, disk.y * u_emitterRadius, u_emitterYOffset);
            vel = mix(u_velMin, u_velMax, vec3(r0, r1, r2));
            life = mix(u_lifeMin, u_lifeMax, r2);
        }
    }

    data.outPos = pos;
    data.outVel = vel;
    data.outLife = life;
    data.outSeed = inSeed;

    return data;
}