#include "PlanarReflectionFeature.h"

#include "../TextureSlots.h"
#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    PlanarReflectionFeature::PlanarReflectionFeature(std::shared_ptr<Manager::TextureManager> reflection,
                                                     const bool enabled,
                                                     const glm::vec4 clipPlane)
        : reflection(std::move(reflection)),
          enabled(enabled),
          clipPlane(clipPlane) {
    }

    void PlanarReflectionFeature::bind(Manager::ShaderProgram& shader,
                                       const Material::RenderContext& ctx) const {
        if (ctx.materialData) {
            ctx.materialData->material_reflectionEnable = enabled ? 1 : 0;
            ctx.materialData->material_clipPlane = clipPlane;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }

        if (enabled && reflection && reflection->hasTexture()) {
            shader.setInt("reflectionTexture", Material::TextureSlots::PlanarReflection);
            reflection->bind(Material::TextureSlots::PlanarReflection);
        }
    }

    void PlanarReflectionFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (reflection) {
            reflection->unbind(Material::TextureSlots::PlanarReflection);
        }
    }

    std::shared_ptr<IMaterialFeature> PlanarReflectionFeature::clone() const {
        return std::make_shared<PlanarReflectionFeature>(reflection, enabled, clipPlane);
    }
} // Feature
