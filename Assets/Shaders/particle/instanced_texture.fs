#version 330 core

in vec2 vTex;
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture0;
uniform int u_mode; // 0 = Oheň/Kouř, 1 = Déšť

void main() {
    vec4 tex = texture(uTexture0, vTex);
    
    float alpha;
    vec3 color;

    if (u_mode == 1) {
        // --- REŽIM DÉŠŤ ---
        // Použijeme standardní barvy bez extrémního zesílení.
        // vColor.a pro déšť by mělo být kolem 0.4 - 0.6
        alpha = tex.a * vColor.a;
        color = tex.rgb * vColor.rgb * 20;
    } else {
        // --- REŽIM OHEŇ/HDR ---
        // Tady necháme tvé původní hodnoty pro záření (Bloom)
        alpha = tex.a * vColor.a * 2.8;
        color = tex.rgb * vColor.rgb;
    }

    FragColor = vec4(color, alpha);
}