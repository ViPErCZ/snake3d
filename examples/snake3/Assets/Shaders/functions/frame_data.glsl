#ifndef FRAME_DATA_GLSL
#define FRAME_DATA_GLSL

// Per-frame UBO bound to binding point 0 (UBO_BINDING_FRAME). Filled by
// RenderManager::populateAndUploadFrameUbo() once per main pass + by
// PlanarReflectionRenderer for the mirror pass. Layout MUST match
// Manager::FrameData C++ struct byte-for-byte (std140).
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
