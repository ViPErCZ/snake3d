#include "PbrFeature.h"

#include "../TextureSlots.h"
#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    PbrFeature::PbrFeature(std::shared_ptr<Manager::TextureManager> metalness,
                           std::shared_ptr<Manager::TextureManager> roughness,
                           std::shared_ptr<Manager::TextureManager> aoMap)
        : metalness(std::move(metalness)),
          roughness(std::move(roughness)),
          aoMap(std::move(aoMap)) {
    }

    void PbrFeature::bind(Manager::ShaderProgram& shader,
                          const Material::RenderContext& ctx) const {
        const bool hasMetal = metalness && metalness->hasTexture();
        const bool hasRough = roughness && roughness->hasTexture();
        const bool hasAo    = aoMap && aoMap->hasTexture();

        if (ctx.materialData) {
            ctx.materialData->material_pbrEnabled = (hasMetal || hasRough) ? 1 : 0;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }

        // NOTE on uniform names: StandardMaterial::bind historically called
        // setInt("roughness", 5) and setInt("aoMap", 7), but basic.fs declares
        // `roughnessMap` and accesses `material.aoMap`. Those setInt calls
        // therefore silently no-op (location -1) and the samplers stay at the
        // default texture unit 0. We preserve that quirky behaviour here to
        // avoid changing rendering during B6 migration; a dedicated follow-up
        // can normalise uniform names later.
        if (hasMetal) {
            shader.setInt("metalness", Material::TextureSlots::Metalness);
            metalness->bind(Material::TextureSlots::Metalness);
        }
        if (hasRough) {
            shader.setInt("roughness", Material::TextureSlots::Roughness);
            roughness->bind(Material::TextureSlots::Roughness);
        }
        if (hasAo) {
            shader.setInt("aoMap", Material::TextureSlots::AoMap);
            aoMap->bind(Material::TextureSlots::AoMap);
        }
    }

    void PbrFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (metalness) metalness->unbind(Material::TextureSlots::Metalness);
        if (roughness) roughness->unbind(Material::TextureSlots::Roughness);
        if (aoMap)     aoMap->unbind(Material::TextureSlots::AoMap);
    }

    std::shared_ptr<IMaterialFeature> PbrFeature::clone() const {
        return std::make_shared<PbrFeature>(metalness, roughness, aoMap);
    }
} // Feature
