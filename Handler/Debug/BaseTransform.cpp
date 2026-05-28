#include "BaseTransform.h"

#include <algorithm>

using namespace std;
using namespace Manager;
using namespace Model;

namespace Debug {
    BaseTransform::BaseTransform(const shared_ptr<Camera> &camera) : camera(camera), enabled(false) {
    }

    void BaseTransform::addItem(const shared_ptr<MeshNode3D> &item) {
        items.push_back(item);

        if (activeItem == nullptr) {
            activeItem = item;
            computeWorld();
        }
    }

    shared_ptr<MeshNode3D> BaseTransform::findNextItem() {
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

    shared_ptr<MeshNode3D> BaseTransform::findFirstVisible() {
        for (auto & item : items) {
            if (item->isVisible()) {
                return item;
            }
        }

        return *items.begin();
    }

    void BaseTransform::activate() {
        enabled = true;
        if (nullptr != activeItem) {
            if (!activeItem->isVisible()) {
                activeItem = findNextItem();
                computeWorld();
            }
            camera->setStickyPoint(activeItem);
        }
    }

    bool BaseTransform::isActiveItemVisible() const {
        if (activeItem == nullptr) {
            return false;
        }

        return activeItem->isVisible();
    }

    glm::vec3 BaseTransform::getItemWorldCenter() const {
        return currentWorldCenter;
    }

    glm::vec3 BaseTransform::getWorldMax() const {
        return worldMax;
    }

    glm::vec3 BaseTransform::getWorldMin() const {
        return worldMin;
    }

    void BaseTransform::setActiveItem(const shared_ptr<MeshNode3D>& item) {
        if (!item) return;
        const auto found = std::find(items.begin(), items.end(), item);
        if (found == items.end()) {
            items.push_back(item);
        }
        activeItem = item;
        computeWorld();
        // No camera side-effect - GUI dropdown řeší teleport přes
        // Camera::focusOn; activate() (entry do keyboard mode) zachovává
        // sticky behavior pro hands-on manipulaci.
    }

    void BaseTransform::computeWorld() {
        worldMin = activeItem->getMesh()->getMin(activeItem->getModelMatrix());
        worldMax = activeItem->getMesh()->getMax(activeItem->getModelMatrix());
        currentWorldCenter = (worldMin + worldMax) * 0.5f;
    }
} // Debug
