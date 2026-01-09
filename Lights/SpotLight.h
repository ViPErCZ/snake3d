#ifndef SNAKE3_SPOTLIGHT_H
#define SNAKE3_SPOTLIGHT_H

#include "../ItemsDto/Transform.h"
#include "../Manager/ShaderManager.h"

using namespace Node3D;
using namespace Manager;

namespace Lights {
    class SpotLight : public Transform {
        glm::vec3 direction = {};
        glm::vec3 ambient = {};
        glm::vec3 diffuse = {};
        glm::vec3 specular = {};
        float constant = 1.0f;
        float linear = 0.19f;
        float quadratic = 0.032f;
        float cutOff = 0.0f;
        float outerCutOff = 0.0f;
        bool pulse = false;

    public:
        [[nodiscard]] glm::vec3 getDirection() const;

        void setDirection(const glm::vec3 &direction);

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

        [[nodiscard]] float setCutOff() const;

        void setCutOff(float cut_off);

        [[nodiscard]] float getOuterCutOff() const;

        void setOuterCutOff(float outer_cut_off);

        [[nodiscard]] bool isPulse() const;

        void setPulse(bool pulse);

        void bind(const ShaderManager *shader, int index) const;
    };
} // Lights

#endif //SNAKE3_SPOTLIGHT_H
