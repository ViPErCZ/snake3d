#ifndef SNAKE3_HOLEMAPFEATURE_H
#define SNAKE3_HOLEMAPFEATURE_H

#include "IMaterialFeature.h"
#include "../../../../Manager/TextureManager.h"

namespace Feature {
    // První konkrétní feature - hole map discard. Drží jednu greyscale
    // texturu, binduje ji na TextureSlots::HoleMap a aktivuje uniform
    // hasHoleMap. Shader (basic.fs s FEATURE_HOLE_MAP) discardne fragment
    // pokud texture(holeMap, uv).r > 0.5.
    class HoleMapFeature final : public IMaterialFeature {
    public:
        explicit HoleMapFeature(std::shared_ptr<Manager::TextureManager> holeMap);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override;
        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderManager& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getTexture() const { return holeMap; }

    private:
        std::shared_ptr<Manager::TextureManager> holeMap;
    };
} // Feature

#endif //SNAKE3_HOLEMAPFEATURE_H
