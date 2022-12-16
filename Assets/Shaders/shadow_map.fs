#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool shadowsEnable = true;

#include "pipeline/shading/shading.glsl"

void main()
{
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    if (length(normal) == 0.0) // normal texture is not set
    {
        normal = normalize(fs_in.Normal);
    } else {
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    }
    vec3 lightColor = vec3(0.3);
    if (shadowsEnable == false) {
        lightColor = texture(diffuseMap, fs_in.TexCoords).rgb;
    }
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    if (shadowsEnable == false) {
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        specular = vec3(1.0, 1.0, 1.0) * spec * vec3(texture(specularMap, fs_in.TexCoords));
        FragColor = vec4(ambient + diffuse + specular, 1.0);
    } else {
        // calculate shadow
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace, shadowMap);
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
        FragColor = vec4(lighting, 1.0);
    }
}