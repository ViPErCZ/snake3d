#ifndef SNAKE3_IBLFEATURE_H
#define SNAKE3_IBLFEATURE_H

#include <memory>

#include "IMaterialFeature.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // Image-based lighting: environment cubemap (slot 6) drives diffuse
    // irradiance + specular reflections in the PBR path. Without this feature,
    // ambient falls back to ambientLightColor * 0.1 (see basic.fs IBL branch).
    // Typically paired with PbrFeature, but compositionally independent.
    class IblFeature final : public IMaterialFeature {
    public:
        explicit IblFeature(std::shared_ptr<Manager::TextureManager> environmentMap);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::IBL);
        }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderManager& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setEnvironmentMap(std::shared_ptr<Manager::TextureManager> t) { environmentMap = std::move(t); }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getEnvironmentMap() const { return environmentMap; }

    private:
        std::shared_ptr<Manager::TextureManager> environmentMap;
    };
} // Feature

#endif //SNAKE3_IBLFEATURE_H
