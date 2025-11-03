#include "Camera.h"

#include <memory>

namespace Manager {
    Camera::Camera(glm::vec3 position, glm::vec3 up) : front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        this->position = position;
        worldUp = up;
        zoom = 28;
        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(PITCH)) * cos(glm::radians(YAW));
        newFront.y = cos(glm::radians(PITCH)) * sin(glm::radians(YAW));
        newFront.z = sin(glm::radians(PITCH));
        front = glm::normalize(newFront);

        constexpr glm::vec3 worldUp(0.0f, 0.0f, 1.0f); // Z nahoru
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    float Camera::getZoom() const {
        return zoom;
    }

    glm::mat4 Camera::getViewMatrix() {
        if (!rightButtonPressed) {
            // --- STANDARDNÍ MÓD ---
            // Kamera je fixována na 'stickyPoint' s daným offsetem
            const auto targetPos = glm::vec3(stickyPoint->getModelMatrix() * glm::vec4(0, 0, 0, 1));
            this->setPosition(targetPos + offsetFromTarget);
            return glm::lookAt(position, targetPos, worldUp);
        }

        // --- SPECTATOR MÓD ---
        // Kamera se volně pohybuje a dívá se, kam míří její 'front' vektor
        // Používáme 'position' jako volnou pozici kamery
        return glm::lookAt(position, position + front, up);
    }

    const glm::vec3 &Camera::getPosition() const {
        return position;
    }

    const glm::vec3 & Camera::getFront() const {
        return front;
    }

    glm::vec3 Camera::getUp() const {
        return up;
    }

    glm::vec3 Camera::getRight() const {
        return right;
    }

    void Camera::setStickyPoint(const shared_ptr<Transform> &stickyPoint) {
        this->stickyPoint = stickyPoint;

        const auto targetPos = glm::vec3(stickyPoint->getModelMatrix() * glm::vec4(0, 0, 0, 1));
        position = targetPos + offsetFromTarget;

        const glm::vec3 dirToTarget = glm::normalize(targetPos - position);

        // YAW: úhel v rovině X-Y
        YAW = glm::degrees(atan2(dirToTarget.y, dirToTarget.x));

        // PITCH: úhel vzhůru/dolů podle Z
        PITCH = glm::degrees(asin(dirToTarget.z));

        updateCameraVectors();
    }

    shared_ptr<Transform> Camera::getStickyPoint() const {
        return stickyPoint;
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

    void Camera::onMouseDown(const int button, const int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS && stickyPoint) {
                rightButtonPressed = true;
                firstMouse = true;

                // Uložíme si aktuální pozici kamery jako startovní bod pro spectator mód
                const auto targetPos = glm::vec3(stickyPoint->getModelMatrix() * glm::vec4(0, 0, 0, 1));
                position = targetPos + offsetFromTarget; // Nastavíme 'position' na aktuální vizuální pozici

                // Vypočítáme YAW a PITCH, aby přechod byl plynulý
                const glm::vec3 dirToTarget = glm::normalize(targetPos - position);
                YAW = glm::degrees(atan2(dirToTarget.z, dirToTarget.x));
                PITCH = glm::degrees(asin(dirToTarget.y));
                updateCameraVectors(); // Ihned aktualizujeme vektory
                cout << "Spectator started..." << endl;
            } else if (action == GLFW_RELEASE) {
                rightButtonPressed = false;
                // Není potřeba nic resetovat,getViewMatrix se postará o návrat na původní pozici
            }
        }
    }

    void Camera::processMouseMovement(const double x, const double y) {
        if (!rightButtonPressed) return;

        if (firstMouse) {
            lastX = static_cast<float>(x);
            lastY = static_cast<float>(y);
            firstMouse = false;
            return;
        }

        auto xoffset = static_cast<float>(x - lastX);
        auto yoffset = static_cast<float>(lastY - y);

        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);

        constexpr float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        YAW   -= xoffset;
        PITCH += yoffset;

        if (PITCH > 89.0f) PITCH = 89.0f;
        if (PITCH < -89.0f) PITCH = -89.0f;

        updateCameraVectors();
    }

    void Camera::setKeyState(const int key, const bool pressed) {
        if (key >= 0 && key < 1024) {
            keys[key] = pressed;
        }
    }

    void Camera::processKeyboard(GLFWwindow *window, const float deltaTime)
    {
        if (!rightButtonPressed) return;

        const float velocity = 0.04f * deltaTime;

        // Vytvoříme nulový vektor pohybu
        glm::vec3 moveDirection(0.0f);

        if (keys[GLFW_KEY_W]) {
            moveDirection += front; // Dopředu
        }
        if (keys[GLFW_KEY_S]) {
            moveDirection -= front; // Dozadu
        }
        if (keys[GLFW_KEY_A]) {
            moveDirection -= right; // Doleva
        }
        if (keys[GLFW_KEY_D]) {
            moveDirection += right; // Doprava
        }

        if (glm::length(moveDirection) > 0.0f) {
            position += glm::normalize(moveDirection) * velocity;
        }
    }
}
