#ifndef SNAKE3_DIRECTIONALLIGHT_H
#define SNAKE3_DIRECTIONALLIGHT_H

#include <glm/vec3.hpp>
#include "../ItemsDto/Transform.h"
#include "../Manager/ShaderManager.h"

using namespace Node3D;
using namespace Manager;

namespace Lights {
    class DirectionalLight : public Transform {
        glm::vec3 direction = {};
        glm::vec3 ambient = {};
        glm::vec3 diffuse = {};
        glm::vec3 specular = {};
        float shininess = 32.0f;

    public:
        [[nodiscard]] glm::vec3 getDirection() const;

        void setDirection(const glm::vec3 &direction);

        [[nodiscard]] glm::vec3 getAmbient() const;

        void setAmbient(const glm::vec3 &ambient);

        [[nodiscard]] glm::vec3 getDiffuse() const;

        void setDiffuse(const glm::vec3 &diffuse);

        [[nodiscard]] glm::vec3 getSpecular() const;

        void setSpecular(const glm::vec3 &specular);

        void bind(const ShaderManager *shader) const;
    };
} // Lights

#endif //SNAKE3_DIRECTIONALLIGHT_H
