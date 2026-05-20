#ifndef SNAKE3_PLANARREFLECTIONFEATURE_H
#define SNAKE3_PLANARREFLECTIONFEATURE_H

#include <memory>

#include <glm/glm.hpp>

#include "IMaterialFeature.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // Planar reflection (mirror surface) - binduje pre-rendered reflection
    // texture na slot 20 a aktivuje `reflectionEnable` runtime. Shader v
    // `if (reflectionEnable)` smíchá výslednou barvu s reflektovaným pohledem.
    // clipPlane parametr je pro reflection rendering pass (StandardMaterial
    // dnes nastavuje (0, 0, 1, 1000) = no-clip pro main pass).
    //
    // Žádný shader feature flag (basic.fs zatím má `if (reflectionEnable)` bez
    // #ifdef wrap). V B8 se reflection převede na snippet a tato feature
    // přestane uniformovat - bude jen registrovat snippet do builderu.
    class PlanarReflectionFeature final : public IMaterialFeature {
    public:
        explicit PlanarReflectionFeature(std::shared_ptr<Manager::TextureManager> reflection,
                                         bool enabled = true,
                                         glm::vec4 clipPlane = glm::vec4(0, 0, 1, 1000.0f));

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        [[nodiscard]] std::map<Manager::MaterialSlot, std::string> snippetPaths() const override {
            return {{Manager::MaterialSlot::FragmentPost, "Assets/Shaders/snippets/planar_reflection.glsl"}};
        }

        void setEnabled(const bool e) { enabled = e; }
        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getTexture() const { return reflection; }

    private:
        std::shared_ptr<Manager::TextureManager> reflection;
        bool enabled;
        glm::vec4 clipPlane;
    };
} // Feature

#endif //SNAKE3_PLANARREFLECTIONFEATURE_H
