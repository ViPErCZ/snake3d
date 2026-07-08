#ifndef SNAKE3_EMISSIVEFEATURE_H
#define SNAKE3_EMISSIVEFEATURE_H

#include <memory>

#include <glm/glm.hpp>

#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>

namespace Feature {
    // Self-illumination: přidá emissive color * intensity k final fragment color
    // PO všech light contributions. Použito pro objekty co svítí samy (collectables,
    // UI markery, magic effects, atd.).
    //
    // PBR poznámka: emissive nemá fyzikální opodstatnění -- skutečné materiály
    // pohlcují a odrážejí, neemitují (kromě LED / atd.). Funkčně užitečné pro:
    // - viditelnost objektu i v dark scenes bez direct light
    // - art-direction "glow" effect (kombinovat s BloomRenderer threshold)
    // - debug visualisation
    //
    // MVP: jen color + intensity, žádná texture. Emissive texture support až
    // pokud bude reálná potřeba (future H8+).
    class EmissiveFeature final : public IMaterialFeature {
    public:
        explicit EmissiveFeature(const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

        // EmissiveBloom shader flag (bit 10). Aktivuje `#define FEATURE_EMISSIVE_BLOOM`
        // v basic.fs -- bez něj shader emissive branch neexistuje (zero overhead pro
        // materiály bez této feature).
        [[nodiscard]] Manager::ShaderFeatureMask flag() const override;

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setColor(const glm::vec3& c) { color = c; }
        void setIntensity(const float i) { intensity = i; }

        [[nodiscard]] const glm::vec3& getColor() const { return color; }
        [[nodiscard]] float getIntensity() const { return intensity; }

    private:
        glm::vec3 color{1.0f};
        float intensity = 1.0f;
    };
} // Feature

#endif //SNAKE3_EMISSIVEFEATURE_H
