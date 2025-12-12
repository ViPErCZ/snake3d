#include "Transform.h"

namespace Node3D {

    const glm::vec3 &Transform::getPosition() const {
        return position;
    }

    glm::mat4 Transform::getModelMatrix() const {
        auto model = glm::mat4(1.0f);

        model = glm::scale(model, scale);
        model = glm::translate(model, position);

        model = glm::rotate(model, glm::radians(rotationX), {1.0, 0.0, 0.0});
        model = glm::rotate(model, glm::radians(rotationY), {0.0, 1.0, 0.0});
        model = glm::rotate(model, glm::radians(rotationZ), {0.0, 0.0, 1.0});

        return model;
    }

    void Transform::setPosition(const glm::vec3 &position) {
        Transform::position = position;
    }

    void Transform::setPosition(const shared_ptr<Transform> &object) {
        position = object->getPosition();
    }

    const glm::vec3 &Transform::getScale() const {
        return scale;
    }

    void Transform::setScale(const glm::vec3 &scale) {
        Transform::scale = scale;
    }

    void Transform::setRotationX(const float rotation_x) {
        rotationX = rotation_x;
    }

    void Transform::setRotationY(const float rotation_y) {
        rotationY = rotation_y;
    }

    void Transform::setRotationZ(const float rotation_z) {
        rotationZ = rotation_z;
    }

    float Transform::getRotationX() const {
        return rotationX;
    }

    float Transform::getRotationY() const {
        return rotationY;
    }

    float Transform::getRotationZ() const {
        return rotationZ;
    }
} // Node3D