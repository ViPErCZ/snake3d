#include "PositionHandler.h"

#include "../../Lights/DirectionalLight.h"

namespace Handler::Debug {
    PositionHandler::PositionHandler(const shared_ptr<Camera> &camera): BaseTransform(camera) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
        currentWorldCenter = glm::vec3(0);
    }

    void PositionHandler::onDefaultHandler() {
    }

    void PositionHandler::onEventHandler(const unsigned int key, int scancode, const int action, const int mods, float deltaTime) {
        if (activeItem == nullptr) {
            return;
        }

        const float sensitivity = mods & GLFW_MOD_SHIFT ? 1.0f : 0.1f;
        glm::vec3 pos = activeItem->getPosition();
        const glm::vec3 zoom = activeItem->getScale();

        switch (key) {
            case GLFW_KEY_RIGHT:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.x += 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     }
                    // }

                    const float movement = sensitivity * zoom.x;
                    pos.x += movement;

                    activeItem->setPosition(pos);
                    computeWorld();
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.x -= 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.x -= 0.1f; // * SCALE
                    //     }
                    // } else {
                    //     pos.x -= 0.0001f;
                    // }

                    const float movement = sensitivity * zoom.x;
                    pos.x -= movement;

                    activeItem->setPosition(pos);
                    computeWorld();
                }
                break;
            case GLFW_KEY_UP:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.y += 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.y += 0.1f;
                    //     }
                    // } else {
                    //     pos.y += 0.0001f;
                    // }


                    const float movement = sensitivity * zoom.y;
                    pos.y += movement;

                    activeItem->setPosition(pos);
                    computeWorld();
                }
                break;
            case GLFW_KEY_DOWN:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.y -= 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.y -= 0.1f;
                    //     }
                    // } else {
                    //     pos.y -= 0.0001f;
                    // }

                    const float movement = sensitivity * zoom.y;
                    pos.y -= movement;

                    activeItem->setPosition(pos);
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.z += 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.z += 0.01f;
                    //     }
                    // } else {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.z += 0.0001f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.z += 0.0001f;
                    //     }
                    // }

                    const float movement = sensitivity * zoom.z;
                    pos.z += movement;

                    activeItem->setPosition(pos);
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (enabled) {
                    // if (mods & GLFW_MOD_SHIFT) {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //             glm::vec3 direction = directLight->getDirection();
                    //             direction.z -= 0.01f;
                    //             directLight->setDirection(direction);
                    //         }
                    //     } else {
                    //         pos.z -= 0.01f;
                    //     }
                    // } else {
                    //     if (mods & GLFW_MOD_CONTROL) {
                    //         if (mods & GLFW_MOD_CONTROL) {
                    //             if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    //                 glm::vec3 direction = directLight->getDirection();
                    //                 direction.z -= 0.0001f;
                    //                 directLight->setDirection(direction);
                    //             }
                    //         }
                    //     } else {
                    //         pos.z -= 0.0001f;
                    //     }
                    // }

                    const float movement = sensitivity * zoom.z;
                    pos.z -= movement;

                    activeItem->setPosition(pos);
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
                    cout << "Debug Position: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
                    if (const shared_ptr<DirectionalLight> light = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                        cout << "Direction: " << light->getDirection().x << ", " << light->getDirection().y << ", " << light->getDirection().z << endl;
                    }
                }
                break;
            default:
                break;
        }
    }
} // Handler
