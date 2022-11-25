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
        void setStickyPoint(BaseItem *stickyPoint);
        glm::vec3 getStickyPosition();
        void updateStickyPoint();
        void processMouseMovement(double x, double y);
        void processKeyboard(Camera_Movement direction, float deltaTime);

    protected:
        glm::vec3 position{};
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};
        float zoom;
        BaseItem* stickyPoint{};
        float YAW = -90.0f;
        float PITCH = 56.0f;

        void updateCameraVectors();
    };

} // Manager

#endif //SNAKE3_CAMERA_H
