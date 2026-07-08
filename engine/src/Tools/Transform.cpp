#include <snake3d/Tools/Transform.h>

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

namespace Node3D {

    const glm::vec3 &Transform::getPosition() const {
        return position;
    }

    glm::mat4 Transform::getModelMatrix() const {
        auto model = glm::mat4(1.0f);

        model = glm::scale(model, scale);
        model = glm::translate(model, position);

        if (useQuat) {
            // E1: quaternion orientation (same S*T*R slot as the Euler rotations).
            model = model * glm::mat4_cast(orientation);
        } else {
            model = glm::rotate(model, glm::radians(rotationX), {1.0, 0.0, 0.0});
            model = glm::rotate(model, glm::radians(rotationY), {0.0, 1.0, 0.0});
            model = glm::rotate(model, glm::radians(rotationZ), {0.0, 0.0, 1.0});
        }

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
        useQuat = false; // explicit Euler reverts the quaternion path
        transformDirty = true;
    }

    void Transform::setRotationY(const float rotation_y) {
        rotationY = rotation_y;
        useQuat = false;
        transformDirty = true;
    }

    void Transform::setRotationZ(const float rotation_z) {
        rotationZ = rotation_z;
        useQuat = false;
        transformDirty = true;
    }

    void Transform::setRotationQuat(const glm::quat &q) {
        orientation = q;
        useQuat = true;
        transformDirty = true;
    }

    void Transform::setRotationToNormal(const glm::vec3 &normal) {
        const float len = glm::length(normal);
        if (len < 1e-6f) { setRotationQuat(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)); return; }
        const glm::vec3 to = normal / len;
        const glm::vec3 from(0.0f, 1.0f, 0.0f);            // QuadMesh3D face normal = local +Y
        const float d = glm::dot(from, to);
        glm::quat q;
        if (d >= 1.0f - 1e-6f) {
            q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);         // already aligned
        } else if (d <= -1.0f + 1e-6f) {
            q = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);         // 180deg about Z (any perp axis)
        } else {
            const glm::vec3 axis = glm::normalize(glm::cross(from, to));
            const float angle = std::acos(d);
            const float s = std::sin(angle * 0.5f);
            q = glm::quat(std::cos(angle * 0.5f), axis.x * s, axis.y * s, axis.z * s);
        }
        setRotationQuat(q);
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
        this->orientation = transform->orientation;
        this->useQuat = transform->useQuat;
        transformDirty = true;
    }
} // Node3D