#include "Camera.h"

namespace Manager {
    Camera::Camera(glm::vec3 position, glm::vec3 up) : front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        this->position = position;
        worldUp = up;
        zoom = 28;
        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        // calculate the new Front vector
        glm::vec3 calculateFront;
        calculateFront.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
        calculateFront.y = sin(glm::radians(PITCH));
        calculateFront.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
        front = glm::normalize(calculateFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    float Camera::getZoom() const {
        return zoom;
    }

    glm::mat4 Camera::getViewMatrix() const {
        const glm::vec3 target = stickyPoint->getPosition() / 21.0f;
        const glm::vec3 cameraPos = target + glm::vec3(0.0f, -3.5f, 3.0f);

        return glm::lookAt(cameraPos, target, glm::vec3(0, 0, 1));
    }

    const glm::vec3 &Camera::getPosition() const {
        return position;
    }

    const glm::vec3 & Camera::getFront() const {
        return front;
    }

    void Camera::setStickyPoint(BaseItem *stickyPoint) {
        this->stickyPoint = stickyPoint;
    }

    glm::vec3 Camera::getStickyPosition() const {
        return stickyPoint->getPosition();
    }

    void Camera::setPosition(const glm::vec3& pos) {
        position = pos;
    }

    void Camera::setFront(const glm::vec3& front) {
        this->front = glm::normalize(front);
    }

    void Camera::setUp(const glm::vec3& up) {
        this->up = up;
    }

    void Camera::processMouseMovement(double x, double y) {
        x *= 0.1;
        y *= 0.1;

        YAW   += static_cast<float>(x);
        PITCH += static_cast<float>(y);

        updateCameraVectors();
    }

    void Camera::processKeyboard(const Camera_Movement direction, const float deltaTime)
    {
        const float velocity = 0.1f * deltaTime;

        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
    }

}