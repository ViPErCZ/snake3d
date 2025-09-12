#ifndef SNAKE3_CAMERA_H
#define SNAKE3_CAMERA_H

#include "../ItemsDto/BaseItem.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ItemsDto;

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
        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] const glm::vec3 &getPosition() const;
        [[nodiscard]] const glm::vec3 &getFront() const;
        void setStickyPoint(BaseItem *stickyPoint);
        BaseItem* getStickyPoint() const;
        glm::vec3 getStickyPosition() const;
        void updateStickyPoint();
        void processMouseMovement(double x, double y);
        void processKeyboard(GLFWwindow *window, float deltaTime);

        void setPosition(const glm::vec3& pos);
        void setFront(const glm::vec3& front);
        void setUp(const glm::vec3& up);
        void onMouseDown(int button, int action, int mods);
        void setKeyState(int key, bool pressed);

    protected:
        bool keys[1024] = { false };
        glm::vec3 position{};
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};
        float zoom;
        BaseItem* stickyPoint{};
        float YAW = -90.0f; // 90
        float PITCH = 56.0f;
        bool rightButtonPressed = true;
        bool firstMouse = true;
        float lastX = 0.0f;
        float lastY = 0.0f;
        glm::vec3 offsetFromTarget = glm::vec3(0.0f, -3.5f, 3.0f); // výchozí pozice

        void updateCameraVectors();
    };

} // Manager

#endif //SNAKE3_CAMERA_H
