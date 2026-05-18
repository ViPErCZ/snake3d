#include "ShadowFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    ShadowFeature::ShadowFeature(std::shared_ptr<Manager::TextureManager> shadowArray,
                                 std::shared_ptr<Manager::ShaderManager> shadowDepthShader)
        : shadowArray(std::move(shadowArray)),
          shadowDepthShader(std::move(shadowDepthShader)) {
    }

    void ShadowFeature::bind(Manager::ShaderManager& shader,
                             const Material::RenderContext& ctx) const {
        shader.setInt("shadowMap", Material::TextureSlots::ShadowArray);

        const bool active = ctx.shadows && shadowArray && shadowArray->hasTexture();
        shader.setBool("shadowsEnable", active);
        if (active) {
            shadowArray->bindArr(Material::TextureSlots::ShadowArray, 0);
        }
    }

    void ShadowFeature::unbind(Manager::ShaderManager& /*shader*/) const {
        if (shadowArray) {
            shadowArray->unbind(Material::TextureSlots::ShadowArray);
        }
    }

    std::shared_ptr<IMaterialFeature> ShadowFeature::clone() const {
        return std::make_shared<ShadowFeature>(shadowArray, shadowDepthShader);
    }
} // Feature
