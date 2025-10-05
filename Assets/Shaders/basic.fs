#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 BrightColor;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 meshColor;
in vec3 TangentLightPos;
in vec3 TangentFragPos;
in vec3 TangentViewPos;
in vec3 Normal;
in vec3 worldNormal;
in vec3 camPos;
in mat3 TBN;
in mat4 viewMatrix;

uniform vec3 viewPos;
uniform vec3 ambientLightColor = vec3(1.0, 1.0, 1.0);
uniform float ambientLightColorIntensity = 1.0;
uniform bool directionLightEnable = false;
uniform bool shadowsEnable = false;
uniform bool pbrEnabled = false;
uniform bool overrideColorMesh = false;
uniform sampler2D metalness;
uniform sampler2D roughness;

#include "functions/fog.glsl"
#include "functions/lights.glsl"
#include "functions/shadows.glsl"
#include "functions/alpha.glsl"

void main()
{
    float shadow = 0.0;
    vec3 ambientColor = ambientLightColor * ambientLightColorIntensity;
    vec4 albedoTexture = useMaterial ? vec4(meshColor, 1.0) : texture(material.ambient, TexCoords);

    if (overrideColorMesh && useMaterial) {
        albedoTexture = vec4(ambientLightColor, 1.0);
    }

    float metalness = pbrEnabled ? texture(metalness, TexCoords).r : 0.0;
    float roughness = pbrEnabled ? texture(roughness, TexCoords).r : 0.5;

    vec3 F0 = vec3(0.04);
    F0 = useMaterial ? mix(F0, pow(albedoTexture.xyz, vec3(2.2)), metalness) : mix(F0, pow(albedoTexture.rgb, vec3(2.2)), metalness);

    vec3 normal = Normal;
    if (normalMapEnabled) {
       vec3 tangentNormal = texture(material.diffuse, TexCoords).xyz;
       tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] -> [-1,1]
       normal = tangentNormal;
    }

    vec3 color = useMaterial ? albedoTexture.xyz : albedoTexture.rgb;
    vec3 ambient = useMaterial ? color : ambientColor * color;
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 final = ambient;

    if (directionLightEnable) {
        if (pbrEnabled) {
            final += CalcDirLightPBR(dirLight, fragPos, normal, viewDir, ambientColor, roughness, metalness, F0);
        } else {

            if (shadowsEnable) {
                vec4 fragPosView = viewMatrix * vec4(fragPos, 1.0);
                float viewDepth = -fragPosView.z;
                int cascadeIndex = int(GetCascadeIndex(viewDepth));
                vec3 shadowNormal = normalMapEnabled ? Normal : worldNormal;

                if (cascadeIndex == 0) {
                    shadow = ShadowCalculation2(fragPos, shadowNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix0);
//                     shadow = ShadowCalculation(fragPos, cascadeIndex, lightSpaceMatrix0);
                } else if (cascadeIndex == 1)
                    shadow = ShadowCalculation2(fragPos, shadowNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix1);
                else
                    shadow = ShadowCalculation2(fragPos, shadowNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix2);
        //         float shadow = ShadowCalculation(fragPos, shadowMap0, lightSpaceMatrix0);
            }

            final = CalcDirLight(dirLight, normal, viewDir, ambient, shadow);
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

//     if (shadowsEnable) {
//         vec4 fragPosView = viewMatrix * vec4(fragPos, 1.0);
//         float viewDepth = -fragPosView.z;
//         int cascadeIndex = int(GetCascadeIndex(viewDepth));
//
//         if(cascadeIndex == 0)
//             shadow = ShadowCalculation2(fragPos, worldNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix0);
//         else if(cascadeIndex == 1)
//             shadow = ShadowCalculation2(fragPos, worldNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix1);
//         else
//             shadow = ShadowCalculation2(fragPos, worldNormal, -dirLight.direction, cascadeIndex, lightSpaceMatrix2);
// //         float shadow = ShadowCalculation(fragPos, shadowMap0, lightSpaceMatrix0);
//
//
//         vec3 colors[3] = vec3[3](vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
//         vec3 debugColor = vec3(viewDepth, 0, 0);
//
//         final = final * pow(1.0 - shadow, 1.2);
//     }

    if (pbrEnabled == false) {
        final /= 1;
    }

    if (fogEnable) {
       float d = distance(viewPos, fragPos);
       float alpha = getFogFactor(d);
       FragColor = mix(vec4(final, 1.0), vec4(0.6f, 0.6f, 0.7f, 0.9f), alpha);
    } else {
       FragColor = alphaBlending(pow(final, vec3(1.0/2.2)));
    }

    gColor = FragColor;

    // Pro lepší kontrolu můžeme říct, že zářit mají jen opravdu jasné části
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) { // Práh jasu pro bloom
       BrightColor = FragColor;
    } else {
       BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}