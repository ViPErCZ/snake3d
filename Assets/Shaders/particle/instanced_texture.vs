#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTex;

layout(location = 4) in vec3 iPos;
layout(location = 5) in vec3 iVel;
layout(location = 6) in float iLife;
layout(location = 7) in float iSeed;

out vec2 vTex;
out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float u_lifeMax;
uniform float u_sizeMin;
uniform float u_sizeMax;
uniform vec4  u_colorStart;
uniform vec4  u_colorEnd;
uniform float u_stretch;
uniform int   u_mode; // 0 = Billboard (Fire/Smoke), 1 = Stretched (Rain)

void main() {
    vTex = aTex;

    float t = clamp(iLife / max(u_lifeMax, 0.0001), 0.0, 1.0);
    float s;
    if (u_mode == 1) {
       // Vytvoříme pseudo-náhodnou velikost mezi sizeMin a sizeMax založenou na seedu
       float variation = fract(sin(iSeed) * 43758.5453);
       s = mix(u_sizeMin, u_sizeMax, variation);
    } else {
       // Pro oheň necháme postupné zmenšování/zvětšování podle života
       s = mix(u_sizeMin, u_sizeMax, t);
    }
    vColor = mix(u_colorEnd, u_colorStart, t);

    // Extrakce vektorů kamery z view matice
    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 offset;

    if (u_mode == 1) {
        // --- REŽIM DÉŠŤ (Protažení podle rychlosti) ---
        vec3 stretchDir = normalize(iVel);
        float speed = length(iVel);

        // aPos.x je šířka (do stran od kamery)
        // aPos.z je délka (ve směru pádu)
        offset = (camRight * aPos.x * s) +
                 (stretchDir * aPos.z * (s + speed * u_stretch));
    }
    else {
        // --- REŽIM OHEŇ/KOUŘ (Klasický billboard) ---
        // Použijeme iPos.y pro případné mírné protažení ohně nahoru,
        // ale standardně se točí za kamerou (aPos.x a aPos.z tvoří quad)
        float speed = length(iVel);
        float sx = s;
        float sy = s + speed * u_stretch; // Oheň se může natahovat "vzhůru"

        offset = (camRight * aPos.x * sx) + (camUp * aPos.z * sy);
    }

    // Výpočet výsledné pozice
    vec4 worldPos;
    if (u_mode == 1) {
        // Rain je v World Space, ignorujeme model matrix (nebo použijeme Identity)
        worldPos = vec4(iPos + offset, 1.0);
    } else {
        // Oheň/Kouř jsou lokální k objektu
        worldPos = model * vec4(iPos + offset, 1.0);
    }

    gl_Position = projection * view * worldPos;
}