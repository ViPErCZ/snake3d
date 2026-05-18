#ifndef SNAKE3_SHADOWFEATURE_H
#define SNAKE3_SHADOWFEATURE_H

#include <memory>

#include "IMaterialFeature.h"
#include "../../../../Manager/ShaderManager.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // CSM shadow array sampling. Drží sampler2DArray na slot 3 a shadowDepth
    // shader pro shadow-pass rendering. Runtime přepínač `shadowsEnable` se
    // navíc zkříží s globálním `RenderContext::shadows` - obojí musí být true
    // aby se v shaderu opravdu nasamploval shadow map. (Globální flag se
    // mapuje z BaseRenderer::shadows, který přepíná klávesa V.)
    class ShadowFeature final : public IMaterialFeature {
    public:
        explicit ShadowFeature(std::shared_ptr<Manager::TextureManager> shadowArray,
                               std::shared_ptr<Manager::ShaderManager> shadowDepthShader = nullptr);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::Shadows);
        }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderManager& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getShadowArray() const { return shadowArray; }
        [[nodiscard]] std::shared_ptr<Manager::ShaderManager> getShadowDepthShader() const { return shadowDepthShader; }

    private:
        std::shared_ptr<Manager::TextureManager> shadowArray;
        std::shared_ptr<Manager::ShaderManager> shadowDepthShader;
    };
} // Feature

#endif //SNAKE3_SHADOWFEATURE_H
