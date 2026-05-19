#include "ShadowFeature.h"

#include "../TextureSlots.h"

namespace Feature {
    ShadowFeature::ShadowFeature(std::shared_ptr<Manager::TextureManager> shadowArray,
                                 std::shared_ptr<Manager::ShaderProgram> shadowDepthShader)
        : shadowArray(std::move(shadowArray)),
          shadowDepthShader(std::move(shadowDepthShader)) {
    }

    void ShadowFeature::bind(Manager::ShaderProgram& shader,
                             const Material::RenderContext& ctx) const {
        shader.setInt("shadowMap", Material::TextureSlots::ShadowArray);

        const bool active = ctx.shadows && shadowArray && shadowArray->hasTexture();
        shader.setBool("shadowsEnable", active);
        if (active) {
            shadowArray->bindArr(Material::TextureSlots::ShadowArray, 0);
        }
    }

    void ShadowFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (shadowArray) {
            shadowArray->unbind(Material::TextureSlots::ShadowArray);
        }
    }

    std::shared_ptr<IMaterialFeature> ShadowFeature::clone() const {
        return std::make_shared<ShadowFeature>(shadowArray, shadowDepthShader);
    }

    bool ShadowFeature::bindShadow(const glm::mat4& model) const {
        if (!shadowDepthShader) {
            return false;
        }
        shadowDepthShader->use();
        shadowDepthShader->setMat4("model", model);
        return true;
    }
} // Feature
