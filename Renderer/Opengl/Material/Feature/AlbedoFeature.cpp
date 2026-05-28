#include "AlbedoFeature.h"

#include "../TextureSlots.h"
#include <snake3d/Manager/MaterialUbo.h>

namespace Feature {
    AlbedoFeature::AlbedoFeature(std::shared_ptr<Manager::TextureManager> albedo)
        : albedo(std::move(albedo)) {
    }

    void AlbedoFeature::bind(Manager::ShaderProgram& shader,
                             const Material::RenderContext& ctx) const {
        const bool hasTexture = albedo && albedo->hasTexture();

        if (ctx.materialData) {
            auto& md = *ctx.materialData;
            md.material_alpha = alpha;
            md.material_ambientLightColorIntensity = ambientIntensity;
            md.material_ambientLightColor = color.value_or(glm::vec3(1.0F));
            md.material_useMaterial = hasTexture ? 0 : 1;
            md.material_hasAlbedoTexture = hasTexture ? 1 : 0;
            md.material_overrideColorMesh = color.has_value() ? 1 : 0;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }

        // Sampler slot + texture binding stay legacy -- texture binding isn't
        // part of the UBO migration. `material.ambient` is a sampler2D and
        // samplers can't live inside a UBO.
        shader.setInt("material.ambient", Material::TextureSlots::Albedo);
        if (hasTexture) {
            albedo->bind(Material::TextureSlots::Albedo);
        }
    }

    void AlbedoFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (albedo) {
            albedo->unbind(Material::TextureSlots::Albedo);
        }
    }

    std::shared_ptr<IMaterialFeature> AlbedoFeature::clone() const {
        auto copy = std::make_shared<AlbedoFeature>(albedo);
        copy->color = color;
        copy->alpha = alpha;
        copy->ambientIntensity = ambientIntensity;
        return copy;
    }
} // Feature
