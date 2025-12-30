#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out vec2 TexCoords;
out vec3 fragPos;
out vec3 TangentLightPos;
out vec3 TangentFragPos;
out vec3 TangentViewPos;
out vec3 Normal;
out vec3 worldNormal;
out vec3 modelNormal;
out mat3 TBN;
out vec3 camPos;
out vec3 meshColor;
out mat4 viewMatrix;
out vec4 clipSpacePos;

uniform vec3 viewPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool useBones = true;
uniform vec2 uvScale = vec2(1.0, 1.0);
uniform vec2 uvOffset = vec2(0.0, 0.0);
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0);
//uniform vec3 cameraPos;

#include "functions/bonesTransform.glsl"

void main()
{
    mat4 viewModel = view * model;

    if (useBones) {
        gl_Position = projection * viewModel * boneTransform(boneIds, weights);
    } else {
        gl_Position = projection * viewModel * vec4(aPos, 1.0);
    }

    vec2 uv = aTexCoords * uvScale + uvOffset;
    TexCoords = uv;
    fragPos = vec3(model * (useBones ? boneTransform(boneIds, weights) : vec4(aPos, 1.0)));

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
    TangentLightPos = TBN * lightPos;
    TangentFragPos  = TBN * fragPos;
    TangentViewPos  = TBN * viewPos;

    Normal = mat3(transpose(inverse(viewModel))) * aNormal;
    worldNormal = mat3(model) * aNormal;
    //worldNormal = normalize(normalMatrix * aNormal);
    modelNormal = normalize(transpose(inverse(mat3(model))) * aNormal);
    //Normal = mat3(model) * aNormal;
    camPos = viewPos;
    meshColor = aColor;
    viewMatrix = view;
    clipSpacePos = gl_Position;
}
