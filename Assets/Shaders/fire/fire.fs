#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec4 ParticleColor; // Přichází z VS, může mít hodnoty > 1.0 (HDR)

uniform sampler2D fire_texture; // Ujisti se, že název uniformu sedí

void main() {
    float mask = texture(fire_texture, TexCoords).r;
    float proceduralMask = 1.0 - smoothstep(0.45, 0.5, length(TexCoords - vec2(0.5)));
    float finalMask = mask * proceduralMask;

    // Finální barva částice s HDR hodnotami
    vec4 finalColor = vec4(ParticleColor.rgb, ParticleColor.a * finalMask);

    if (finalColor.a < 0.01) {
        discard;
    }

    // Pro lepší kontrolu můžeme říct, že zářit mají jen opravdu jasné části
    float brightness = dot(finalColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) { // Práh jasu pro bloom
        BrightColor = finalColor;
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); // Pokud není dost jasná, do bloomu neposílá nic
    }

    // Do finální scény vykreslíme barvu vždy
    gColor = finalColor;
}