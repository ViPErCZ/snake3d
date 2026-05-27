#ifndef SNAKE3_FRAMEUBO_H
#define SNAKE3_FRAMEUBO_H

#include <glm/glm.hpp>

#include "UboBindings.h"
#include "UniformBuffer.h"

namespace Manager {

    // std140 packing: vec3+float pair fills one vec4 slot (16B). C++ layout
    // must match GLSL std140 byte-for-byte - sizeof() static_assert below
    // catches any drift.
    //
    // D1.1b scope: camera fields (view / projection / viewPos / uTime).
    // D1.1c experiment moved dirLight into FrameData, but that broke the
    // PlayerScene / RemoteSnakeScene path which uses its own (dim) local
    // DirectionalLight per snake material. Snake materials would silently
    // pick up the global (bright) light from FrameData and render too
    // bright. D1.1c-fix: dirLight migrated to MaterialData UBO instead, so
    // every material carries its own copy (still UBO, just per-material).
    // FrameData reverted to camera-only.

    struct alignas(16) FrameData {
        glm::mat4 view;                          // offset 0
        glm::mat4 projection;                    // offset 64
        glm::vec3 viewPos;        float uTime;   // offset 128 (vec3+float pair)
        // 144
    };
    static_assert(sizeof(FrameData) == 144);

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
