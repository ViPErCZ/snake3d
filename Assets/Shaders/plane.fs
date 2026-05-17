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
uniform vec3 ambientLightColor = vec3(1.0, 1.0, 1.0);
uniform float ambientLightColorIntensity = 1.0;
uniform bool directionLightEnable = false;
uniform bool shadowsEnable = false;
uniform bool pbrEnabled = false;
uniform bool overrideColorMesh = false;
uniform sampler2D metalness;
uniform sampler2D roughnessMap;

uniform bool rainDropEnable = false;
uniform float rainSpeed = 0.2;
uniform float rainDensity = 20.0;

// Hole map: greyscale grid (e.g. 48x48) where r > 0.5 marks a "hole" - the
// fragment is discarded so the player can see through the surface.
uniform bool hasHoleMap = false;
uniform sampler2D holeMap;

#include "functions/fog.glsl"
#include "functions/lights.glsl"
#include "functions/reflection.glsl"
#include "functions/shadows.glsl"
#include "functions/alpha.glsl"
#include "functions/rainRipple.glsl"

void main()
{
    if (hasHoleMap) {
        // TexCoords is aTexCoords * uvScale, so dividing by uvScale gives the
        // raw 0..1 coords across the mesh. Plane UVs are flipped on Y relative
        // to the level row indexing, so invert v before sampling.
        vec2 cellUV = outUvScale.x > 0.0 && outUvScale.y > 0.0
            ? TexCoords / outUvScale
            : TexCoords;
        vec2 sampleUV = vec2(cellUV.x, 1.0 - cellUV.y);
        if (texture(holeMap, sampleUV).r > 0.5) {
            discard;
        }
    }

    float shadow = 0.0;
    vec3 ambientColor = ambientLightColor * ambientLightColorIntensity;
    vec4 albedoTexture = useMaterial ? vec4(meshColor, 1.0) : texture(material.ambient, TexCoords);

    if (overrideColorMesh && useMaterial) {
        albedoTexture = vec4(ambientLightColor, 1.0);
    }

    float metalness = pbrEnabled ? texture(metalness, TexCoords).b : 0.0;
    float roughness = pbrEnabled ? texture(roughnessMap, TexCoords).g : 0.5;
    vec3 F0 = vec3(0.04);

    if (pbrEnabled) {
        roughness = clamp(roughness, 0.05, 1.0);
        metalness = clamp(metalness, 0.0, 1.0);
        F0 = useMaterial ? mix(F0, pow(albedoTexture.xyz, vec3(2.2)), metalness) : mix(F0, pow(albedoTexture.rgb, vec3(2.2)), metalness);
    }

    vec2 rippleOffset = vec2(0.0);
    if (rainDropEnable) {
        vec2 cleanUV = TexCoords / outUvScale * 2;
        rippleOffset = getRainRippleDistortion(cleanUV, uTime, rainSpeed, rainDensity);
    }

    vec3 normal = Normal;
    if (normalMapEnabled) {
        vec3 tangentNormal = texture(material.diffuse, TexCoords).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] -> [-1,1]

        if (rainDropEnable) {
            // Modifikujeme normálovou mapu před převodem do World Space
            tangentNormal.xy += rippleOffset * 2.0;
            tangentNormal = normalize(tangentNormal);
        }

        normal = TBN * tangentNormal;
    } else if (rainDropEnable) {
        // Pokud není normal mapa, vytvoříme normálu jen z vlnek
        normal = normalize(TBN * vec3(rippleOffset, 1.0));
    }

    vec3 color = useMaterial ? albedoTexture.xyz : albedoTexture.rgb;
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 ambient = vec3(0.0);

    if (pbrEnabled) {
        // A) PBR Ambient (IBL)
        vec3 kS = fresnelSchlick(max(dot(normal, viewDir), 0.0), F0);
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metalness;

        vec3 irradiance = vec3(0.03);
        vec3 reflections = vec3(0.0);

        if (iblEnabled) {
            irradiance = CalcIBLDiffuse(normal);
            vec3 R = reflect(-viewDir, normal);
            reflections = CalcIBLSpecular(R, roughness, F0) * kS;
        } else {
             irradiance = ambientLightColor * 0.1;
        }

        vec3 diffusePart = irradiance * albedoTexture.rgb;

        // Ambient Occlusion
        float ao = texture(material.aoMap, TexCoords).r;
        if (ao < 0.01) ao = 1.0;

        // Výsledný ambient scény
        ambient = (kD * diffusePart + reflections) * ao * ambientLightColorIntensity;

    } else {
        vec3 color = useMaterial ? albedoTexture.xyz : albedoTexture.rgb;
        ambient = useMaterial ? color : ambientLightColor * ambientLightColorIntensity * color;
    }

    vec3 final = ambient;

    if (directionLightEnable) {
        if (pbrEnabled) {
            final += CalcDirLightPBR(dirLight, normal, fragPos, viewDir, ambient, roughness, metalness, F0);
        } else {
            if (shadowsEnable) {
                vec4 fragPosView = viewMatrix * vec4(fragPos, 1.0);
                float viewDepth = -fragPosView.z;
                vec3 shadowNormal = normalMapEnabled ? Normal : worldNormal;
                shadow = ShadowBlended(fragPos, shadowNormal, -dirLight.direction, viewDepth);
            }

            final = CalcDirLight(dirLight, normal, viewDir, ambient, shadow);
        }
    }

    vec3 lightAlbedo = vec3(texture(material.ambient, TexCoords));
    vec3 lightSpecular = vec3(texture(material.specular, TexCoords));

    if (pbrEnabled) {
        for(int i = 0; i < numPointLights; i++)
        {
            final += CalcPointLightPBR(pointLight[i], normal, fragPos, viewDir,
                                 ambient, roughness, metalness, F0);
        }
    } else {
        for(int i = 0; i < numPointLights; i++)
        {
            final += CalcPointLight(pointLight[i], normal, fragPos, viewDir, ambient, lightAlbedo, lightSpecular);
        }
    }

    for(int i = 0; i < numSpotLights; i++)
    {
       final += CalcSpotLight(spotLight[i], normalize(Normal), fragPos, viewDir, ambient, uTime, lightAlbedo, lightSpecular);
    }

    if (fogEnable) {
       float d = distance(viewPos, fragPos);
       float alpha = getFogFactor(d);
       FragColor = mix(vec4(final, 1.0), vec4(0.6f, 0.6f, 0.7f, 0.9f), alpha);
    } else {
       FragColor = alphaBlending(pow(final, vec3(1.0/2.2)));
    }

    if (reflectionEnable) {
        vec4 distortedClipPos = clipSpacePos;
        if (rainDropEnable) {
            // Planární reflexe se rozvlní
            distortedClipPos.xy += rippleOffset * 0.05;
        }
        FragColor = vec4(calcReflexion(clipSpacePos, FragColor.rgb), FragColor.a);
    }

    gColor = FragColor;
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) {
       BrightColor = FragColor;
    } else {
       BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
