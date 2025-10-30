#include "Transform.h"

namespace Node3D {
    const glm::vec3 &Transform::getPosition() const {
        return position;
    }

    glm::mat4 Transform::getModelMatrix() const {
        auto model = glm::mat4(1.0f);

        model = glm::scale(model, zoom);
        model = glm::translate(model, position);

        if (rotate[0].w != 0.0f) model = glm::rotate(model, glm::radians(rotate[0].w), glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z));
        if (rotate[1].w != 0.0f) model = glm::rotate(model, glm::radians(rotate[1].w), glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z));
        if (rotate[2].w != 0.0f) model = glm::rotate(model, glm::radians(rotate[2].w), glm::vec3(rotate[2].x, rotate[2].y, rotate[2].z));

        return model;
    }

    void Transform::setPosition(const glm::vec3 &position) {
        Transform::position = position;
    }

    const glm::vec3 &Transform::getZoom() const {
        return zoom;
    }

    void Transform::setZoom(const glm::vec3 &zoom) {
        Transform::zoom = zoom;
    }

    const glm::vec4 *Transform::getRotate() const {
        return rotate;
    }

    void Transform::setRotate(const glm::vec4 &rotateX, const glm::vec4 &rotateY, const glm::vec4 &rotateZ) {
        rotate[0] = rotateX;
        rotate[1] = rotateY;
        rotate[2] = rotateZ;
    }
} // Node3D