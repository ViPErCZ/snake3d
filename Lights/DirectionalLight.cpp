#include "DirectionalLight.h"

namespace Lights {
    void DirectionalLight::bind(const ShaderManager *shader) const {
        // directional light
        shader->use();
        shader->setVec3("lightPos", position);
        shader->setVec3("dirLight.direction", getDirection());
        shader->setVec3("dirLight.ambient", ambient);
        shader->setVec3("dirLight.diffuse", diffuse);
        shader->setVec3("dirLight.specular", specular);
        shader->setFloat("material.shininess", shininess);
        shader->setInt("material.ambient", 0);
        shader->setInt("material.diffuse", 1);
        shader->setInt("material.specular", 2);
    }
} // Lights
