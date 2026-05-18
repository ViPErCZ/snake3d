#include "NormalMapFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    NormalMapFeature::NormalMapFeature(std::shared_ptr<Manager::TextureManager> normal)
        : normal(std::move(normal)) {
    }

    void NormalMapFeature::bind(Manager::ShaderManager& shader,
                                const Material::RenderContext& /*ctx*/) const {
        shader.setInt("material.diffuse", Material::TextureSlots::Normal);
        const bool active = normal && normal->hasTexture();
        shader.setBool("normalMapEnabled", active);
        if (active) {
            normal->bind(Material::TextureSlots::Normal);
        }
    }

    void NormalMapFeature::unbind(Manager::ShaderManager& /*shader*/) const {
        if (normal) {
            normal->unbind(Material::TextureSlots::Normal);
        }
    }

    std::shared_ptr<IMaterialFeature> NormalMapFeature::clone() const {
        return std::make_shared<NormalMapFeature>(normal);
    }
} // Feature
