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
out vec2 outUvScale;

uniform mat4 model;
// Default false: when FEATURE_BONES isn't injected and a caller forgets to
// set useBones explicitly, the non-bones path runs (gl_Position from aPos).
// Skeletal materials (AnimationArrayMesh) set useBones=true explicitly.
uniform bool useBones = false;
// D1.2d.1: uvScale / uvOffset migrated to MaterialData UBO
// (material_uvScale / material_uvOffset). Legacy uniform vec2 declarations
// removed -- UvTransformFeature writes the UBO shadow instead.
// D1.1c: lightPos uniform removed. TangentLightPos výstup byl historicky
// určený pro tangent-space directional shading, ale basic.fs ho nikde
// nečte (mrtvý kód). Zachovávám výstup, abychom neměnili VS/FS interface,
// a po D1.1c-fix ho už neplníme (pozice světla nemáme -- DirLight v
// MaterialData UBO drží jenom direction). Nastavujeme na vec3(0.0).

// D1.1b: view / projection / viewPos migrated to FrameData UBO (slot 0).
// Read via frame_view / frame_projection / frame_viewPos below.
#include "snake3d/frame_data.glsl"
#include "functions/bonesTransform.glsl"
#include "snake3d/material_data.glsl"

void main()
{
    mat4 viewModel = frame_view * model;

#ifdef FEATURE_BONES
    if (useBones) {
        gl_Position = frame_projection * viewModel * boneTransform(boneIds, weights);
    }
#endif
    if (!useBones) {
        gl_Position = frame_projection * viewModel * vec4(aPos, 1.0);
    }

    vec2 uv = aTexCoords * material_uvScale + material_uvOffset;
    TexCoords = uv;
    fragPos = vec3(model * (useBones ? boneTransform(boneIds, weights) : vec4(aPos, 1.0)));

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
    TangentLightPos = TBN * vec3(0.0);
    TangentFragPos  = TBN * fragPos;
    TangentViewPos  = TBN * frame_viewPos;

    Normal = mat3(transpose(inverse(viewModel))) * aNormal;
    worldNormal = mat3(model) * aNormal;
    //worldNormal = normalize(normalMatrix * aNormal);
    modelNormal = normalize(transpose(inverse(mat3(model))) * aNormal);
    //Normal = mat3(model) * aNormal;
    camPos = frame_viewPos;
    meshColor = aColor;
    viewMatrix = frame_view;
    outUvScale = material_uvScale;
    clipSpacePos = gl_Position;
}
