#include <snake3d/Lights/PointLight.h>

namespace Lights {
    float PointLight::getConstant() const {
        return constant;
    }

    void PointLight::setConstant(const float constant) {
        this->constant = constant;
    }

    float PointLight::getLinear() const {
        return linear;
    }

    void PointLight::setLinear(const float linear) {
        this->linear = linear;
    }

    float PointLight::getQuadratic() const {
        return quadratic;
    }

    void PointLight::setQuadratic(const float quadratic) {
        this->quadratic = quadratic;
    }
} // Light