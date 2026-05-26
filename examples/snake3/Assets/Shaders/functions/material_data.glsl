#ifndef MATERIAL_DATA_GLSL
#define MATERIAL_DATA_GLSL

// Per-material UBO bound to binding point 1 (UBO_BINDING_MATERIAL).
// Filled by MaterialInstance::bind on upload; consumed by every basic.vs/fs
// permutation. Layout MUST match Manager::MaterialDataStd140 byte-for-byte
// (std140 packing).
//
// D1.2a state: this block is declared in basic.vs/basic.fs/lights.glsl but
// NO shader code reads any `material_*` member yet. Purpose of D1.2a is
// purely to land the plumbing -- features still write the legacy
// `uniform vec3 ambientLightColor; ...` etc. via setUniform. Successive
// D1.2x phases migrate readers one feature at a time.

layout(std140) uniform MaterialData {
    vec3  material_ambientLightColor;     float material_alpha;
    float material_ambientLightColorIntensity;
    int   material_useMaterial;
    int   material_overrideColorMesh;
    int   material_hasAlbedoTexture;
    int   material_normalMapEnabled;
    int   material_specularMapEnabled;
    int   material_pbrEnabled;
    int   material_hasHoleMap;
    int   material_fogEnable;
    int   material_reflectionEnable;
    int   material_rainDropEnable;
    float material_rainSpeed;
    float material_rainDensity;
    vec4  material_clipPlane;
    vec2  material_uvScale;               vec2 material_uvOffset;
};

#endif
