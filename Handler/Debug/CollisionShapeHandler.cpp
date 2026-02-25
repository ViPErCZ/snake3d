#include "CollisionShapeHandler.h"

#include "../../Physic/BoxShape.h"
#include "../../Physic/CapsuleShape.h"
#include "../../Physic/CylinderShape.h"
#include "../../Physic/SphereShape.h"

namespace Handler::Debug {
    CollisionShapeHandler::CollisionShapeHandler(const shared_ptr<Camera> &camera) : camera(camera), enabled(false) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
        currentWorldCenter = glm::vec3(0);
    }

    void CollisionShapeHandler::onDefaultHandler() {
    }

    void CollisionShapeHandler::onEventHandler(const unsigned key, int scancode, int action, const int mods, float deltaTime) {
        if (activeItem == nullptr) {
            return;
        }

        const float sensitivity = mods & GLFW_MOD_SHIFT ? 0.1f : 0.01f;

        switch (key) {
            case GLFW_KEY_RIGHT:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.x += sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.x += sensitivity;
                            reinterpretItem->setSize(size);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Capsule) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CapsuleShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius += sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Sphere) {
                            const auto reinterpretItem = reinterpret_pointer_cast<SphereShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius += sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Cylinder) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CylinderShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius += sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                    }
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.x -= sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.x -= sensitivity;
                            reinterpretItem->setSize(size);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Capsule) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CapsuleShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius -= sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Sphere) {
                            const auto reinterpretItem = reinterpret_pointer_cast<SphereShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius -= sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Cylinder) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CylinderShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius -= sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                    }
                    computeWorld();
                }
                break;
            case GLFW_KEY_UP:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.z -= sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.y += sensitivity;
                            reinterpretItem->setSize(size);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Capsule) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CapsuleShape>(activeItem->getShape());

                            float height = reinterpretItem->getHeight();
                            height += sensitivity;
                            reinterpretItem->setHeight(height);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Sphere) {
                            const auto reinterpretItem = reinterpret_pointer_cast<SphereShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius += sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Cylinder) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CylinderShape>(activeItem->getShape());

                            float height = reinterpretItem->getHeight();
                            height += sensitivity;
                            reinterpretItem->setHeight(height);
                        }
                    }

                    computeWorld();
                }
                break;
            case GLFW_KEY_DOWN:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.z += sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.y -= sensitivity;
                            reinterpretItem->setSize(size);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Capsule) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CapsuleShape>(activeItem->getShape());

                            float height = reinterpretItem->getHeight();
                            height -= sensitivity;
                            reinterpretItem->setHeight(height);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Sphere) {
                            const auto reinterpretItem = reinterpret_pointer_cast<SphereShape>(activeItem->getShape());

                            float radius = reinterpretItem->getRadius();
                            radius -= sensitivity;
                            reinterpretItem->setRadius(radius);
                        }
                        if (activeItem->getShape()->getType() == ShapeType::Cylinder) {
                            const auto reinterpretItem = reinterpret_pointer_cast<CylinderShape>(activeItem->getShape());

                            float height = reinterpretItem->getHeight();
                            height -= sensitivity;
                            reinterpretItem->setHeight(height);
                        }
                    }
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.y -= sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.z -= sensitivity;
                            reinterpretItem->setSize(size);
                        }
                    }
                    computeWorld();
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (enabled) {
                    if (mods & GLFW_MOD_CONTROL) {
                        auto pos = activeItem->getPosition();
                        pos.y += sensitivity;
                        activeItem->setPosition(pos);
                    } else {
                        if (activeItem->getShape()->getType() == ShapeType::Box) {
                            const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                            glm::vec3 size = reinterpretItem->getSize();
                            size.z += sensitivity;
                            reinterpretItem->setSize(size);
                        }
                    }
                    computeWorld();
                }
                break;
            case GLFW_KEY_TAB:
                if (enabled) {
                    activeItem = findNextItem();
                    computeWorld();
                    const auto parent = activeItem->getParent();
                    camera->setStickyPoint(parent != nullptr ? parent : activeItem);
                }
                break;
            case GLFW_KEY_SPACE:
                if (enabled) {
                    auto pos = activeItem->getPosition();
                    cout << "Debug Position: " << pos.x << ", " << pos.y << ", " << pos.z << endl;
                    if (activeItem->getShape()->getType() == ShapeType::Box) {
                        const auto reinterpretItem = reinterpret_pointer_cast<BoxShape>(activeItem->getShape());

                        glm::vec3 size = reinterpretItem->getSize();
                        cout << "Debug BoxShape size: " << size.x << ", " << size.y << ", " << size.z << endl;
                    } else if (activeItem->getShape()->getType() == ShapeType::Capsule) {
                        const auto reinterpretItem = reinterpret_pointer_cast<CapsuleShape>(activeItem->getShape());

                        cout << "Debug CapsuleShape height: " << reinterpretItem->getHeight() << endl;
                        cout << "Debug CapsuleShape radius: " << reinterpretItem->getRadius() << endl;
                    } else if (activeItem->getShape()->getType() == ShapeType::Sphere) {
                        const auto reinterpretItem = reinterpret_pointer_cast<SphereShape>(activeItem->getShape());

                        cout << "Debug SphereShape radius: " << reinterpretItem->getRadius() << endl;
                    } else if (activeItem->getShape()->getType() == ShapeType::Cylinder) {
                        const auto reinterpretItem = reinterpret_pointer_cast<CylinderShape>(activeItem->getShape());

                        cout << "Debug CylinderShape height: " << reinterpretItem->getHeight() << endl;
                        cout << "Debug CylinderShape radius: " << reinterpretItem->getRadius() << endl;
                    }
                }
                break;
            default:
                break;
        }
    }

    void CollisionShapeHandler::addItem(const shared_ptr<CollisionShape3D> &item) {
        items.push_back(item);

        if (activeItem == nullptr) {
            activeItem = item;
            computeWorld();
        }
    }

    void CollisionShapeHandler::active() {
        enabled = true;
        if (nullptr != activeItem) {
            if (!activeItem->isVisible()) {
                activeItem = findNextItem();
                computeWorld();
            }
            const auto parent = activeItem->getParent();
            camera->setStickyPoint(parent != nullptr ? parent : activeItem);
        }
    }

    shared_ptr<CollisionShape3D> CollisionShapeHandler::findNextItem() {
        if (items.empty()) {
            return nullptr;
        }

        for (auto it = items.begin(); it != items.end(); ++it) {
            if (activeItem == *it) {
                if (it + 1 == items.end()) {
                    if (items.size() > 1 && !items[0]->isVisible()) {
                        return findFirstVisible();
                    }
                    return *items.begin();
                }

                int index = 1;
                auto nextItem = *(it + 1);
                if (nextItem->isVisible()) {
                    return nextItem;
                }

                for (; it + index < items.end(); ++index) {
                    if (items[index]->isVisible()) {
                        return items[index];
                    }
                }

                return *items.begin();
            }
        }

        return nullptr;
    }

    shared_ptr<CollisionShape3D> CollisionShapeHandler::findFirstVisible() {
        for (auto & item : items) {
            if (item->isVisible()) {
                return item;
            }
        }

        return *items.begin();
    }

    void CollisionShapeHandler::computeWorld() {
        worldMin = activeItem->getShape()->getMeshNode()->getMesh()->getMin(activeItem->getModelMatrix());
        worldMax = activeItem->getShape()->getMeshNode()->getMesh()->getMax(activeItem->getModelMatrix());
        currentWorldCenter = (worldMin + worldMax) * 0.5f;
    }
}
