#ifndef SNAKE3_POINTLIGHT_H
#define SNAKE3_POINTLIGHT_H

#include <snake3d/Lights/Light.h>

namespace Lights {
    // D1.1d: bind(ShaderProgram*, int) removed -- point lights are now
    // populated into MaterialData UBO directly by LightingFeature /
    // ShaderMaterial (see material_pointLights[] in MaterialUbo.h). The
    // light just exposes data via getters now.
    class PointLight : public Light {
        float constant = 1.0f;
        float linear = 0.19f;
        float quadratic = 0.032f;

    public:
        [[nodiscard]] float getConstant() const;

        void setConstant(float constant);

        [[nodiscard]] float getLinear() const;

        void setLinear(float linear);

        [[nodiscard]] float getQuadratic() const;

        void setQuadratic(float quadratic);
    };
} // Light

#endif //SNAKE3_POINTLIGHT_H
