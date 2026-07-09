#ifndef SNAKE3_SPECULARFEATURE_H
#define SNAKE3_SPECULARFEATURE_H

#include <memory>

#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>
#include <snake3d/Manager/TextureManager.h>

namespace Feature {
    // Specular highlight map - binduje na slot 2 a aktivuje specularMapEnabled.
    // Žádný shader feature flag (basic.fs/lights.glsl řeší přes runtime
    // specularMapEnabled). Doplněk k Phong / non-PBR shading - PBR větev ji
    // ignoruje (používá metalness/roughness místo specular textury).
    class SpecularFeature final : public IMaterialFeature {
    public:
        explicit SpecularFeature(std::shared_ptr<Manager::TextureManager> specular,
                                 float shininess = 32.0f);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setShininess(const float s) { shininess = s; }

    private:
        std::shared_ptr<Manager::TextureManager> specular;
        float shininess;
    };
} // Feature

#endif //SNAKE3_SPECULARFEATURE_H
