#include "DirectionalLight.h"

namespace Lights {
    glm::vec3 DirectionalLight::getDirection() const {
        return direction;
    }

    void DirectionalLight::setDirection(const glm::vec3 &direction) {
        this->direction = direction;
    }

    glm::vec3 DirectionalLight::getAmbient() const {
        return ambient;
    }

    void DirectionalLight::setAmbient(const glm::vec3 &ambient) {
        this->ambient = ambient;
    }

    glm::vec3 DirectionalLight::getDiffuse() const {
        return diffuse;
    }

    void DirectionalLight::setDiffuse(const glm::vec3 &diffuse) {
        this->diffuse = diffuse;
    }

    glm::vec3 DirectionalLight::getSpecular() const {
        return specular;
    }

    void DirectionalLight::setSpecular(const glm::vec3 &specular) {
        this->specular = specular;
    }

    void DirectionalLight::bind(const ShaderManager *shader) const {
        // directional light
        shader->use();
        shader->setVec3("lightPos", position);
        shader->setVec3("dirLight.direction", direction);
        shader->setVec3("dirLight.ambient", ambient);
        shader->setVec3("dirLight.diffuse", diffuse);
        shader->setVec3("dirLight.specular", specular);
        shader->setFloat("material.shininess", shininess);
        shader->setInt("material.ambient", 0);
        shader->setInt("material.diffuse", 1);
        shader->setInt("material.specular", 2);
    }
} // Lights
