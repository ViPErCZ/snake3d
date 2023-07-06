const int MAX_BONES = 100;
uniform mat4 finalBonesMatrices[MAX_BONES];

vec4 boneTransform(ivec4 ids, vec4 boneWeigts) {
    mat4 BoneTransform = finalBonesMatrices[ids[0]] * boneWeigts[0];;
    BoneTransform += finalBonesMatrices[ids[1]] * boneWeigts[1];
    BoneTransform += finalBonesMatrices[ids[2]] * boneWeigts[2];
    BoneTransform += finalBonesMatrices[ids[3]] * boneWeigts[3];

    return BoneTransform * vec4(aPos, 1.0);
}