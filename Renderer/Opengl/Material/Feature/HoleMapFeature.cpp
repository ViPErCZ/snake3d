#include "HoleMapFeature.h"

#include "../TextureSlots.h"

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
                              const Material::RenderContext& /*ctx*/) const {
        if (holeMap && holeMap->hasTexture()) {
            shader.setBool("hasHoleMap", true);
            shader.setInt("holeMap", Material::TextureSlots::HoleMap);
            holeMap->bind(Material::TextureSlots::HoleMap);
        } else {
            shader.setBool("hasHoleMap", false);
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
