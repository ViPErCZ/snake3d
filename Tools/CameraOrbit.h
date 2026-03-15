#ifndef SNAKE3_CAMERAORBIT_H
#define SNAKE3_CAMERAORBIT_H

#include <memory>
#include <glm/glm.hpp>

#include "../Manager/Camera.h"

using namespace Manager;

namespace Tools {
    class CameraOrbit {
    public:
        void reset();

        void setCenter(const glm::vec3 &center);
        void setSpeed(float speed);
        void setBackOffset(float offset);
        void setWorldUp(const glm::vec3 &up);

        void initFromCamera(const Camera &camera);
        void initFromCamera(const std::shared_ptr<Camera> &camera);

        void update(Camera &camera, float deltaTime);
        void update(const std::shared_ptr<Camera> &camera, float deltaTime);

        [[nodiscard]] bool isInitialized() const;

    private:
        bool initialized = false;
        float angle = 0.0f;
        float radius = 0.0f;
        float height = 0.0f;
        float speed = 0.12f;
        float backOffset = 3.0f;
        glm::vec3 center = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUp = {0.0f, 0.0f, 1.0f};
    };
} // namespace Tools

#endif // SNAKE3_CAMERAORBIT_H
