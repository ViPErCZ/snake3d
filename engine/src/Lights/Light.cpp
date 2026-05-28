#include <snake3d/Lights/Light.h>

namespace Lights {
    glm::vec3 Light::getAmbient() const {
        return ambient;
    }

    void Light::setAmbient(const glm::vec3 &ambient) {
        this->ambient = ambient;
    }

    glm::vec3 Light::getDiffuse() const {
        return diffuse;
    }

    void Light::setDiffuse(const glm::vec3 &diffuse) {
        this->diffuse = diffuse;
    }

    glm::vec3 Light::getSpecular() const {
        return specular;
    }

    void Light::setSpecular(const glm::vec3 &specular) {
        this->specular = specular;
    }
} // Lights