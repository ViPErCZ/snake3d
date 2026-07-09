#include <snake3d/Renderer/Opengl/Material/Feature/ShadowFeature.h>

#include <snake3d/Renderer/Opengl/Material/TextureSlots.h>

namespace Feature {
    ShadowFeature::ShadowFeature(std::shared_ptr<Manager::TextureManager> shadowArray,
                                 std::shared_ptr<Manager::ShaderProgram> shadowDepthShader)
        : shadowArray(std::move(shadowArray)),
          shadowDepthShader(std::move(shadowDepthShader)) {
    }

    void ShadowFeature::bind(Manager::ShaderProgram& shader,
                             const Material::RenderContext& ctx) const {
        shader.setInt("shadowMap", Material::TextureSlots::ShadowArray);

        const bool active = receive && ctx.shadows && shadowArray && shadowArray->hasTexture();
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
        auto copy = std::make_shared<ShadowFeature>(shadowArray, shadowDepthShader);
        copy->receive = receive;
        copy->cast = cast;
        return copy;
    }

    bool ShadowFeature::bindShadow(const glm::mat4& model) const {
        if (!cast || !shadowDepthShader) {
            return false; // receive-only surfaces (e.g. the terrain) don't cast / self-shadow
        }
        shadowDepthShader->use();
        shadowDepthShader->setMat4("model", model);
        // Reset skinning to OFF for every caster. The depth program's `useBones`
        // uniform is sticky GL state: a skinned caster (AnimationArrayMesh) sets it
        // true and never clears it, so a non-skinned caster drawn afterwards (a rock
        // ArrayMesh, which has no bone attributes) would inherit useBones=true and get
        // skinned by garbage bone data -> its vertices collapse and it casts no usable
        // shadow. Skinned meshes re-set useBones=true in renderMesh after this bind.
        shadowDepthShader->setBool("useBones", false);
        return true;
    }
} // Feature
