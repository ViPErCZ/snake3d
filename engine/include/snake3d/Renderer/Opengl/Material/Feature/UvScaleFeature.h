#ifndef SNAKE3_UVSCALEFEATURE_H
#define SNAKE3_UVSCALEFEATURE_H

#include <glm/glm.hpp>

#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>

namespace Feature {
    // Tiles a material's UVs by feeding `uUvScale` to the vertex shader (which is
    // expected to multiply its texture coords by it: `TexCoords = aTexCoords * uUvScale`).
    // Carries no shader-permutation bit (flag()==0, like AlbedoFeature) — it only
    // pushes the uniform each draw, so a 1m texture can repeat across a large face
    // instead of stretching one texel over the whole mesh.
    //
    // Setting the uniform on a program that doesn't declare `uUvScale` is a safe
    // no-op (GL ignores writes to location -1), so this is harmless on shaders that
    // don't opt in.
    class UvScaleFeature final : public IMaterialFeature {
    public:
        explicit UvScaleFeature(const glm::vec2 scale = glm::vec2(1.0f)) : scale(scale) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& /*ctx*/) const override {
            shader.setVec2("uUvScale", scale);
        }
        void unbind(Manager::ShaderProgram& /*shader*/) const override {}

        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<UvScaleFeature>(scale);
        }

        void setScale(const glm::vec2 s) { scale = s; }
        [[nodiscard]] glm::vec2 getScale() const { return scale; }

    private:
        glm::vec2 scale{1.0f};
    };
} // namespace Feature

#endif // SNAKE3_UVSCALEFEATURE_H
