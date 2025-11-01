#ifndef SNAKE3_TRANSFORM_H
#define SNAKE3_TRANSFORM_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Node3D {
    class Transform {
        public:
        Transform() = default;
        virtual ~Transform() = default;

        void setPosition(const glm::vec3 &position);
        [[nodiscard]] const glm::vec3 &getPosition() const;
        [[nodiscard]] virtual glm::mat4 getModelMatrix() const;
        [[nodiscard]] const glm::vec3 &getZoom() const;
        void setZoom(const glm::vec3 &zoom);
        [[nodiscard]] const glm::vec4 *getRotate() const;
        void setRotationX(float rotation_x);
        void setRotationY(float rotation_y);
        void setRotationZ(float rotation_z);
        [[nodiscard]] float getRotationX() const;
        [[nodiscard]] float getRotationY() const;
        [[nodiscard]] float getRotationZ() const;

    protected:
        glm::mat4 worldMatrix{};
        glm::vec3 position{};
        glm::vec3 zoom{1.0f, 1.0f, 1.0f};
        float rotationX{};
        float rotationY{};
        float rotationZ{};
    };
} // Node3D

#endif //SNAKE3_TRANSFORM_H