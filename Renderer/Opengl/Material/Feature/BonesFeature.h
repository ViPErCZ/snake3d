#ifndef SNAKE3_BONESFEATURE_H
#define SNAKE3_BONESFEATURE_H

#include "IMaterialFeature.h"

namespace Feature {
    // Skeletal animation toggle. Drives the `if (useBones)` branch in
    // basic.vs (FEATURE_BONES). Bone matrices themselves are uploaded
    // separately by AnimationArrayMesh::render directly to the program
    // via setMat4("finalBonesMatrices[i]", ...) - this feature only
    // mirrors the runtime `useBones` flag because the matrix data is a
    // per-frame stream that doesn't fit the static feature shape.
    class BonesFeature final : public IMaterialFeature {
    public:
        explicit BonesFeature(bool useBones = false) : useBones(useBones) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::Bones);
        }

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& /*ctx*/) const override {
            shader.setBool("useBones", useBones);
        }

        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<BonesFeature>(useBones);
        }

        void setUseBones(const bool u) { useBones = u; }
        [[nodiscard]] bool getUseBones() const { return useBones; }

    private:
        bool useBones;
    };
} // Feature

#endif //SNAKE3_BONESFEATURE_H
