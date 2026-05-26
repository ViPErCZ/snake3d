#ifndef SNAKE3_MATERIALUBO_H
#define SNAKE3_MATERIALUBO_H

#include <glm/glm.hpp>

#include "UboBindings.h"
#include "UniformBuffer.h"

namespace Manager {

    // Per-material UBO bound to binding point 1 (UBO_BINDING_MATERIAL).
    // Each MaterialInstance owns one; uploaded on dirty, bound on draw.
    //
    // D1.2a state: layout and C++/GLSL plumbing exist, but no shader code
    // reads any `material_*` field yet. This file lands the std140 struct
    // and a typed wrapper so D1.2b can attach it to MaterialInstance without
    // further infra churn.
    //
    // std140 packing: vec3+float pair fills one vec4 slot (16B). A vec4
    // member forces the block base alignment to 16 and inserts padding
    // before itself if needed. The sizeof() static_assert below catches
    // any drift between this C++ layout and material_data.glsl.
    struct alignas(16) MaterialDataStd140 {
        // 0    : vec3 + float pair (one vec4 slot, 16B)
        glm::vec3 material_ambientLightColor{1.0F, 1.0F, 1.0F};
        float     material_alpha{1.0F};

        // 16   : packed scalar block (each 4B, std140 lets adjacent
        //        scalars share a vec4 slot up to its 16B boundary)
        float material_ambientLightColorIntensity{1.0F};
        int   material_useMaterial{0};
        int   material_overrideColorMesh{0};
        int   material_hasAlbedoTexture{0};

        // 32
        int material_normalMapEnabled{0};
        int material_specularMapEnabled{0};
        int material_pbrEnabled{0};
        int material_hasHoleMap{0};

        // 48
        int material_fogEnable{0};
        int material_reflectionEnable{0};
        int material_rainDropEnable{0};
        float material_rainSpeed{0.2F};

        // 64
        float material_rainDensity{20.0F};
        // 68   : vec4 needs 16B alignment -> pad to offset 80
        float _pad0{0.0F};
        float _pad1{0.0F};
        float _pad2{0.0F};

        // 80   : clip plane (PlanarReflectionFeature default: keep everything
        //        above z=-1000 -- i.e. essentially "no clipping" until a
        //        reflection pass overrides it).
        glm::vec4 material_clipPlane{0.0F, 0.0F, 1.0F, 1000.0F};

        // 96   : two vec2s, each aligned to 8B -> they pack into one vec4 slot
        glm::vec2 material_uvScale{1.0F, 1.0F};
        glm::vec2 material_uvOffset{0.0F, 0.0F};
        // 112  : end; multiple of 16 so no trailing pad needed.
    };
    static_assert(sizeof(MaterialDataStd140) == 112,
                  "MaterialDataStd140 must match GLSL std140 layout in material_data.glsl");

    class MaterialUbo {
    public:
        // Lazy-inits on first call. Safe to invoke after GLEW is ready
        // without an explicit init() step (mirrors FrameUbo pattern).
        void upload(const MaterialDataStd140& data);
        void bind() const;

    private:
        UniformBuffer ubo;
    };

} // namespace Manager

#endif //SNAKE3_MATERIALUBO_H
