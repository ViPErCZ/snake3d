#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D fireTexture;

void main() {
    // Získáme tvar částice z textury (použijeme červený kanál, protože je černobílá)
    float mask = texture(fireTexture, TexCoords).r;

    // Vynásobíme barvu částice její maskou (tvarem z textury)
    FragColor = vec4(ParticleColor.rgb, ParticleColor.a * mask);

    if (FragColor.a < 0.01) {
        discard;
    }
}