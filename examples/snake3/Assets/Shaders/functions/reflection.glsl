uniform sampler2D reflectionTexture;

vec3 calcReflexion(vec4 spacePos, vec3 color);

vec3 calcReflexion(vec4 spacePos, vec3 color) {
    // Screen-space souřadnice pro odraz
    vec2 ndc = (spacePos.xy / spacePos.w) / 2.0 + 0.5;
    ndc.x = 1.0 - ndc.x;
    vec4 reflectionColor = texture(reflectionTexture, ndc);

    float reflectionStrength = 0.5; // Sníženo, aby více vyniklo albedo

    // Zkombinujeme barvu s odrazem
    vec3 result = mix(color, reflectionColor.rgb, reflectionStrength);

    return result * vec3(1.05, 1.05, 1.05) + vec3(0.05);
}