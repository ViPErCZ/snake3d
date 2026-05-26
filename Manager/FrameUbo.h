#ifndef SNAKE3_FRAMEUBO_H
#define SNAKE3_FRAMEUBO_H

#include <glm/glm.hpp>

#include "UboBindings.h"
#include "UniformBuffer.h"

namespace Manager {

    inline constexpr int MAX_POINT_LIGHTS = 8;
    inline constexpr int MAX_SPOT_LIGHTS  = 8;

    // std140 packing: vec3+float pair fills one vec4 slot (16B). C++ layout
    // must match GLSL std140 byte-for-byte - sizeof() static_asserts below
    // catch any drift.

    struct alignas(16) DirLightStd140 {
        glm::vec3 position;   float _pad0;
        glm::vec3 direction;  float _pad1;
        glm::vec3 ambient;    float _pad2;
        glm::vec3 diffuse;    float _pad3;
        glm::vec3 specular;   float _pad4;
    };
    static_assert(sizeof(DirLightStd140) == 80);

    struct alignas(16) PointLightStd140 {
        glm::vec3 position;   float constant;
        glm::vec3 ambient;    float linear;
        glm::vec3 diffuse;    float quadratic;
        glm::vec3 specular;   float _pad0;
    };
    static_assert(sizeof(PointLightStd140) == 64);

    struct alignas(16) SpotLightStd140 {
        glm::vec3 position;   float cutOff;
        glm::vec3 direction;  float outerCutOff;
        glm::vec3 ambient;    float constant;
        glm::vec3 diffuse;    float linear;
        glm::vec3 specular;   float quadratic;
        int pulse;            int _pad0; int _pad1; int _pad2;
    };
    static_assert(sizeof(SpotLightStd140) == 96);

    struct alignas(16) FrameData {
        glm::mat4 view;                          // 0
        glm::mat4 projection;                    // 64
        glm::vec3 viewPos;        float uTime;   // 128
        DirLightStd140 dirLight;                 // 144
        int directionLightEnable;                // 224
        int numPointLights;                      // 228
        int numSpotLights;                       // 232
        int _pad0;                               // 236
        PointLightStd140 pointLights[MAX_POINT_LIGHTS];  // 240, 8*64=512
        SpotLightStd140  spotLights[MAX_SPOT_LIGHTS];    // 752, 8*96=768
    };
    static_assert(sizeof(FrameData) == 1520);

    class FrameUbo {
    public:
        void init();
        // Lazy-inits on first call - safe to invoke after GLEW is ready
        // without an explicit init() step.
        void upload(const FrameData& data);
        void bind() const;

    private:
        UniformBuffer ubo;
    };

} // namespace Manager

#endif //SNAKE3_FRAMEUBO_H
