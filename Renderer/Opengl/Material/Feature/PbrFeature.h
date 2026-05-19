#ifndef SNAKE3_PBRFEATURE_H
#define SNAKE3_PBRFEATURE_H

#include <memory>

#include "IMaterialFeature.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // Metallic-roughness PBR inputs. Binds metalness (slot 4), roughness
    // (slot 5) and ambient occlusion (slot 7) textures. Activates pbrEnabled
    // runtime so the Cook-Torrance path in basic.fs runs instead of Phong.
    // IBL diffuse + specular env reflections are a sibling IblFeature (kept
    // separate so a material can opt into PBR without forcing an env cubemap).
    class PbrFeature final : public IMaterialFeature {
    public:
        explicit PbrFeature(std::shared_ptr<Manager::TextureManager> metalness,
                            std::shared_ptr<Manager::TextureManager> roughness,
                            std::shared_ptr<Manager::TextureManager> aoMap = nullptr);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::PBR);
        }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setMetalness(std::shared_ptr<Manager::TextureManager> t) { metalness = std::move(t); }
        void setRoughness(std::shared_ptr<Manager::TextureManager> t) { roughness = std::move(t); }
        void setAoMap(std::shared_ptr<Manager::TextureManager> t) { aoMap = std::move(t); }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getMetalness() const { return metalness; }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getRoughness() const { return roughness; }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getAoMap() const { return aoMap; }

    private:
        std::shared_ptr<Manager::TextureManager> metalness;
        std::shared_ptr<Manager::TextureManager> roughness;
        std::shared_ptr<Manager::TextureManager> aoMap;
    };
} // Feature

#endif //SNAKE3_PBRFEATURE_H
