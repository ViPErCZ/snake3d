#include <snake3d/Lights/SpotLight.h>

namespace Lights {
    float SpotLight::getConstant() const {
        return constant;
    }

    void SpotLight::setConstant(const float constant) {
        this->constant = constant;
    }

    float SpotLight::getLinear() const {
        return linear;
    }

    void SpotLight::setLinear(const float linear) {
        this->linear = linear;
    }

    float SpotLight::getQuadratic() const {
        return quadratic;
    }

    void SpotLight::setQuadratic(const float quadratic) {
        this->quadratic = quadratic;
    }

    float SpotLight::getCutOff() const {
        return cutOff;
    }

    void SpotLight::setCutOff(const float cut_off) {
        cutOff = cut_off;
    }

    float SpotLight::getOuterCutOff() const {
        return outerCutOff;
    }

    void SpotLight::setOuterCutOff(const float outer_cut_off) {
        outerCutOff = outer_cut_off;
    }

    bool SpotLight::isPulse() const {
        return pulse;
    }

    void SpotLight::setPulse(const bool pulse) {
        this->pulse = pulse;
    }
} // Lights
