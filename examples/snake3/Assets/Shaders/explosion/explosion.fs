#version 450

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

in vec4 vColor;
in vec3 fragNormal;
in vec3 fragPos;
in vec2 TexCoords;

#include "../functions/lights.glsl"

uniform bool hasFallbackColor = false;
uniform vec3 fallbackColor = vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos;
// D1.1c: directionLightEnable migrated to MaterialData UBO. The gate is
// material_directionLightEnable (ShaderMaterial::bind sets it to 1 when
// directionalLight is wired in, otherwise 0).

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 baseColor = hasFallbackColor ? fallbackColor : vColor.rgb;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 final = baseColor;

    if (material_directionLightEnable != 0) {
        // D1.1c-fix: dirLight fields jsou v MaterialData UBO (per-materiál).
        DirLight dl;
        dl.direction = material_dirLight_direction;
        dl.ambient   = material_dirLight_ambient;
        dl.diffuse   = material_dirLight_diffuse;
        dl.specular  = material_dirLight_specular;
        final = CalcDirLight(dl, normal, viewDir, baseColor, 0.0);
    }

    vec3 lightAlbedo = texture(material.ambient, TexCoords).rgb;
    vec3 lightSpecular = texture(material.specular, TexCoords).rgb;

    for (int i = 0; i < numPointLights; i++) {
        final += CalcPointLight(pointLight[i], normal, fragPos, viewDir, baseColor, lightAlbedo, lightSpecular);
    }

    for (int i = 0; i < numSpotLights; i++) {
        final += CalcSpotLight(spotLight[i], normal, fragPos, viewDir, baseColor, 0.0, lightAlbedo, lightSpecular);
    }

    FragColor = vec4(pow(final, vec3(1.0 / 2.2)), vColor.a);
    gColor = FragColor;
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
