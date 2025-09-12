#ifndef SNAKE3_TRANSFORM_H
#define SNAKE3_TRANSFORM_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Node3D {
    class Transform {
        public:
        void setPosition(const glm::vec3 &position);
        void setWorldMatrix(const glm::mat4 &matrix);
        [[nodiscard]] const glm::vec3 &getPosition() const;
        [[nodiscard]] glm::mat4 getModelMatrix() const;
        [[nodiscard]] const glm::vec3 &getZoom() const;
        void setZoom(const glm::vec3 &zoom);
        [[nodiscard]] const glm::vec4 *getRotate() const;
        void setRotate(const glm::vec4 &rotateX, const glm::vec4 &rotateY, const glm::vec4 &rotateZ);
        protected:
        glm::mat4 worldMatrix{};
        glm::vec3 position{};
        glm::vec3 zoom{1.0f, 1.0f, 1.0f};
        glm::vec4 rotate[3]{};
    };
} // Node3D

#endif //SNAKE3_TRANSFORM_H