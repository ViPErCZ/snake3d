#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gBloom;

in vec2 TexCoords;
in vec4 ParticleColor; // Barva kouře z VS

uniform sampler2D smoke_texture; // Ujisti se, že název uniformu sedí

void main() {
    float textureMask = texture(smoke_texture, TexCoords).r;
    float proceduralMask = 1.0 - smoothstep(0.45, 0.5, length(TexCoords - vec2(0.5)));
    float finalMask = textureMask * proceduralMask;

    vec4 finalColor = vec4(ParticleColor.rgb, ParticleColor.a * finalMask);

    if (finalColor.a < 0.01) {
        discard;
    }

    // Kouř je vidět ve finální scéně
    gColor = finalColor;
    // Kouř ale nezáří, takže do bloom bufferu pošleme černou
    gBloom = vec4(0.0, 0.0, 0.0, 1.0);
}