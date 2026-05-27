#ifndef SNAKE3_RAINRIPPLEFEATURE_H
#define SNAKE3_RAINRIPPLEFEATURE_H

#include "IMaterialFeature.h"
#include "../../../../Manager/MaterialUbo.h"

using namespace Material;
using namespace Manager;
using namespace std;

namespace Feature {

    class RainRippleFeature : public IMaterialFeature {
    public:
        explicit RainRippleFeature(const bool enabled = true,
                                   const float speed = 0.2f,
                                   const float density = 20.0f)
            : enabled(enabled), speed(speed), density(density) {}

        [[nodiscard]] ShaderFeatureMask flag() const override {
            return static_cast<ShaderFeatureMask>(ShaderFeature::RainRipple);
        }

        void bind(ShaderProgram& /*shader*/, const RenderContext& ctx) const override {
            if (ctx.materialData) {
                ctx.materialData->material_rainDropEnable = enabled ? 1 : 0;
                ctx.materialData->material_rainSpeed = speed;
                ctx.materialData->material_rainDensity = density;
                if (ctx.materialDirty) *ctx.materialDirty = true;
            }
        }

        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<RainRippleFeature>(enabled, speed, density);
        }

        void setEnabled(const bool e) { enabled = e; }
        void setSpeed(const float s)  { speed = s; }
        void setDensity(const float d){ density = d; }

    private:
        bool enabled;
        float speed;
        float density;
    };
} // Feature

#endif //SNAKE3_RAINRIPPLEFEATURE_H
