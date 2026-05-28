#ifndef SNAKE3_UVTRANSFORMFEATURE_H
#define SNAKE3_UVTRANSFORMFEATURE_H

#include <glm/glm.hpp>

#include "IMaterialFeature.h"
#include <snake3d/Manager/MaterialUbo.h>

namespace Feature {
        class UvTransformFeature final : public IMaterialFeature {
    public:
        explicit UvTransformFeature(const glm::vec2 scale = glm::vec2(1.0f),
                                    const glm::vec2 offset = glm::vec2(0.0f))
            : scale(scale), offset(offset) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderProgram& /*shader*/,
                  const Material::RenderContext& ctx) const override {
            if (ctx.materialData) {
                ctx.materialData->material_uvScale = scale;
                ctx.materialData->material_uvOffset = offset;
                if (ctx.materialDirty) *ctx.materialDirty = true;
            }
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
