#ifndef SNAKE3_CAMERA_H
#define SNAKE3_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Manager {

    const float YAW = -90.0f;
    const float PITCH = 56.0f;

    class Camera {
    public:
        explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
        [[nodiscard]] float getZoom() const;
        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] const glm::vec3 &getPosition() const;

    protected:
        glm::vec3 position{};
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};
        float zoom;

        void updateCameraVectors();
    };

} // Manager

#endif //SNAKE3_CAMERA_H
