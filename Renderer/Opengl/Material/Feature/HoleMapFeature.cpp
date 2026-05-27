#include "HoleMapFeature.h"

#include "../TextureSlots.h"
#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    HoleMapFeature::HoleMapFeature(std::shared_ptr<Manager::TextureManager> holeMap)
        : holeMap(std::move(holeMap)) {
    }

    Manager::ShaderFeatureMask HoleMapFeature::flag() const {
        // C2b: žádný #ifdef gate v master shaderu - kód je v snippetu, který
        // se injektuje pouze když je feature přítomna v kompozici. Cache key
        // unikátnost zajišťuje snippet path v ShaderHandle::snippets.
        return 0;
    }

    void HoleMapFeature::bind(Manager::ShaderProgram& shader,
                              const Material::RenderContext& ctx) const {
        const bool active = holeMap && holeMap->hasTexture();
        // D1.2d.5: hasHoleMap migrated to MaterialData UBO
        // (material_hasHoleMap). The `holeMap` sampler stays legacy.
        if (ctx.materialData) {
            ctx.materialData->material_hasHoleMap = active ? 1 : 0;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }
        if (active) {
            shader.setInt("holeMap", Material::TextureSlots::HoleMap);
            holeMap->bind(Material::TextureSlots::HoleMap);
        }
    }

    void HoleMapFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (holeMap) {
            holeMap->unbind(Material::TextureSlots::HoleMap);
        }
    }

    std::shared_ptr<IMaterialFeature> HoleMapFeature::clone() const {
        // Textura je immutable shared resource - clone instance featury, ale
        // sdílí stejný TextureManager.
        return std::make_shared<HoleMapFeature>(holeMap);
    }
} // Feature
