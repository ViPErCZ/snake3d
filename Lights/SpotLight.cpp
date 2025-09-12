#include "SpotLight.h"

namespace Lights {
    glm::vec3 SpotLight::getDirection() const {
        return direction;
    }

    void SpotLight::setDirection(const glm::vec3 &direction) {
        this->direction = direction;
    }

    glm::vec3 SpotLight::getAmbient() const {
        return ambient;
    }

    void SpotLight::setAmbient(const glm::vec3 &ambient) {
        this->ambient = ambient;
    }

    glm::vec3 SpotLight::getDiffuse() const {
        return diffuse;
    }

    void SpotLight::setDiffuse(const glm::vec3 &diffuse) {
        this->diffuse = diffuse;
    }

    glm::vec3 SpotLight::getSpecular() const {
        return specular;
    }

    void SpotLight::setSpecular(const glm::vec3 &specular) {
        this->specular = specular;
    }

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

    void SpotLight::bind(const ShaderManager *shader, const int index = 0) const {
        shader->use();
        const string name = "spotLight[" + std::to_string(index) + "]";
        shader->setVec3(name + ".position", position);
        shader->setVec3(name + ".direction",glm::normalize(direction - position));
        shader->setVec3(name + ".ambient", ambient);
        shader->setVec3(name + ".diffuse", diffuse);
        shader->setVec3(name + ".specular", specular);
        shader->setFloat(name + ".constant", constant);
        shader->setFloat(name + ".linear", linear);
        shader->setFloat(name + ".quadratic", quadratic);
        shader->setFloat(name + ".cutOff", glm::cos(glm::radians(cutOff)));
        shader->setFloat(name + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));
    }
} // Lights
