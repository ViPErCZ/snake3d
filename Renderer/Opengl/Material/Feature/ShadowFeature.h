#ifndef SNAKE3_SHADOWFEATURE_H
#define SNAKE3_SHADOWFEATURE_H

#include <memory>

#include <glm/glm.hpp>

#include "IMaterialFeature.h"
#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Manager/TextureManager.h>

namespace Feature {
    // CSM shadow array sampling. Drží sampler2DArray na slot 3 a shadowDepth
    // shader pro shadow-pass rendering. Runtime přepínač `shadowsEnable` se
    // navíc zkříží s globálním `RenderContext::shadows` - obojí musí být true
    // aby se v shaderu opravdu nasamploval shadow map. (Globální flag se
    // mapuje z BaseRenderer::shadows, který přepíná klávesa V.)
    class ShadowFeature final : public IMaterialFeature {
    public:
        explicit ShadowFeature(std::shared_ptr<Manager::TextureManager> shadowArray,
                               std::shared_ptr<Manager::ShaderProgram> shadowDepthShader = nullptr);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::Shadows);
        }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        // Pro shadow-map pre-pass: aktivuje vlastní shadowDepthShader a zapíše
        // model matrix. Vrátí true pokud opravdu shadow shader bind. Volá z
        // MaterialInstance::bindShadow, který je hostován z StandardMesh::
        // renderShadowMap.
        bool bindShadow(const glm::mat4& model) const;

        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getShadowArray() const { return shadowArray; }
        [[nodiscard]] std::shared_ptr<Manager::ShaderProgram> getShadowDepthShader() const { return shadowDepthShader; }

    private:
        std::shared_ptr<Manager::TextureManager> shadowArray;
        std::shared_ptr<Manager::ShaderProgram> shadowDepthShader;
    };
} // Feature

#endif //SNAKE3_SHADOWFEATURE_H
