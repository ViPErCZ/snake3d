#ifndef SNAKE3_LIGHT_H
#define SNAKE3_LIGHT_H

#include <glm/vec3.hpp>

#include "../Tools/Transform.h"

namespace Lights {
    class Light : public Node3D::Transform {
    public:

        [[nodiscard]] glm::vec3 getAmbient() const;

        void setAmbient(const glm::vec3 &ambient);

        [[nodiscard]] glm::vec3 getDiffuse() const;

        void setDiffuse(const glm::vec3 &diffuse);

        [[nodiscard]] glm::vec3 getSpecular() const;

        void setSpecular(const glm::vec3 &specular);

    protected:
        glm::vec3 ambient = {};
        glm::vec3 diffuse = {};
        glm::vec3 specular = {};
    };
} // Lights

#endif //SNAKE3_LIGHT_H
