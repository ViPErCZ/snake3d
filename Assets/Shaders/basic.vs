#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out vec2 TexCoords;
out vec3 fragPos;
out vec3 Color;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform bool useBones = true;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    if (useBones) {
        mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
        BoneTransform += finalBonesMatrices[boneIds[1]] * weights[1];
        BoneTransform += finalBonesMatrices[boneIds[2]] * weights[2];
        BoneTransform += finalBonesMatrices[boneIds[3]] * weights[3];

        vec4 PosL = BoneTransform * vec4(aPos, 1.0);

        mat4 viewModel = view * model;
        gl_Position = projection * viewModel * PosL;
    } else {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }

    TexCoords = aTexCoords;
    Color = aColor;
    fragPos = vec3(model * vec4(aPos, 1.0));
}

