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
in vec4 clipSpacePos;
in vec2 outUvScale;

uniform float uTime = 1;
uniform vec3 viewPos;
// D1.2c: ambientLightColor / ambientLightColorIntensity / overrideColorMesh
// migrated to MaterialData UBO (read as material_*). Legacy declarations
// removed -- AlbedoFeature now writes the UBO shadow instead of calling
// setUniform on these names.
uniform bool directionLightEnable = false;
uniform bool shadowsEnable = false;
// D1.2d.4: pbrEnabled migrated to MaterialData UBO (material_pbrEnabled).
uniform sampler2D metalness;
uniform sampler2D roughnessMap;

// D1.2d.8: rainDropEnable / rainSpeed / rainDensity migrated to MaterialData
// UBO (material_rainDropEnable / material_rainSpeed / material_rainDensity).

// Hole map: greyscale grid (e.g. 48x48) where r > 0.5 marks a "hole" -
// the fragment is discarded so the player can see through the surface.
// D1.2d.5: hasHoleMap migrated to MaterialData UBO (material_hasHoleMap).
// The holeMap sampler stays legacy.
uniform sampler2D holeMap;

#include "functions/material_data.glsl"
#include "functions/fog_material.glsl"
#include "functions/lights.glsl"
#include "functions/reflection.glsl"
#include "functions/shadows.glsl"
#include "functions/alpha_material.glsl"
#include "functions/rainRipple.glsl"

void main()
{
    // @MATERIAL_FRAGMENT_PRE
    // ^ Snippet injection slot - ShaderPreprocessor::applySnippets nahradí
    //   tento řádek obsahem všech snippetů, které features dodaly pro tento
    //   marker (C2a: víc features se může injektovat na stejné místo, kód se
    //   concatenuje v pořadí registrace v MaterialBuilder).
    //   Aktuální consumer: HoleMapFeature - hole_map_discard.glsl.

    float shadow = 0.0;
    vec3 ambientColor = material_ambientLightColor * material_ambientLightColorIntensity;
    vec4 albedoTexture = (material_useMaterial != 0) ? vec4(meshColor, 1.0) : texture(material.ambient, TexCoords);

    if ((material_overrideColorMesh != 0) && (material_useMaterial != 0)) {
        albedoTexture = vec4(material_ambientLightColor, 1.0);
    }

    float metalness = (material_pbrEnabled != 0) ? texture(metalness, TexCoords).b : 0.0;
    float roughness = (material_pbrEnabled != 0) ? texture(roughnessMap, TexCoords).g : 0.5;
    vec3 F0 = vec3(0.04);

#ifdef FEATURE_PBR
    if (material_pbrEnabled != 0) {
        roughness = clamp(roughness, 0.05, 1.0);
        metalness = clamp(metalness, 0.0, 1.0);
        F0 = (material_useMaterial != 0) ? mix(F0, pow(albedoTexture.xyz, vec3(2.2)), metalness) : mix(F0, pow(albedoTexture.rgb, vec3(2.2)), metalness);
    }
#endif

    // rippleOffset declaration stays unconditional - the planar reflection
    // snippet references it. Without FEATURE_RAIN_RIPPLE it just stays zero
    // and no distortion is applied anywhere.
    vec2 rippleOffset = vec2(0.0);
#ifdef FEATURE_RAIN_RIPPLE
    if (material_rainDropEnable != 0) {
        vec2 cleanUV = TexCoords / outUvScale * 2;
        rippleOffset = getRainRippleDistortion(cleanUV, uTime, material_rainSpeed, material_rainDensity);
    }
#endif

    vec3 normal = Normal;
#ifdef FEATURE_NORMAL_MAP
    if (material_normalMapEnabled != 0) {
        vec3 tangentNormal = texture(material.diffuse, TexCoords).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] -> [-1,1]

    #ifdef FEATURE_RAIN_RIPPLE
        if (material_rainDropEnable != 0) {
            // Modifikujeme normálovou mapu před převodem do World Space
            tangentNormal.xy += rippleOffset * 2.0;
            tangentNormal = normalize(tangentNormal);
        }
    #endif

        normal = TBN * tangentNormal;
    }
#endif
#ifdef FEATURE_RAIN_RIPPLE
    if ((material_normalMapEnabled == 0) && (material_rainDropEnable != 0)) {
        // Pokud není normal mapa, vytvoříme normálu jen z vlnek
        normal = normalize(TBN * vec3(rippleOffset, 1.0));
    }
