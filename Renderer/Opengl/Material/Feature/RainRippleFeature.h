#ifndef SNAKE3_RAINRIPPLEFEATURE_H
#define SNAKE3_RAINRIPPLEFEATURE_H

#include "IMaterialFeature.h"

namespace Feature {
    // Rain ripple distortion: a procedural ripple offset perturbs the
    // tangent-space normal (and the planar reflection sampling position
    // via the reflection snippet). Activates basic.fs FEATURE_RAIN_RIPPLE
    // blocks and writes the rainDropEnable / rainSpeed / rainDensity
    // uniforms at bind time.
    //
    // Dormant in B8c: no material composition currently uses this feature
    // (the rainDropEnable uniform was never wired from C++). The class is
    // here so a future weather system can just add it to the plane's
    // builder chain.
    class RainRippleFeature final : public IMaterialFeature {
    public:
        explicit RainRippleFeature(bool enabled = true,
                                   float speed = 0.2f,
                                   float density = 20.0f)
            : enabled(enabled), speed(speed), density(density) {}

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override {
            return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::RainRipple);
        }

        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& /*ctx*/) const override {
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
