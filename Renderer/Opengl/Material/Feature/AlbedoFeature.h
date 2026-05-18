#ifndef SNAKE3_ALBEDOFEATURE_H
#define SNAKE3_ALBEDOFEATURE_H

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include "IMaterialFeature.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // Baseline material color state: albedo texture (slot 0 = material.ambient),
    // optional uniform color override, ambient light parameters and alpha. Mirrors
    // the "color related" half of StandardMaterial::bind so a builder-built plane
    // looks identical to PlaneMaterial.
    //
    // Logika: pokud má texturu, useMaterial=false (shader bere z material.ambient
    // sampleru) a hasAlbedoTexture=true. Pokud nemá, useMaterial=true a barva se
    // určuje z mesh vertex color nebo color overrideu. Když je color set, taky se
    // overrideColorMesh=true a ambientLightColor = color.
    class AlbedoFeature final : public IMaterialFeature {
    public:
        explicit AlbedoFeature(std::shared_ptr<Manager::TextureManager> albedo = nullptr);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderManager& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setAlbedo(std::shared_ptr<Manager::TextureManager> a) { albedo = std::move(a); }
        void setColor(const glm::vec3& c) { color = c; }
        void clearColor() { color.reset(); }
        void setAlpha(const float a) { alpha = a; }
        void setAmbientIntensity(const float i) { ambientIntensity = i; }

        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getAlbedo() const { return albedo; }
        [[nodiscard]] std::optional<glm::vec3> getColor() const { return color; }
        [[nodiscard]] float getAlpha() const { return alpha; }
        [[nodiscard]] float getAmbientIntensity() const { return ambientIntensity; }

    private:
        std::shared_ptr<Manager::TextureManager> albedo;
        std::optional<glm::vec3> color;
        float alpha = 1.0f;
        float ambientIntensity = 0.05f;
    };
} // Feature

#endif //SNAKE3_ALBEDOFEATURE_H
