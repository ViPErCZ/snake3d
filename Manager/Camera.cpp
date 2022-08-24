#include "Camera.h"

namespace Manager {
    Camera::Camera(glm::vec3 position, glm::vec3 up) : front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        this->position = position;
        worldUp = up;
        zoom = 20; //20
        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        // calculate the new Front vector
        glm::vec3 calculateFront;
        calculateFront.x = (float) (cos(glm::radians(YAW)) * cos(glm::radians(PITCH)));
        calculateFront.y = (float) sin(glm::radians(PITCH));
        calculateFront.z = (float) (sin(glm::radians(YAW)) * cos(glm::radians(PITCH)));
        front = glm::normalize(calculateFront);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    float Camera::getZoom() const {
        return zoom;
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    const glm::vec3 &Camera::getPosition() const {
        return position;
    }

    void Camera::setStickyPoint(BaseItem *stickyPoint) {
        this->stickyPoint = stickyPoint;
    }

    void Camera::updateStickyPoint() {
        glm::vec3 pos = stickyPoint->getPosition();
        position = pos;
        position.x /= 26;
        position.y /= 26;
        position.y -= 3;
        position.z = 1;
    }

}