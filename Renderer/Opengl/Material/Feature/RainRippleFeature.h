#ifndef SNAKE3_RAINRIPPLEFEATURE_H
#define SNAKE3_RAINRIPPLEFEATURE_H

#include "IMaterialFeature.h"

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

        void bind(ShaderProgram& shader, const RenderContext& /*ctx*/) const override {
            shader.setBool("rainDropEnable", enabled);
            shader.setFloat("rainSpeed", speed);
            shader.setFloat("rainDensity", density);
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
