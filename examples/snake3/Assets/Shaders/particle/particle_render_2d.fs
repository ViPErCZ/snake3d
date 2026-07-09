#version 330 core

in vec2 vUV;
in float vAlpha;

out vec4 FragColor;

uniform vec4 u_colorStart;
uniform vec4 u_colorEnd;

void main() {
    // Vzdálenost od středu (0.5, 0.5) -> transform na (-1..1)
    vec2 center = vUV * 2.0 - 1.0;
    float dist = length(center);

    // Oříznutí do kruhu
    if (dist > 1.0) discard;

    // --- PROCEDURÁLNÍ VZHLED KAPKY VODY ---

    // 1. Normála (vypouklá čočka)
    float z = sqrt(1.0 - dist * dist);
    vec3 normal = vec3(center.x, center.y, z);

    // 2. Světlo (zprava nahoře)
    vec3 lightDir = normalize(vec3(0.5, 0.5, 1.0));

    // 3. Specular (odlesk) - malá ostrá tečka
    float spec = pow(max(dot(normal, lightDir), 0.0), 40.0);

    // 4. Rim (okraj) - kapka je na okrajích trochu tmavší/výraznější
    float rim = smoothstep(0.8, 1.0, dist);

    // 5. Míchání barev
    vec4 baseColor = u_colorStart;

    // Voda je průhledná uprostřed, více viditelná na okrajích + odlesk
    float finalAlpha = baseColor.a * (0.1 + rim * 0.5) * vAlpha;

    vec3 finalRGB = baseColor.rgb;
    finalRGB += vec3(1.0) * spec; // Přičtení bílého odlesku

    FragColor = vec4(finalRGB, finalAlpha);
}