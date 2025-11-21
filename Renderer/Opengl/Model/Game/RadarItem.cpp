#include "RadarItem.h"

#include <utility>

#include "../Standard/2D/QuadNode2D.h"

namespace Model {
    RadarItem::RadarItem(const shared_ptr<ResourceManager> &resourceManager,
        const shared_ptr<MeshNode3D> &mesh, const glm::vec3 &color, std::string name)
        : mesh(mesh), changedSize(false), name(std::move(name)),
            color(color) {
        const auto quad = make_shared<QuadNode2D>(4, 4, resourceManager->getShader("basic2d"));
        quad->setColor(color);
        radarItem = make_shared<MeshNode2D>(quad, resourceManager);
        radarItem->setVisible(mesh->isVisible());
    }

    void RadarItem::update() {
        // update position
        radarItem->setPosition(glm::vec3(-110 + 15 + (mesh->x - 16) / 32 * 4, 110 - 15 - (mesh->y - 16) / 32 * 4, 0));

        if (mesh->hasChildrenChangedSignal()) {
            changedSize = true;
        } else {
            changedSize = false;
        }
    }

    const vector<shared_ptr<MeshNode3D>> &RadarItem::getChildren() const {
        return mesh->getChildren();
    }

    glm::vec3 RadarItem::getColor() const {
        return color;
    }

    shared_ptr<MeshNode2D> RadarItem::getRadarItem() {
        return radarItem;
    }

    bool RadarItem::hasChangedSize() const {
        return changedSize;
    }

    std::string RadarItem::getName() const {
        return name;
    }
} // Model