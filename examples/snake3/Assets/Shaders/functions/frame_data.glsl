#ifndef FRAME_DATA_GLSL
#define FRAME_DATA_GLSL

// Per-frame UBO bound to binding point 0 (UBO_BINDING_FRAME). Filled by
// RenderManager once per frame; consumed by every basic.vs/fs permutation.
// Layout MUST match Manager::FrameData C++ struct byte-for-byte (std140).
//
// D1.1b state: only camera data is read from this block. dirLight + point/
// spot light arrays remain reserved (so C++/GLSL layout stays in sync) but
// shader code reads them via the legacy `uniform DirLight dirLight` etc.
// declared in lights.glsl.

struct DirLight {
    vec3 position;   float _pad0;
    vec3 direction;  float _pad1;
    vec3 ambient;    float _pad2;
    vec3 diffuse;    float _pad3;
    vec3 specular;   float _pad4;
};

struct UboPointLight {
    vec3 position;   float constant;
    vec3 ambient;    float linear;
    vec3 diffuse;    float quadratic;
    vec3 specular;   float _pad0;
};

struct UboSpotLight {
    vec3 position;   float cutOff;
    vec3 direction;  float outerCutOff;
    vec3 ambient;    float constant;
    vec3 diffuse;    float linear;
    vec3 specular;   float quadratic;
    int pulse;       int _pad0; int _pad1; int _pad2;
};

layout(std140) uniform FrameData {
    mat4 frame_view;
    mat4 frame_projection;
    vec3 frame_viewPos;             float frame_uTime;
    DirLight frame_dirLight;
    int frame_directionLightEnable;
    int frame_numPointLights;
    int frame_numSpotLights;
    int _frame_pad0;
    UboPointLight frame_pointLights[8];
    UboSpotLight  frame_spotLights[8];
};

#endif
