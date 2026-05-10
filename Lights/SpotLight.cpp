#include "SpotLight.h"

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

    float SpotLight::setCutOff() const {
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

    void SpotLight::bind(const ShaderManager *shader, const int index = 0) const {
        shader->use();
        const string name = "spotLight[" + std::to_string(index) + "]";
        shader->setVec3(name + ".position", position);
        shader->setVec3(name + ".direction",glm::normalize(getDirection() - position));
        shader->setVec3(name + ".ambient", ambient);
        shader->setVec3(name + ".diffuse", diffuse);
        shader->setVec3(name + ".specular", specular);
        shader->setFloat(name + ".constant", constant);
        shader->setFloat(name + ".linear", linear);
        shader->setFloat(name + ".quadratic", quadratic);
        shader->setBool(name + ".pulse", pulse);
        shader->setFloat(name + ".cutOff", glm::cos(glm::radians(cutOff)));
        shader->setFloat(name + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));
    }
} // Lights
