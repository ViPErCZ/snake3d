#include "ScaleHandler.h"

namespace Handler::Debug {
    ScaleHandler::ScaleHandler(const shared_ptr<Camera> &camera) : BaseTransform(camera) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
        currentWorldCenter = glm::vec3(0);
    }

    void ScaleHandler::onDefaultHandler() {
    }

    void ScaleHandler::onEventHandler(const unsigned key, int scancode, int action, const int mods,
                                      const float deltaTime) {
        if (activeItem == nullptr) {
            return;
        }

        constexpr float sensitivity = 0.1f;
        glm::vec3 zoom = activeItem->getScale();
        const float oldScaleX = zoom.x;
        const float oldScaleY = zoom.y;
        const float oldScaleZ = zoom.z;
        const float movementX = sensitivity * zoom.x;
        const float movementY = sensitivity * zoom.y;
        const float movementZ = sensitivity * zoom.z;

        switch (key) {
            case GLFW_KEY_KP_ADD:
                if (enabled) {
                    zoom.x += movementX;
                    zoom.y += movementY;
                    zoom.z += movementZ;

                    activeItem->setPosition(glm::vec3(
                        activeItem->getPosition().x * (oldScaleX / zoom.x),
                        activeItem->getPosition().y * (oldScaleY / zoom.y),
                        activeItem->getPosition().z * (oldScaleZ / zoom.z)
                    ));
                    activeItem->setScale(zoom);
                    computeWorld();
                }
                break;
            case GLFW_KEY_KP_SUBTRACT:
                if (enabled) {
                    zoom.x -= movementX;
                    zoom.y -= movementY;
                    zoom.z -= movementZ;

                    activeItem->setPosition(glm::vec3(
                        activeItem->getPosition().x * (oldScaleX / zoom.x),
                        activeItem->getPosition().y * (oldScaleY / zoom.y),
                        activeItem->getPosition().z * (oldScaleZ / zoom.z)
                    ));
                    activeItem->setScale(zoom);
                    computeWorld();
                }
                break;
            case GLFW_KEY_TAB:
                if (enabled) {
                    activeItem = findNextItem();
                    computeWorld();
                    camera->setStickyPoint(activeItem);
                }
                break;
            case GLFW_KEY_SPACE:
                if (enabled) {
                    cout << "Debug Zoom: " << zoom.x << ", " << zoom.y << ", " << zoom.z << endl;
                }
                break;
            default:
                break;
        }
    }
}
