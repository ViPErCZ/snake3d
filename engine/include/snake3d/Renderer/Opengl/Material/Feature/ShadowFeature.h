#ifndef SNAKE3_SHADOWFEATURE_H
#define SNAKE3_SHADOWFEATURE_H

#include <memory>

#include <glm/glm.hpp>

#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>
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

        // Cast-but-don't-receive: when false, this surface still writes the depth pass
        // (casts onto others) but its own fragments never sample the shadow map (no
        // self-shadow). Useful for skinned characters whose depth pass uses 1-frame-stale
        // bones -> the misaligned self-shadow shows up as nonsensical acne banding.
        void setReceive(bool r) { receive = r; }
        [[nodiscard]] bool getReceive() const { return receive; }

        // Receive-but-don't-cast: when false, this surface is NOT written into the depth
        // map (bindShadow returns false), so it casts no shadow and - crucially - cannot
        // self-shadow. A tessellated ground that casts into a tight focus-box depth map
        // self-shadows into striped acne on some GPUs; making it receive-only kills that
        // while it still shows the shadows cast by rocks/units.
        void setCast(bool c) { cast = c; }
        [[nodiscard]] bool getCast() const { return cast; }

    private:
        std::shared_ptr<Manager::TextureManager> shadowArray;
        std::shared_ptr<Manager::ShaderProgram> shadowDepthShader;
        bool receive = true;
        bool cast = true;
    };
} // Feature

#endif //SNAKE3_SHADOWFEATURE_H
