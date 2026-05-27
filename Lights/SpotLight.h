#ifndef SNAKE3_SPOTLIGHT_H
#define SNAKE3_SPOTLIGHT_H

#include "OrientableLight.h"
#include "../Tools/Visibility.h"

using namespace Node3D;

namespace Lights {
    // D1.1d: bind(ShaderProgram*, int) removed -- spot lights are now
    // populated into MaterialData UBO directly by LightingFeature /
    // ShaderMaterial (see material_spotLights[] in MaterialUbo.h). The
    // light just exposes data via getters now.
    class SpotLight : public Visibility, public OrientableLight {
        float constant = 1.0f;
        float linear = 0.19f;
        float quadratic = 0.032f;
        float cutOff = 0.0f;
        float outerCutOff = 0.0f;
        bool pulse = false;

    public:
        [[nodiscard]] float getConstant() const;

        void setConstant(float constant);

        [[nodiscard]] float getLinear() const;

        void setLinear(float linear);

        [[nodiscard]] float getQuadratic() const;

        void setQuadratic(float quadratic);

        // D1.1d: was incorrectly named setCutOff() const (a getter shaped
        // like a setter -- return type float gave it away). Renamed to
        // getCutOff() so LightingFeature / ShaderMaterial can populate the
        // SpotLightStd140 cutOff field via a properly-named accessor.
        [[nodiscard]] float getCutOff() const;

        void setCutOff(float cut_off);

        [[nodiscard]] float getOuterCutOff() const;

        void setOuterCutOff(float outer_cut_off);

        [[nodiscard]] bool isPulse() const;

        void setPulse(bool pulse);
    };
} // Lights

#endif //SNAKE3_SPOTLIGHT_H
