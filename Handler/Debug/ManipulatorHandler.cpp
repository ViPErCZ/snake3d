#include "ManipulatorHandler.h"

namespace Handler::Debug {
    ManipulatorHandler::ManipulatorHandler(const shared_ptr<Camera> &camera) {
        positionHandler = make_shared<PositionHandler>(camera);
        scaleHandler = make_shared<ScaleHandler>(camera);
        rotationHandler = make_shared<RotationHandler>(camera);
        collisionShapeHandler = make_shared<CollisionShapeHandler>(camera);
    }

    void ManipulatorHandler::onDefaultHandler() {
    }

    void ManipulatorHandler::onEventHandler(const unsigned key, const int scancode, const int action, const int mods, const float deltaTime)
    {
        switch (key) {
            case GLFW_KEY_F5:
                positionEnable = !positionEnable;
                if (positionEnable) {
                    positionHandler->activate();
                }
                scaleEnable = false;
                rotateEnable = false;
                scaleHandler->deactivate();
                rotationHandler->deactivate();
                collisionShapeHandler->deactivate();
                break;
            case GLFW_KEY_F6:
                scaleEnable = !scaleEnable;
                if (scaleEnable) {
                    scaleHandler->activate();
                }
                rotateEnable = false;
                positionEnable = false;
                rotationHandler->deactivate();
                positionHandler->deactivate();
                collisionShapeHandler->deactivate();
                break;
            case GLFW_KEY_F7:
                rotateEnable = !rotateEnable;
                if (rotateEnable) {
                    rotationHandler->activate();
                }
                scaleEnable = false;
                positionEnable = false;
                positionHandler->deactivate();
                scaleHandler->deactivate();
                collisionShapeHandler->deactivate();
                break;
            case GLFW_KEY_F8:
                collisionShapeEnable = !collisionShapeEnable;
                if (collisionShapeEnable) {
                    collisionShapeHandler->active();
                }
                scaleEnable = false;
                positionEnable = false;
                rotateEnable = false;
                positionHandler->deactivate();
                scaleHandler->deactivate();
                rotationHandler->deactivate();
                break;
            default:
                break;
        }

        if (positionEnable) {
            positionHandler->onEventHandler(key, scancode, action, mods, deltaTime);
        } else if (scaleEnable) {
            scaleHandler->onEventHandler(key, scancode, action, mods, deltaTime);
        } else if (rotateEnable) {
            rotationHandler->onEventHandler(key, scancode, action, mods, deltaTime);
        } else if (collisionShapeEnable) {
            collisionShapeHandler->onEventHandler(key, scancode, action, mods, deltaTime);
        }
    }

    bool ManipulatorHandler::isActive() const {
        if (positionEnable) {
            return positionHandler->isActiveItemVisible();
        }
        if (scaleEnable) {
            return scaleHandler->isActiveItemVisible();
        }
        if (rotateEnable) {
            return rotationHandler->isActiveItemVisible();
        }

        return false;
    }

    glm::vec3 ManipulatorHandler::getItemWorldCenter() const {
        if (positionEnable) {
            return positionHandler->getItemWorldCenter();
        }
        if (scaleEnable) {
            return scaleHandler->getItemWorldCenter();
        }
        if (rotateEnable) {
            return rotationHandler->getItemWorldCenter();
        }

        return glm::vec3(0);
    }

    glm::vec3 ManipulatorHandler::getWorldMin() const {
        if (positionEnable) {
            return positionHandler->getWorldMin();
        }
        if (scaleEnable) {
            return scaleHandler->getWorldMin();
        }
        if (rotateEnable) {
            return rotationHandler->getWorldMin();
        }

        return glm::vec3(0);
    }

    glm::vec3 ManipulatorHandler::getWorldMax() const {
        if (positionEnable) {
            return positionHandler->getWorldMax();
        }
        if (scaleEnable) {
            return scaleHandler->getWorldMax();
        }
        if (rotateEnable) {
            return rotationHandler->getWorldMax();
        }

        return glm::vec3(0);
    }
}
