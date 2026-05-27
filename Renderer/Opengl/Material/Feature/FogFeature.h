#ifndef SNAKE3_FOGFEATURE_H
#define SNAKE3_FOGFEATURE_H

#include "IMaterialFeature.h"
#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    // Toggle fog blending. Shader (basic.fs s FEATURE_FOG) v
    // `if (material_fogEnable != 0)` smíchá final color s fog barvou podle
    // distance. Konkrétní fog factor funkce (getFogFactor) je v
    // functions/fog_material.glsl (MaterialInstance-driven variant).
    //
    // D1.2d.6: fogEnable migrated to MaterialData UBO (material_fogEnable).
    class FogFeature final : public IMaterialFeature {
    public:
        explicit FogFeature(bool enabled = true) : enabled(enabled) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::Fog);
        }

        void bind(Manager::ShaderProgram& /*shader*/,
                  const Material::RenderContext& ctx) const override {
            if (ctx.materialData) {
                ctx.materialData->material_fogEnable = enabled ? 1 : 0;
                if (ctx.materialDirty) *ctx.materialDirty = true;
            }
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
