#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D smoke_texture; // Použij správný název uniformu

void main() {
    // Maska z textury
    float textureMask = texture(smoke_texture, TexCoords).r;

    // Procedurální maska, která zaručí měkký kruhový okraj
    float proceduralMask = 1.0 - smoothstep(0.45, 0.5, length(TexCoords - vec2(0.5)));

    // Zkombinujeme obě masky - získáme detail textury A měkký okraj
    float finalMask = textureMask * proceduralMask;

    FragColor = vec4(ParticleColor.rgb, ParticleColor.a * finalMask);

    if (FragColor.a < 0.01) {
        discard;
    }
}