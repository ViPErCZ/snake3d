#include <snake3d/Handler/Debug/RotationHandler.h>

using namespace std;
using namespace Manager;

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

        const float sensitivity = mods & GLFW_MOD_SHIFT ? 1.0f : 0.1f;
        const float rotationX = activeItem->getRotationX();
        const float rotationY = activeItem->getRotationY();
        const float rotationZ = activeItem->getRotationZ();

        switch (key) {
            case GLFW_KEY_RIGHT:
                if (enabled) {
                    activeItem->setRotationY(rotationY-sensitivity);
                    computeWorld();
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                    activeItem->setRotationY(rotationY+sensitivity);
                    computeWorld();
                }
                break;
            case GLFW_KEY_UP:
                if (enabled) {
                    activeItem->setRotationX(rotationX-sensitivity);
                    computeWorld();
                }
                break;
            case GLFW_KEY_DOWN:
                if (enabled) {
                    activeItem->setRotationX(rotationX+sensitivity);
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (enabled) {
                    activeItem->setRotationZ(rotationZ-sensitivity);
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (enabled) {
                    activeItem->setRotationZ(rotationZ+sensitivity);
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
                    cout << "Debug Rotation X: " << rotationX << endl;
                    cout << "Debug Rotation Y: " << rotationY << endl;
                    cout << "Debug Rotation Z: " << rotationZ << endl;
                }
                break;
            default:
                break;
        }
    }

}
