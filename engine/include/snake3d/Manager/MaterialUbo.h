#ifndef SNAKE3_MATERIALUBO_H
#define SNAKE3_MATERIALUBO_H

#include <glm/glm.hpp>

#include <snake3d/Manager/UniformBuffer.h>

namespace Manager {

    // D1.1d: per-material point + spot light arrays migrated from per-program
    // uniforms into MaterialData UBO. Layout MUST match GLSL std140 view --
    // each vec3 still has 16B base alignment, but a float that immediately
    // follows a vec3 PACKS into the trailing 4B of that vec3's 16B slot
    // (std140 only forces padding when the next member's alignment requires
    // it, e.g. another vec3). The earlier "vec3 + _padX after EVERY vec3"
    // pattern silently shifted every following float by 4B, which manifested
    // as broken point-light attenuation + spotlights that lost their cone.
    struct alignas(16) PointLightStd140 {
        glm::vec3 position{0.0F}; float _p0{0.0F};   // 0  | next vec3 -> pad
        glm::vec3 ambient{0.0F};  float _p1{0.0F};   // 16 | next vec3 -> pad
        glm::vec3 diffuse{0.0F};  float _p2{0.0F};   // 32 | next vec3 -> pad
        glm::vec3 specular{0.0F};                     // 48 | next is float, packs at 60
        float constant{1.0F};                         // 60
        float linear{0.0F};                           // 64
        float quadratic{0.0F};                        // 68
        float _p3{0.0F};                              // 72
        float _p4{0.0F};                              // 76 -> end 80
    };                                                // 80 B
    static_assert(sizeof(PointLightStd140) == 80,
                  "PointLightStd140 must be 80 B (std140)");

    struct alignas(16) SpotLightStd140 {
        glm::vec3 position{0.0F};  float _p0{0.0F};  // 0  | next vec3 -> pad
        glm::vec3 direction{0.0F}; float _p1{0.0F};  // 16 | next vec3 -> pad
        glm::vec3 ambient{0.0F};   float _p2{0.0F};  // 32 | next vec3 -> pad
        glm::vec3 diffuse{0.0F};   float _p3{0.0F};  // 48 | next vec3 -> pad
        glm::vec3 specular{0.0F};                     // 64 | next is float, packs at 76
        float constant{1.0F};                         // 76
        float linear{0.0F};                           // 80
        float quadratic{0.0F};                        // 84
        float cutOff{0.0F};                           // 88
        float outerCutOff{0.0F};                      // 92
        int   pulse{0};                               // 96
        int   _p4{0};                                 // 100
        int   _p5{0};                                 // 104
        int   _p6{0};                                 // 108 -> end 112
    };                                                // 112 B
    static_assert(sizeof(SpotLightStd140) == 112,
                  "SpotLightStd140 must be 112 B (std140)");

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

        // 192  : D1.1d per-material point/spot light arrays. SnakeMeshNode3D
        // builds its tile material with empty light vectors so the snake body
        // stays pure-ambient red even when the scene has 4 active lamps --
        // per-material storage preserves that contract (FrameUBO would force
        // them all on globally).
        //
        // std140 array stride = sizeof(struct) (both are multiples of 16).
        // 8 * 80 = 640 B for points, 8 * 112 = 896 B for spots.
        PointLightStd140 material_pointLights[8];                         // 192 .. 832
        SpotLightStd140  material_spotLights[8];                          // 832 .. 1728

        // 1728 : light counts. Two ints fit in the first half of the vec4
        // slot; pad the rest so the next struct starts 16-aligned.
        int   material_numPointLights{0};
        int   material_numSpotLights{0};
        float _padN0{0.0F};
        float _padN1{0.0F};

        // 1744 : emissive feature (self-illumination). vec3+float pair packs
        // into one vec4 slot. material_emissiveEnabled=0 means feature absent
        // -- shader emissive branch is skipped entirely (gated by FEATURE_
        // EMISSIVE_BLOOM ifdef + this flag).
        glm::vec3 material_emissive_color{0.0F, 0.0F, 0.0F};
        float     material_emissive_intensity{0.0F};
        int       material_emissiveEnabled{0};
        float _padE0{0.0F};
        float _padE1{0.0F};
        float _padE2{0.0F};
        // 1776 : end.
    };
    static_assert(sizeof(MaterialDataStd140) == 1776,
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
