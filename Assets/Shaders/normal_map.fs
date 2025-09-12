#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 Color;
    vec3 Normal;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform vec3 viewPos;
uniform bool parallaxEnable = false;

vec2 TexCoords = fs_in.TexCoords;

#include "pipeline/parallax/parallax.glsl"
#include "functions/fog.glsl"
#include "functions/alpha.glsl"
#include "functions/lights.glsl"

void main()
{
    if (useMaterial) {
        // ambient
        float ambientStrength = 0.4;
        vec3 ambient = ambientStrength * fs_in.Color;

        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 norm = normalize(fs_in.FragPos);
        vec3 result = CalcDirLightMaterial(materialDirLight, norm, viewDir, fs_in.FragPos, ambient) * fs_in.Color;
        FragColor = alphaBlending(result);
    } else {
        vec2 texCoords = fs_in.TexCoords;
        vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
        if (parallaxEnable) {
            texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
            if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
        }

        // obtain normal from normal map in range [0,1]
        vec3 normal = texture(normalMap, texCoords).rgb;
        // transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

        // get diffuse color
        vec3 color = texture(diffuseMap, texCoords).rgb;
        // ambient
        vec3 ambient = 0.0001 * color;
        // diffuse
        vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = (diff * color) / 2;
        // specular
        //vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

        // vec3 specular = vec3(0.2) * spec;

        vec3 specular = vec3(1.0, 1.0, 1.0) * spec * vec3(texture(specularMap, texCoords)) / 2;

        FragColor = alphaBlending(vec3(ambient + diffuse + specular));
    }

    if (fogEnable) {
        vec3 V = fs_in.FragPos;
        float d = distance(viewPos, V);
        float alpha = getFogFactor(d);
        FragColor = mix(FragColor, vec4(0.6f, 0.6f, 0.7f, 1.f), alpha);
    }
}