#endif

    vec3 color = (material_useMaterial != 0) ? albedoTexture.xyz : albedoTexture.rgb;
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 ambient = vec3(0.0);

#ifdef FEATURE_PBR
    if (material_pbrEnabled != 0) {
        // A) PBR Ambient (IBL)
        vec3 kS = fresnelSchlick(max(dot(normal, viewDir), 0.0), F0);
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metalness;

        vec3 irradiance = vec3(0.03);
        vec3 reflections = vec3(0.0);

    #ifdef FEATURE_IBL
        if (iblEnabled) {
            irradiance = CalcIBLDiffuse(normal);
            vec3 R = reflect(-viewDir, normal);
            reflections = CalcIBLSpecular(R, roughness, F0) * kS;
        }
    #endif
        if (!iblEnabled) {
            irradiance = material_ambientLightColor * 0.1;
        }

        vec3 diffusePart = irradiance * albedoTexture.rgb;

        // Ambient Occlusion
        float ao = texture(material.aoMap, TexCoords).r;
        if (ao < 0.01) ao = 1.0;

        // Výsledný ambient scény
        ambient = (kD * diffusePart + reflections) * ao * material_ambientLightColorIntensity;
    }
#endif
    if (material_pbrEnabled == 0) {
        vec3 color = (material_useMaterial != 0) ? albedoTexture.xyz : albedoTexture.rgb;
        ambient = (material_useMaterial != 0) ? color : material_ambientLightColor * material_ambientLightColorIntensity * color;
    }

    vec3 final = ambient;

#ifdef FEATURE_DIRECTIONAL_LIGHT
    if (directionLightEnable) {
    #ifdef FEATURE_PBR
        if (material_pbrEnabled != 0) {
            final += CalcDirLightPBR(dirLight, normal, fragPos, viewDir, ambient, roughness, metalness, F0);
        }
    #endif
        if (material_pbrEnabled == 0) {
    #ifdef FEATURE_SHADOWS
            if (shadowsEnable) {
                vec4 fragPosView = viewMatrix * vec4(fragPos, 1.0);
                float viewDepth = -fragPosView.z;
                vec3 shadowNormal = (material_normalMapEnabled != 0) ? Normal : worldNormal;
                shadow = ShadowBlended(fragPos, shadowNormal, -dirLight.direction, viewDepth);
            }
    #endif
            final = CalcDirLight(dirLight, normal, viewDir, ambient, shadow);
        }
    }
#endif

    vec3 lightAlbedo = vec3(texture(material.ambient, TexCoords));
    vec3 lightSpecular = vec3(texture(material.specular, TexCoords));

#ifdef FEATURE_PBR
    if (material_pbrEnabled != 0) {
        for(int i = 0; i < numPointLights; i++)
        {
            final += CalcPointLightPBR(pointLight[i], normal, fragPos, viewDir,
                                 ambient, roughness, metalness, F0);
        }
    }
#endif
    if (material_pbrEnabled == 0) {
        for(int i = 0; i < numPointLights; i++)
        {
            final += CalcPointLight(pointLight[i], normal, fragPos, viewDir, ambient, lightAlbedo, lightSpecular);
        }
    }

#ifdef FEATURE_PBR
    if (material_pbrEnabled != 0) {
        for(int i = 0; i < numSpotLights; i++)
        {
            final += CalcSpotLightPBR(spotLight[i], normal, fragPos, viewDir,
                                      uTime, ambient, roughness, metalness, F0);
        }
    }
#endif
    if (material_pbrEnabled == 0) {
        for(int i = 0; i < numSpotLights; i++)
        {
            final += CalcSpotLight(spotLight[i], normalize(Normal), fragPos, viewDir, ambient, uTime, lightAlbedo, lightSpecular);
        }
    }

#ifdef FEATURE_FOG
    if (material_fogEnable != 0) {
       float d = distance(viewPos, fragPos);
       float alpha = getFogFactor(d);
       FragColor = mix(vec4(final, 1.0), vec4(0.6f, 0.6f, 0.7f, 0.9f), alpha);
    }
#endif
    if (material_fogEnable == 0) {
       FragColor = alphaBlending(pow(final, vec3(1.0/2.2)));
    }

    // @MATERIAL_FRAGMENT_POST
    // ^ Snippet injection slot for post-shading effects (planar reflection,
    //   future: tonemap tweaks, color grading). PlanarReflectionFeature
    //   provides the reflection blob; without it the line stays a comment.

    gColor = FragColor;
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) {
       BrightColor = FragColor;
    } else {
       BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}