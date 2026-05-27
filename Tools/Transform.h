#ifndef SNAKE3_TRANSFORM_H
#define SNAKE3_TRANSFORM_H

#include <memory>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Node3D {
    class Transform {
    public:
        Transform() = default;

        virtual ~Transform() = default;

        void setPosition(const glm::vec3 &position);

        void setPosition(const std::shared_ptr<Transform> &object);

        [[nodiscard]] const glm::vec3 &getPosition() const;

        [[nodiscard]] virtual glm::mat4 getModelMatrix() const;

        [[nodiscard]] const glm::vec3 &getScale() const;

        void setScale(const glm::vec3 &scale);

        void setRotationX(float rotation_x);

        void setRotationY(float rotation_y);

        void setRotationZ(float rotation_z);

        [[nodiscard]] float getRotationX() const;

        [[nodiscard]] float getRotationY() const;

        [[nodiscard]] float getRotationZ() const;

        void setTransform(const std::shared_ptr<Transform> &transform);

        // Dirty tag pro skip recompute v scene graph (computeWorldMatrix
        // short-circuit). Setters níže nastaví true; volající (MeshNode3D)
        // smaže po recompute. Static colliders (2300+ floor cells) ho pak
        // udrží false a celá ich subtree se přeskočí.
        [[nodiscard]] bool isTransformDirty() const { return transformDirty; }
        void clearTransformDirty() { transformDirty = false; }

    protected:
        glm::vec3 position{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        float rotationX = 0;
        float rotationY = 0;
        float rotationZ = 0;
        bool transformDirty = true;
    };
} // Node3D

#endif //SNAKE3_TRANSFORM_H
