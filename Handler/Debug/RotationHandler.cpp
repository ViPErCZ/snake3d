#include "RotationHandler.h"

namespace Handler::Debug {
    RotationHandler::RotationHandler(const shared_ptr<Camera> &camera) : BaseTransform(camera) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
        currentWorldCenter = glm::vec3(0);
    }

    void RotationHandler::onDefaultHandler() {
    }

    void RotationHandler::onEventHandler(const unsigned key, int scancode, int action, int mods, float deltaTime) {
        if (activeItem == nullptr) {
            return;
        }

        const float rotationX = activeItem->getRotationX();
        const float rotationY = activeItem->getRotationY();
        const float rotationZ = activeItem->getRotationZ();

        switch (key) {
            case GLFW_KEY_RIGHT:
                if (enabled) {
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                }
            case GLFW_KEY_TAB:
                if (enabled) {
                    activeItem = findNextItem();
                    computeWorld();
                    camera->setStickyPoint(activeItem);
                }
                break;
            case GLFW_KEY_SPACE:
                if (enabled) {
                    cout << "Rotation X: " << rotationX << endl;
                    cout << "Rotation Y: " << rotationY << endl;
                    cout << "Rotation Z: " << rotationZ << endl;
                }
                break;
            default:
                break;
        }
    }

}
