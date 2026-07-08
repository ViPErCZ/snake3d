#ifndef FRAME_DATA_GLSL
#define FRAME_DATA_GLSL

// Per-frame UBO bound to binding point 0 (UBO_BINDING_FRAME). Filled by
// RenderManager::populateAndUploadFrameUbo() once per main pass + by
// PlanarReflectionRenderer for the mirror pass. Layout MUST match
// Manager::FrameData C++ struct byte-for-byte (std140).
//
// ENGINE-OWNED shader prelude. Resolved by any shader via
//   #include "snake3d/frame_data.glsl"
// through the ShaderLoader engine include root (default "EngineShaders/",
// populated by the copy_engine_shaders CMake target). Examples no longer
// carry a private copy -- this is the single source of truth for the
// FrameData UBO layout.
//
// D1.1c-fix: directional light moved out of FrameData into MaterialData so
// each material (snake body/head, walls, floor, ...) can carry its own
// (potentially dim) DirectionalLight. FrameData is back to camera-only.

layout(std140) uniform FrameData {
    mat4  frame_view;
    mat4  frame_projection;
    vec3  frame_viewPos;
    float frame_uTime;
};

#endif
