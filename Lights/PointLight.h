#ifndef SNAKE3_POINTLIGHT_H
#define SNAKE3_POINTLIGHT_H

#include "Light.h"
#include "../ItemsDto/Visibility.h"
#include "../Manager/ShaderManager.h"

using namespace Node3D;
using namespace Manager;

namespace Lights {
    class PointLight : public Visibility, public Light {
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

        void bind(const ShaderManager *shader, int index) const;
    };
} // Light

#endif //SNAKE3_POINTLIGHT_H
