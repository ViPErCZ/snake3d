#include "IblFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    IblFeature::IblFeature(std::shared_ptr<Manager::TextureManager> environmentMap)
        : environmentMap(std::move(environmentMap)) {
    }

    void IblFeature::bind(Manager::ShaderManager& shader,
                          const Material::RenderContext& /*ctx*/) const {
        const bool active = environmentMap && environmentMap->hasTexture();
        shader.setBool("iblEnabled", active);
        if (active) {
            shader.setInt("environmentMap", Material::TextureSlots::EnvironmentMap);
            environmentMap->cubeBind(Material::TextureSlots::EnvironmentMap);
        }
    }

    void IblFeature::unbind(Manager::ShaderManager& /*shader*/) const {
        if (environmentMap) {
            environmentMap->unbind(Material::TextureSlots::EnvironmentMap);
        }
    }

    std::shared_ptr<IMaterialFeature> IblFeature::clone() const {
        return std::make_shared<IblFeature>(environmentMap);
    }
} // Feature
