#ifndef MATERIAL_DATA_GLSL
#define MATERIAL_DATA_GLSL

// Per-material UBO bound to binding point 1 (UBO_BINDING_MATERIAL).
// Filled by MaterialInstance::bind on upload; consumed by every basic.vs/fs
// permutation. Layout MUST match Manager::MaterialDataStd140 byte-for-byte
// (std140 packing).
//
// ENGINE-OWNED shader prelude. Resolved by any shader via
//   #include "snake3d/material_data.glsl"
// through the ShaderLoader engine include root (default "EngineShaders/",
// populated by the copy_engine_shaders CMake target). Examples no longer
// carry a private copy -- this is the single source of truth for the
// MaterialData UBO layout.
//
// D1.2x state (rolling migration): most ambient / fog / reflection /
// rain / uv / hole flags now read from this UBO instead of legacy
// `uniform vec3 ambientLightColor; ...` etc. D1.1c also moves the
// directional light gate (material_directionLightEnable) in here so
// the tile material (snake body) can opt out without an extra setBool
// call per draw.
//
// D1.1c-fix: directional light fields (direction/ambient/diffuse/specular)
// migrated from FrameData into MaterialData. The snake body/head scenes
// build their own dim DirectionalLight per material; making them per-
// material restores the pre-D1 visuals (FrameData would have forced the
// global bright light onto the snake).

// DirLight struct lives here now (was in frame_data.glsl during D1.1c).
// lights.glsl includes this file so CalcDirLight* signatures resolve the
// type. position field dropped -- it wasn't read by CalcDirLight* (only by
// basic.vs's TangentLightPos output, which basic.fs declares but never
// reads -- dead code).
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// D1.1d: PointLight / SpotLight struct definitions moved here from
// lights.glsl. The UBO block below references them, so they must be visible
// before the `layout(std140) uniform MaterialData` block. Field order MUST
// match Manager::PointLightStd140 / SpotLightStd140 byte-for-byte; vec3+pad
// padding is implicit in std140, but field ORDER is load-bearing.
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    bool  pulse; // std140: bool is 4 B
};

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
    // D1.1c per-material opt-out for the directional light. Tile material
    // (snake body) keeps this at 0 so the directional block is skipped and
    // the sphere stays pure-ambient red, matching pre-D1 visuals.
    int   material_directionLightEnable;
    // D1.1c-fix per-material directional light fields. Each material carries
    // its own copy so PlayerScene's dim local DirectionalLight is honored
    // (pre-D1 LightingFeature wrote these as `dirLight.*` per-program
    // uniforms). Synthesized into a local DirLight at the call site in
    // basic.fs / respawn.fs / explosion.fs.
    vec3  material_dirLight_direction;
    vec3  material_dirLight_ambient;
    vec3  material_dirLight_diffuse;
    vec3  material_dirLight_specular;
    // D1.1d per-material point/spot light arrays + counts. Tile material
    // (snake body) leaves these at zero so the body stays pure-ambient red
    // even when the rest of the scene has active street lamps.
    PointLight material_pointLights[8];
    SpotLight  material_spotLights[8];
    int        material_numPointLights;
    int        material_numSpotLights;

    // Emissive feature (H7+). When material_emissiveEnabled != 0, basic.fs
    // adds (material_emissive_color * material_emissive_intensity) to final
    // fragment color AFTER all light contributions. Pre-multiplied; gated by
    // FEATURE_EMISSIVE_BLOOM ifdef (bit 10 in ShaderFeatureMask).
    vec3  material_emissive_color;
    float material_emissive_intensity;
    int   material_emissiveEnabled;
};

#endif
