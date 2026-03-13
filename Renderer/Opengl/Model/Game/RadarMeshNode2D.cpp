#include "RadarMeshNode2D.h"

#include <ranges>

namespace Model {
    RadarMeshNode2D::RadarMeshNode2D(const shared_ptr<ContextState> &contextState, const shared_ptr<QuadNode2D> &mesh,
                                     const shared_ptr<ResourceManager> &resourceManager)
        : MeshNode2D(contextState, mesh, resourceManager) {
    }

    void RadarMeshNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, const float dt,
        const glm::mat4 &parentTransform) const {

        if (visible) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            for (const auto &snd: children | views::values) {
                snd->render(camera, ortho, dt, transformDetached ? glm::mat4(1.0f) : finalTransform);
            }
            contextState->setBlendingMode(mesh->getBlending());
            contextState->setDepthTest(mesh->getDepthTest());
            contextState->setDepthWrite(mesh->getDepthWrite());
            mesh->render(camera, ortho, 1, finalTransform);
        } else if (transformDetached) {
            for (const auto &snd: children | views::values) {
                snd->render(camera, ortho, dt, glm::mat4(1.0f));
            }
        }
    }

    void RadarMeshNode2D::update(const float dt, const uint64_t frameId) {
        MeshNode2D::update(dt, frameId);
        for (const auto &snd: items | views::values) {
            snd->update();
            if (snd->hasChangedSize()) {
                for (auto it = items.begin(); it != items.end(); ) {
                    if (it->first.rfind(snd->getName() + "-", 0) == 0)
                        it = items.erase(it);
                    else
                        ++it;
                }

                for (auto it = children.begin(); it != children.end(); ) {
                    if (it->first.rfind(snd->getName() + "-", 0) == 0)
                        it = children.erase(it);
                    else
                        ++it;
                }

                // create new radar items for a new snake tile
                int index = 0;
                for (const auto& child: snd->getChildren()) {
                    if (child->isCollisionShapeNode()) {
                        continue;
                    }
                    addItem(child, snd->getColor(), snd->getName() + "-" + std::to_string(index++));
                }
            }
        }
    }

    void RadarMeshNode2D::addItem(const shared_ptr<MeshNode3D> &item, const glm::vec3 &color, const std::string &name) {
        const auto radarItem = make_shared<RadarItem>(contextState, resourceManager, item, color, name);
        radarItem->getRadarItem()->setVisible(item->isVisible());
        items.emplace(name, radarItem);
        addNode(radarItem->getRadarItem(), name);
        int index = 0;

        for (const auto &child: item->getChildren()) {
            if (child->isCollisionShapeNode()) {
                continue;
            }
            const std::string itemName = name + "-" + std::to_string(index++);
            const auto subRadarItem = make_shared<RadarItem>(contextState, resourceManager, child, color, itemName);
            items.emplace(itemName, subRadarItem);

            addNode(subRadarItem->getRadarItem(), itemName);
        }
    }

    void RadarMeshNode2D::hideItems() const {
        for (const auto &snd: children | views::values) {
            snd->setVisible(false);
        }
    }

    void RadarMeshNode2D::showItems() const {
        for (const auto &snd: children | views::values) {
            snd->setVisible(true);
        }
    }

    void RadarMeshNode2D::hideItem(const std::string &name) const {
        items.at(name)->getRadarItem()->setVisible(false);
    }

    void RadarMeshNode2D::showItem(const std::string &name) const {
        items.at(name)->getRadarItem()->setVisible(true);
    }

    void RadarMeshNode2D::clearItems() {
        items.clear();
        children.clear();
    }
} // Model
