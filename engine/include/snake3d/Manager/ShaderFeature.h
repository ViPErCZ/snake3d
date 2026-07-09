#ifndef SNAKE3_SHADERFEATURE_H
#define SNAKE3_SHADERFEATURE_H

#include <cstdint>
#include <string>
#include <vector>

namespace Manager {
    // Modální feature flags pro shader permutace. Injektují se jako
    // `#define FEATURE_<name>` do GLSL zdroje před kompilací.
    //
    // Smysl: feature je tu jen pro "modální přepínače" co mění celou pipeline
    // (PBR vs Phong, shadows on/off, IBL). Per-material lokální kód (HoleMap,
    // RainRipple, vertex displacement) se v fázi B+ řeší přes snippet injekci,
    // nikoli feature flagy.
    enum class ShaderFeature : uint32_t {
        None             = 0,
        PBR              = 1u << 0,
        NormalMap        = 1u << 1,
        Shadows          = 1u << 2,
        DirectionalLight = 1u << 3,
        PointLights      = 1u << 4,
        SpotLights       = 1u << 5,
        Fog              = 1u << 6,
        IBL              = 1u << 7,
        Bones            = 1u << 8,
        AlphaBlend       = 1u << 9,
        EmissiveBloom    = 1u << 10,
        // bit 11 = FREE (was HoleMap pre-C2b; migrated to snippet injection)
        RainRipple       = 1u << 12,
    };

    using ShaderFeatureMask = uint32_t;

    constexpr ShaderFeatureMask operator|(ShaderFeature a, ShaderFeature b) {
        return static_cast<ShaderFeatureMask>(a) | static_cast<ShaderFeatureMask>(b);
    }

    constexpr ShaderFeatureMask operator|(ShaderFeatureMask a, ShaderFeature b) {
        return a | static_cast<ShaderFeatureMask>(b);
    }

    constexpr bool has(ShaderFeatureMask mask, ShaderFeature f) {
        return (mask & static_cast<ShaderFeatureMask>(f)) != 0;
    }

    // Vrací jméno feature pro `#define FEATURE_<name>` injekci.
    // Pořadí v `enumerate()` musí odpovídat pořadí v `enum class`.
    inline const char* featureName(ShaderFeature f) {
        switch (f) {
            case ShaderFeature::PBR:              return "FEATURE_PBR";
            case ShaderFeature::NormalMap:        return "FEATURE_NORMAL_MAP";
            case ShaderFeature::Shadows:          return "FEATURE_SHADOWS";
            case ShaderFeature::DirectionalLight: return "FEATURE_DIRECTIONAL_LIGHT";
            case ShaderFeature::PointLights:      return "FEATURE_POINT_LIGHTS";
            case ShaderFeature::SpotLights:       return "FEATURE_SPOT_LIGHTS";
            case ShaderFeature::Fog:              return "FEATURE_FOG";
            case ShaderFeature::IBL:              return "FEATURE_IBL";
            case ShaderFeature::Bones:            return "FEATURE_BONES";
            case ShaderFeature::AlphaBlend:       return "FEATURE_ALPHA_BLEND";
            case ShaderFeature::EmissiveBloom:    return "FEATURE_EMISSIVE_BLOOM";
            case ShaderFeature::RainRipple:       return "FEATURE_RAIN_RIPPLE";
            case ShaderFeature::None:             return "";
        }
        return "";
    }

    inline std::vector<std::string> definesForMask(const ShaderFeatureMask mask) {
        std::vector<std::string> defines;
        for (uint32_t bit = 0; bit < 32; ++bit) {
            const auto flag = static_cast<ShaderFeature>(1u << bit);
            if (has(mask, flag)) {
                if (const char* name = featureName(flag); name[0] != '\0') {
                    defines.emplace_back(name);
                }
            }
        }
        return defines;
    }
} // Manager

#endif //SNAKE3_SHADERFEATURE_H
