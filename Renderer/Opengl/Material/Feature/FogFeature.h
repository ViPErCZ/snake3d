#ifndef SNAKE3_FOGFEATURE_H
#define SNAKE3_FOGFEATURE_H

#include "IMaterialFeature.h"

namespace Feature {
    // Toggle fog blending. Shader (basic.fs s FEATURE_FOG) v `if (fogEnable)`
    // smíchá final color s fog barvou podle distance. Konkrétní fog factor
    // funkce (getFogFactor) je v functions/fog.glsl.
    class FogFeature final : public IMaterialFeature {
    public:
        explicit FogFeature(bool enabled = true) : enabled(enabled) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::Fog);
        }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& /*ctx*/) const override {
            shader.setBool("fogEnable", enabled);
        }

        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<FogFeature>(enabled);
        }

        void setEnabled(const bool e) { enabled = e; }
        [[nodiscard]] bool isEnabled() const { return enabled; }

    private:
        bool enabled;
    };
} // Feature

#endif //SNAKE3_FOGFEATURE_H
