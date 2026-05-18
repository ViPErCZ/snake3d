#include "PlanarReflectionFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    PlanarReflectionFeature::PlanarReflectionFeature(std::shared_ptr<Manager::TextureManager> reflection,
                                                     const bool enabled,
                                                     const glm::vec4 clipPlane)
        : reflection(std::move(reflection)),
          enabled(enabled),
          clipPlane(clipPlane) {
    }

    void PlanarReflectionFeature::bind(Manager::ShaderManager& shader,
                                       const Material::RenderContext& /*ctx*/) const {
        shader.setBool("reflectionEnable", enabled);
        shader.setVec4("clipPlane", clipPlane);

        if (enabled && reflection && reflection->hasTexture()) {
            shader.setInt("reflectionTexture", Material::TextureSlots::PlanarReflection);
            reflection->bind(Material::TextureSlots::PlanarReflection);
        }
    }

    void PlanarReflectionFeature::unbind(Manager::ShaderManager& /*shader*/) const {
        if (reflection) {
            reflection->unbind(Material::TextureSlots::PlanarReflection);
        }
    }

    std::shared_ptr<IMaterialFeature> PlanarReflectionFeature::clone() const {
        return std::make_shared<PlanarReflectionFeature>(reflection, enabled, clipPlane);
    }
} // Feature
