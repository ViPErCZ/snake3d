#include "../Tools/Transform.h"

using namespace std;

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
        transformDirty = true;
    }

    void Transform::setPosition(const shared_ptr<Transform> &object) {
        position = object->getPosition();
        transformDirty = true;
    }

    const glm::vec3 &Transform::getScale() const {
        return scale;
    }

    void Transform::setScale(const glm::vec3 &scale) {
        Transform::scale = scale;
        transformDirty = true;
    }

    void Transform::setRotationX(const float rotation_x) {
        rotationX = rotation_x;
        transformDirty = true;
    }

    void Transform::setRotationY(const float rotation_y) {
        rotationY = rotation_y;
        transformDirty = true;
    }

    void Transform::setRotationZ(const float rotation_z) {
        rotationZ = rotation_z;
        transformDirty = true;
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

    void Transform::setTransform(const shared_ptr<Transform> &transform) {
        this->position = transform->getPosition();
        this->scale = transform->getScale();
        this->rotationX = transform->getRotationX();
        this->rotationY = transform->getRotationY();
        this->rotationZ = transform->getRotationZ();
        transformDirty = true;
    }
} // Node3D