#include "AlbedoFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    AlbedoFeature::AlbedoFeature(std::shared_ptr<Manager::TextureManager> albedo)
        : albedo(std::move(albedo)) {
    }

    void AlbedoFeature::bind(Manager::ShaderProgram& shader,
                             const Material::RenderContext& /*ctx*/) const {
        shader.setFloat("alpha", alpha);
        shader.setFloat("ambientLightColorIntensity", ambientIntensity);
        shader.setInt("material.ambient", Material::TextureSlots::Albedo);

        if (albedo && albedo->hasTexture()) {
            shader.setBool("useMaterial", false);
            shader.setBool("hasAlbedoTexture", true);
            albedo->bind(Material::TextureSlots::Albedo);
        } else {
            shader.setBool("useMaterial", true);
            shader.setBool("hasAlbedoTexture", false);
        }

        if (color.has_value()) {
            shader.setVec3("ambientLightColor", *color);
            shader.setBool("overrideColorMesh", true);
        } else {
            shader.setVec3("ambientLightColor", glm::vec3(1.0f));
            shader.setBool("overrideColorMesh", false);
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
