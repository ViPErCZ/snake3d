#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 TangentLightPos;
in vec3 TangentFragPos;
in vec3 TangentViewPos;
in vec3 Normal;
in vec3 camPos;
in mat3 TBN;

uniform vec3 viewPos;
uniform vec3 ambientLightColor = vec3(1.0, 1.0, 1.0);
uniform float ambientLightColorIntensity = 1.0;
uniform bool useOverrideColor;
uniform bool directionLightEnable = false;
uniform bool shadowsEnable = false;
uniform bool pbrEnabled = false;
uniform sampler2D metalness;
uniform sampler2D roughness;

#include "functions/fog.glsl"
#include "functions/lights.glsl"
#include "functions/shadows.glsl"
#include "functions/alpha.glsl"

void main()
{
    vec3 ambientColor = ambientLightColor * ambientLightColorIntensity;

    if (useMaterial) {
        FragColor = vec4(ambientColor, 0);
    } else {
        vec4 albedoTexture = texture(material.ambient, TexCoords);
        float metalness = pbrEnabled ? texture(metalness, TexCoords).r : 0.0;
        float roughness = pbrEnabled ? texture(roughness, TexCoords).r : 0.5;

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, pow(albedoTexture.rgb, vec3(2.2)), metalness);

        vec3 normal = Normal;
        if (normalMapEnabled) {
           vec3 tangentNormal = texture(material.diffuse, TexCoords).xyz;
           tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] -> [-1,1]
           normal = tangentNormal;
        }

        vec3 color = albedoTexture.rgb;
        vec3 ambient = ambientColor * color;
        vec3 viewDir = normalize(camPos - fragPos);
//         vec3 viewDir = normalize(camPos - fragPos);
        vec3 final = ambient;

        if (directionLightEnable) {
            if (pbrEnabled) {
                final += CalcDirLightPBR(dirLight, normal, viewDir, ambientColor, roughness, metalness, F0);
            } else {
                final = CalcDirLight(dirLight, normal, viewDir, ambientColor);
            }
        }

        for(int i = 0; i < numPointLights; i++)
        {
            if (pbrEnabled) {
                final += CalcPointLightPBR(pointLight[i], normal, fragPos, viewDir, roughness, metalness, F0);
            } else {
                final += CalcPointLight(pointLight[i], normal, fragPos, viewDir);
            }
        }

        for(int i = 0; i < numSpotLights; i++)
        {
           final += CalcSpotLight(spotLight[i], normalize(Normal), fragPos, viewDir);
        }

        if (shadowsEnable) {
           float shadow = ShadowCalculation(fragPos);
           final = final * (1.0 - shadow);
        }

        if (fogEnable) {
           float d = distance(viewPos, fragPos);
           float alpha = getFogFactor(d);
           FragColor = mix(vec4(final, 1.0), vec4(0.6f, 0.6f, 0.7f, 0.9f), alpha);
        } else {
           FragColor = alphaBlending(final);
//            FragColor = vec4(pow(final, vec3(1.0/2.2)), 1.0);
        }
    }

    gColor = FragColor;
    BrightColor = vec4(0.0);
}