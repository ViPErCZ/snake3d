#ifndef SNAKE3_UVTRANSFORMFEATURE_H
#define SNAKE3_UVTRANSFORMFEATURE_H

#include <glm/glm.hpp>

#include "IMaterialFeature.h"

namespace Feature {
    // UV scale + offset pro tiling textur. Plane používá scale (48, 48) aby
    // se gamefield texture opakovala přes celou podlahu; default (1, 1) /
    // (0, 0) je no-op. Žádný shader feature flag - uvScale/uvOffset uniformy
    // jsou v basic.vs vždy přítomny.
    class UvTransformFeature final : public IMaterialFeature {
    public:
        explicit UvTransformFeature(glm::vec2 scale = glm::vec2(1.0f),
                                    glm::vec2 offset = glm::vec2(0.0f))
            : scale(scale), offset(offset) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& /*ctx*/) const override {
            shader.setVec2("uvScale", scale);
            shader.setVec2("uvOffset", offset);
        }

        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<UvTransformFeature>(scale, offset);
        }

        void setScale(const glm::vec2& s) { scale = s; }
        void setOffset(const glm::vec2& o) { offset = o; }
        [[nodiscard]] glm::vec2 getScale() const { return scale; }
        [[nodiscard]] glm::vec2 getOffset() const { return offset; }

    private:
        glm::vec2 scale;
        glm::vec2 offset;
    };
} // Feature

#endif //SNAKE3_UVTRANSFORMFEATURE_H
