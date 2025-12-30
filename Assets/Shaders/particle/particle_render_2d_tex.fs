#version 330 core

in vec2 vUV;
in vec2 vScreenUV;
in float vAlpha;

out vec4 FragColor;

uniform sampler2D uSceneTexture;
uniform sampler2D uNormalTexture;

void main() {
    vec4 normalData = texture(uNormalTexture, vUV);
    float shapeAlpha = normalData.a;

    if (shapeAlpha < 0.05) discard;

    vec3 normal = normalize(normalData.rgb * 2.0 - 1.0);
    normal.y = -normal.y;
    float refractionStrength = 0.04;
    vec2 offset = normal.xy * refractionStrength;
    offset.y *= -1.0;
    vec2 coords = vScreenUV + offset;
    coords = clamp(coords, 0.005, 0.995);
    vec3 sceneColor = texture(uSceneTexture, coords).rgb;

    // -------------------------------------------------------------------
    // 3. ODLESKY A STÍNOVÁNÍ
    // -------------------------------------------------------------------

    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 lightDir = normalize(vec3(0.5, 0.8, 0.5));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    float fresnel = 1.0 - dot(normal, viewDir);
    float edgeDarkening = smoothstep(0.6, 1.0, fresnel);

    vec3 finalRGB = sceneColor;
    finalRGB *= (1.0 - edgeDarkening * 0.5);
    finalRGB += vec3(1.0) * spec * 1.5;

    float finalAlpha = smoothstep(0.0, 0.2, shapeAlpha) * vAlpha;

    FragColor = vec4(finalRGB, finalAlpha);
}