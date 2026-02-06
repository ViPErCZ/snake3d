#include "PositionHandler.h"

#include "../../Lights/DirectionalLight.h"

namespace Handler {
    PositionHandler::PositionHandler(const shared_ptr<Camera> &camera): camera(camera), enabled(false) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
    }

    void PositionHandler::onDefaultHandler() {
    }

    void PositionHandler::onEventHandler(const unsigned int key, int scancode, const int action, int mods) {
        if (activeItem == nullptr) {
            return;
        }

        glm::vec3 pos = activeItem->getPosition();
        glm::vec3 zoom = activeItem->getScale();
        const float rotationX = activeItem->getRotationX();
        const float rotationY = activeItem->getRotationY();
        const float rotationZ = activeItem->getRotationZ();

        switch (key) {
            case GLFW_KEY_F8:
                enabled = !enabled;
                camera->setStickyPoint(enabled ? activeItem : cameraOriginalStickyPoint);
                break;
            case GLFW_KEY_RIGHT:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.x += 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.x += 0.1f;  // * SCALE
                        }
                    } else {
                        pos.x += 0.0001f;
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.x -= 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.x -= 0.1f; // * SCALE
                        }
                    } else {
                        pos.x -= 0.0001f;
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_UP:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.y += 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.y += 0.1f;
                        }
                    } else {
                        pos.y += 0.0001f;
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_DOWN:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.y -= 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.y -= 0.1f;
                        }
                    } else {
                        pos.y -= 0.0001f;
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.z += 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.z += 0.01f;
                        }
                    } else {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.z += 0.0001f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.z += 0.0001f;
                        }
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                glm::vec3 direction = directLight->getDirection();
                                direction.z -= 0.01f;
                                directLight->setDirection(direction);
                            }
                        } else {
                            pos.z -= 0.01f;
                        }
                    } else {
                        if (mods & GLFW_MOD_CONTROL) {
                            if (mods & GLFW_MOD_CONTROL) {
                                if (shared_ptr<DirectionalLight> directLight = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                                    glm::vec3 direction = directLight->getDirection();
                                    direction.z -= 0.0001f;
                                    directLight->setDirection(direction);
                                }
                            }
                        } else {
                            pos.z -= 0.0001f;
                        }
                    }
                    activeItem->setPosition(pos);
                }
                break;
            case GLFW_KEY_KP_ADD:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        zoom.x += 0.1f;
                        zoom.y += 0.1f;
                        zoom.z += 0.1f;
                    } else {
                        zoom.x += 0.01f;
                        zoom.y += 0.01f;
                        zoom.z += 0.01f;
                    }
                    activeItem->setScale(zoom);
                }
                break;
            case GLFW_KEY_KP_SUBTRACT:
                if (enabled) {
                    if (mods & GLFW_MOD_SHIFT) {
                        zoom.x -= 0.1f;
                        zoom.y -= 0.1f;
                        zoom.z -= 0.1f;
                    } else {
                        zoom.x -= 0.01f;
                        zoom.y -= 0.01f;
                        zoom.z -= 0.01f;
                    }
                    activeItem->setScale(zoom);
                }
                break;
            case GLFW_KEY_TAB:
                if (enabled) {
                    activeItem = findNextItem();
                    camera->setStickyPoint(activeItem);
                }
                break;
            case GLFW_KEY_SPACE:
                cout << "Position: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
                cout << "Zoom: " << zoom.x << ", " << zoom.y << ", " << zoom.z << endl;
                cout << "Rotation X: " << rotationX << endl;
                cout << "Rotation Y: " << rotationY << endl;
                cout << "Rotation Z: " << rotationZ << endl;
                if (shared_ptr<DirectionalLight> light = std::dynamic_pointer_cast<DirectionalLight>(activeItem)) {
                    cout << "Direction: " << light->getDirection().x << ", " << light->getDirection().y << ", " << light->getDirection().z << endl;
                }
                break;
            default:
                break;
        }
    }

    void PositionHandler::addItem(const shared_ptr<Transform> &item) {
        items.push_back(item);

        if (activeItem == nullptr) {
            activeItem = item;
        }
    }

    shared_ptr<Transform> PositionHandler::findNextItem() {
        if (items.empty()) {
            return nullptr;
        }

        for (auto it = items.begin(); it != items.end(); ++it) {
            if (activeItem == *it) {
                if (it + 1 == items.end()) {
                    return *items.begin();
                }

                return *(it + 1);
            }
        }

        return nullptr;
    }
} // Handler
