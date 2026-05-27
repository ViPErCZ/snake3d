#ifndef SNAKE3_MATERIALUBO_H
#define SNAKE3_MATERIALUBO_H

#include <glm/glm.hpp>

#include "UniformBuffer.h"

namespace Manager {

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

        // 112  : D1.1c per-material opt-out for the directional light. The
        // tile material (snake body spheres) sets this to 0 so the directional
        // shading block is skipped and only the ambient red shows through,
        // matching pre-D1 visuals. All other materials default to 1 once
        // LightingFeature has a directional light wired in.
        int material_directionLightEnable{0};
        float _pad3{0.0F};
        float _pad4{0.0F};
        float _pad5{0.0F};

        // 128  : D1.1c-fix per-material directional light. Snake body/head
        // materials carry their own (intentionally dim) DirectionalLight in
        // PlayerScene / RemoteSnakeScene; pre-D1 the LightingFeature copied
        // those into per-program `dirLight.*` uniforms. After D1.1c they need
        // to live next to material_directionLightEnable so each material
        // brings its own light parameters in.
        //
        // std140: each vec3 is aligned to 16B and consumes a full vec4 slot
        // (trailing 4B padding). We mirror that with explicit `_pad` floats
        // so sizeof() matches the GLSL view byte-for-byte.
        glm::vec3 material_dirLight_direction{0.0F}; float _pad6{0.0F};   // 128
        glm::vec3 material_dirLight_ambient{0.0F};   float _pad7{0.0F};   // 144
        glm::vec3 material_dirLight_diffuse{0.0F};   float _pad8{0.0F};   // 160
        glm::vec3 material_dirLight_specular{0.0F};  float _pad9{0.0F};   // 176
        // 192  : end; multiple of 16 so no trailing pad needed.
    };
    static_assert(sizeof(MaterialDataStd140) == 192,
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
