uniform vec4 clipPlane = vec4(0, 0, 1, 1.01);
uniform sampler2D reflectionTexture;
uniform bool reflectionEnable = false;

vec3 calcReflexion(vec4 spacePos, vec3 color);

vec3 calcReflexion(vec4 spacePos, vec3 color) {
    // Screen-space souřadnice pro odraz
    vec2 ndc = (spacePos.xy / spacePos.w) / 2.0 + 0.5;
    // Had jede na odraze na opacnou stranu -> prevratime X
    ndc.x = 1.0 - ndc.x;
    vec4 reflectionColor = texture(reflectionTexture, ndc);

    float reflectionStrength = 0.7; // Sníženo, aby více vyniklo albedo

    // Zkombinujeme barvu s odrazem
    vec3 result = mix(color, reflectionColor.rgb, reflectionStrength);
    result = result * vec3(1.05, 1.05, 1.05) + vec3(0.05);

    return result;
}