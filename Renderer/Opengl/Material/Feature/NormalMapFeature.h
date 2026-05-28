#ifndef SNAKE3_NORMALMAPFEATURE_H
#define SNAKE3_NORMALMAPFEATURE_H

#include <memory>

#include "IMaterialFeature.h"
#include <snake3d/Manager/TextureManager.h>

namespace Feature {
    // Tangent-space normal map. Binduje texture na slot 1 (alias material.diffuse
    // v shader uniformu) a aktivuje normalMapEnabled. Shader pak v rámci
    // `#ifdef FEATURE_NORMAL_MAP` přepočítá normálu pomocí TBN matice.
    class NormalMapFeature final : public IMaterialFeature {
    public:
        explicit NormalMapFeature(std::shared_ptr<Manager::TextureManager> normal);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::NormalMap);
        }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setTexture(std::shared_ptr<Manager::TextureManager> texture) { normal = std::move(texture); }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getTexture() const { return normal; }

    private:
        std::shared_ptr<Manager::TextureManager> normal;
    };
} // Feature

#endif //SNAKE3_NORMALMAPFEATURE_H
