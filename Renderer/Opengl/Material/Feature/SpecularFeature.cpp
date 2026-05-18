#include "SpecularFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    SpecularFeature::SpecularFeature(std::shared_ptr<Manager::TextureManager> specular,
                                     const float shininess)
        : specular(std::move(specular)), shininess(shininess) {
    }

    void SpecularFeature::bind(Manager::ShaderManager& shader,
                               const Material::RenderContext& /*ctx*/) const {
        shader.setInt("material.specular", Material::TextureSlots::Specular);
        shader.setFloat("material.shininess", shininess);

        const bool active = specular && specular->hasTexture();
        shader.setBool("specularMapEnabled", active);
        if (active) {
            specular->bind(Material::TextureSlots::Specular);
        }
    }

    void SpecularFeature::unbind(Manager::ShaderManager& /*shader*/) const {
        if (specular) {
            specular->unbind(Material::TextureSlots::Specular);
        }
    }

    std::shared_ptr<IMaterialFeature> SpecularFeature::clone() const {
        return std::make_shared<SpecularFeature>(specular, shininess);
    }
} // Feature
