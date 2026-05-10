#version 330 core

// Atributy modelu (Quad 1x1)
layout (location = 0) in vec2 aPos; // (-0.5 až 0.5)
layout (location = 1) in vec2 aUV;  // (0.0 až 1.0) - pokud nemáš v meshi, spočítáme z aPos

// INSTANCED atributy (z TF bufferu) - musí sedět s glVertexAttribPointer v render metodě
layout (location = 3) in vec2 iPos;
layout (location = 4) in vec2 iVel;
layout (location = 5) in float iLife;
layout (location = 6) in float iSeed;

uniform float u_aspectRatio; // Width / Height
uniform float u_sizeMin;
uniform float u_sizeMax;

out vec2 vUV;
out float vAlpha;

void main() {
    // 1. Určení velikosti podle rychlosti (rychlejší = větší/delší)
    float speed = length(iVel);
    float sizeBase = mix(u_sizeMin, u_sizeMax, clamp(speed * 2.0, 0.0, 1.0));

    // 2. Stretch efekt (protažení ve směru pohybu)
    vec2 scale = vec2(sizeBase);

    // Pokud se hýbe, protáhneme ji ve směru Y (jednoduchá verze)
    // Pro komplexnější rotaci bys musel použít matici rotace podle velocity vectoru
    if (speed > 0.05) {
        scale.y *= (1.0 + speed * 1.5);
        scale.x *= (1.0 - speed * 0.1);
    }

    // 3. Výpočet pozice
    // aPos * scale * aspectCorrection -> aby kruh byl kruh i na širokoúhlém monitoru
    vec2 vertexPos = iPos + (aPos * scale * vec2(1.0, u_aspectRatio));

    gl_Position = vec4(vertexPos, 0.0, 1.0);

    // UV předáme dál (nebo vypočítáme aPos + 0.5)
    vUV = aPos + 0.5;

    // Fade in/out na začátku a konci života
    float fade = min(iLife, 1.0); // Jednoduchý fade out na konci
    vAlpha = fade;
}