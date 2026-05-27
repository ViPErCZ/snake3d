#include "SpecularFeature.h"

#include "../TextureSlots.h"
#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    SpecularFeature::SpecularFeature(std::shared_ptr<Manager::TextureManager> specular,
                                     const float shininess)
        : specular(std::move(specular)), shininess(shininess) {
    }

    void SpecularFeature::bind(Manager::ShaderProgram& shader,
                               const Material::RenderContext& ctx) const {
        shader.setInt("material.specular", Material::TextureSlots::Specular);
        // material.shininess stays legacy -- Mesa pow(x, 0) quirk means we
        // can't move shininess into the UBO without changing rendering
        // behaviour. Spec sampler likewise stays legacy (samplers can't live
        // in a UBO).
        shader.setFloat("material.shininess", shininess);

        const bool active = specular && specular->hasTexture();
        // D1.2d.3: specularMapEnabled migrated to MaterialData UBO
        // (material_specularMapEnabled). Shininess/sampler stay above.
        if (ctx.materialData) {
            ctx.materialData->material_specularMapEnabled = active ? 1 : 0;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }
        if (active) {
            specular->bind(Material::TextureSlots::Specular);
        }
    }

    void SpecularFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (specular) {
            specular->unbind(Material::TextureSlots::Specular);
        }
    }

    std::shared_ptr<IMaterialFeature> SpecularFeature::clone() const {
        return std::make_shared<SpecularFeature>(specular, shininess);
    }
} // Feature
