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

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool useMaterial = false;
uniform bool parallaxEnable = false;

#include "pipeline/parallax/parallax.glsl"
#include "pipeline/fog/fog.glsl"
#include "pipeline/blending/alpha.glsl"

void main()
{
    if (useMaterial) {
        // ambient
        float ambientStrength = 0.4;
        vec3 ambient = ambientStrength * fs_in.Color;

        // diffuse
        vec3 norm = normalize(fs_in.FragPos);
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(0.8, 0.8, 0.8);

        // specular
        float specularStrength = 0.6;
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * fs_in.Color;

        vec3 result = (ambient + diffuse + specular) * fs_in.Color;
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
        vec3 ambient = 0.1 * color;
        // diffuse
        vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color;
        // specular
        //vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

        // vec3 specular = vec3(0.2) * spec;

        vec3 specular = vec3(1.0, 1.0, 1.0) * spec * vec3(texture(specularMap, texCoords));

        FragColor = alphaBlending(vec3(ambient + diffuse + specular));
    }

    if (fogEnable) {
        vec3 V = fs_in.FragPos;
        float d = distance(viewPos, V);
        float alpha = getFogFactor(d);
        FragColor = mix(FragColor, vec4(0.6f, 0.6f, 0.7f, 1.f), alpha);
    }
}