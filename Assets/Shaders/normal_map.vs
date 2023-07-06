#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 Color;
    vec3 Normal;
} vs_out;

uniform bool useBones = false;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

#include "pipeline/skeleton/bonesTransform.glsl"

void main()
{
    vs_out.FragPos = vec3(model * (useBones ? boneTransform(boneIds, weights) : vec4(aPos, 1.0)));
    vs_out.TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.Color = aColor;
    vs_out.Normal = aNormal;

    if (useBones) {
        mat4 viewModel = view * model;
        gl_Position = projection * viewModel * boneTransform(boneIds, weights);
    } else {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}
