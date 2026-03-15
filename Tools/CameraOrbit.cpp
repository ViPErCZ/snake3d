#include "CameraOrbit.h"

#include <algorithm>
#include <cmath>

namespace Tools {
    void CameraOrbit::reset() {
        initialized = false;
    }

    void CameraOrbit::setCenter(const glm::vec3 &center) {
        this->center = center;
    }

    void CameraOrbit::setSpeed(const float speed) {
        this->speed = speed;
    }

    void CameraOrbit::setBackOffset(const float offset) {
        backOffset = offset;
    }

    void CameraOrbit::setWorldUp(const glm::vec3 &up) {
        worldUp = up;
    }

    void CameraOrbit::initFromCamera(const Camera &camera) {
        const glm::vec3 cameraPos = camera.getPosition();
        const glm::vec3 toCamera = cameraPos - center;
        const float horizontalDistance = glm::length(glm::vec2(toCamera.x, toCamera.y));

        radius = std::max(0.1f, horizontalDistance + backOffset);
        height = toCamera.z;
        angle = std::atan2(toCamera.y, toCamera.x);
        initialized = true;
    }

    void CameraOrbit::initFromCamera(const std::shared_ptr<Camera> &camera) {
        if (!camera) {
            return;
        }
        initFromCamera(*camera);
    }

    void CameraOrbit::update(Camera &camera, const float deltaTime) {
        if (!initialized) {
            initFromCamera(camera);
        }

        angle += deltaTime * speed;
        const glm::vec3 orbitPos = {
            center.x + std::cos(angle) * radius,
            center.y + std::sin(angle) * radius,
            center.z + height
        };

        camera.setPosition(orbitPos);
        camera.setFront(center - orbitPos);
        camera.setUp(worldUp);
    }

    void CameraOrbit::update(const std::shared_ptr<Camera> &camera, const float deltaTime) {
        if (!camera) {
            return;
        }
        update(*camera, deltaTime);
    }

    bool CameraOrbit::isInitialized() const {
        return initialized;
    }
} // namespace Tools
