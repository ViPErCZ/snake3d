#ifndef SNAKE3_CAMERA_H
#define SNAKE3_CAMERA_H

#include <memory>
#include <glm/glm.hpp>

#include <stdafx.h>
#include <snake3d/Tools/Transform.h>

namespace Manager {

    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class Camera {
    public:
        explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
        [[nodiscard]] float getZoom() const;
        [[nodiscard]] glm::mat4 getViewMatrix();

        // Project a world point to a screen pixel (top-left origin) using this
        // camera's view + the given projection and viewport. Returns false when the
        // point is behind the camera (out then untouched). Convenience for anchoring
        // HUD elements (health bars, labels) over 3D objects; wraps Tools::worldToScreen.
        bool worldToScreen(const glm::vec3 &worldPos, const glm::mat4 &projection,
                           int width, int height, glm::vec2 &out);

        [[nodiscard]] const glm::vec3 &getPosition() const;
        [[nodiscard]] const glm::vec3 &getFront() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] glm::vec3 getRight() const;
        void setStickyPoint(const std::shared_ptr<Node3D::Transform> &stickyPoint);
        [[nodiscard]] std::shared_ptr<Node3D::Transform> getStickyPoint() const;

        // One-shot teleport k objektu bez follow. Po focusOn je kamera free
        // (rightButtonPressed=true) - dá se s ní hýbat WASD/sipkami/myší bez
        // restrikce. setStickyPoint naopak kameru přilepí napevno.
        void focusOn(const std::shared_ptr<Node3D::Transform>& target);

        // Reset prvního mouse delta - další processMouseMovement zahodí
        // delta a uloží lastX/lastY. Voláno po edge transition (Ctrl/RMB
        // edge press) aby kamera neudělala skok při velkém cursor offsetu.
        void resetMouseDelta();
        [[nodiscard]] glm::vec3 getStickyPosition() const;
        void processMouseMovement(double x, double y);
        void processKeyboard(GLFWwindow *window, float deltaTime);

        void setPosition(const glm::vec3& pos);
        void setFront(const glm::vec3& front);
        void setUp(const glm::vec3& up);
        void onMouseDown(int button, int action, int mods);
        void setKeyState(int key, bool pressed);
        void setReflectionPass(bool value);
        void syncFollowPosition();
        void releaseFollow();

    protected:
        bool keys[1024] = { false };
        glm::vec3 position{};
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};
        float zoom;
        std::shared_ptr<Node3D::Transform> stickyPoint;
        float YAW = -90.0f; // 90
        float PITCH = 56.0f;
        bool rightButtonPressed = true;
        bool firstMouse = true;
        bool reflectionPass = false;
        float lastX = 0.0f;
        float lastY = 0.0f;
        glm::vec3 offsetFromTarget = glm::vec3(0.0f, -3.5f, 3.0f);

        void updateCameraVectors();
    };

} // Manager

#endif //SNAKE3_CAMERA_H
