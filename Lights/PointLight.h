#ifndef SNAKE3_POINTLIGHT_H
#define SNAKE3_POINTLIGHT_H

#include <glm/vec3.hpp>
#include "../ItemsDto/Transform.h"
#include "../Manager/ShaderManager.h"

using namespace Node3D;
using namespace Manager;

namespace Lights {
    class PointLight : public Transform {
        glm::vec3 ambient = {};
        glm::vec3 diffuse = {};
        glm::vec3 specular = {};
        float constant = 1.0f;
        float linear = 0.19f;
        float quadratic = 0.032f;

    public:
        [[nodiscard]] glm::vec3 getAmbient() const;

        void setAmbient(const glm::vec3 &ambient);

        [[nodiscard]] glm::vec3 getDiffuse() const;

        void setDiffuse(const glm::vec3 &diffuse);

        [[nodiscard]] glm::vec3 getSpecular() const;

        void setSpecular(const glm::vec3 &specular);

        [[nodiscard]] float getConstant() const;

        void setConstant(float constant);

        [[nodiscard]] float getLinear() const;

        void setLinear(float linear);

        [[nodiscard]] float getQuadratic() const;

        void setQuadratic(float quadratic);

        void bind(const ShaderManager *shader, int index) const;
    };
} // Light

#endif //SNAKE3_POINTLIGHT_H
