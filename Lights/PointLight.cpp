#include "PointLight.h"

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

    void PointLight::bind(const ShaderManager *shader, const int index) const {
        shader->use();
        const string name = "pointLight[" + std::to_string(index) + "]";
        shader->setVec3(name + ".position", position);
        shader->setVec3(name + ".ambient", ambient);
        shader->setVec3(name + ".diffuse", diffuse);
        shader->setVec3(name + ".specular", specular);
        shader->setFloat(name + ".constant", constant);
        shader->setFloat(name + ".linear", linear);
        shader->setFloat(name + ".quadratic", quadratic);
    }
} // Light