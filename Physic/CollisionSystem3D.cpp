#include "CollisionSystem3D.h"
#include "CollisionCheck.h"

namespace Physic {
    void CollisionSystem3D::addCollider(const shared_ptr<MeshNode3D> &collider) {
        const auto &shapes = collider->getCollisionShapes();

        for (const auto &shapeNode: shapes) {
            if (const auto collisionShape = std::dynamic_pointer_cast<CollisionShape3D>(shapeNode)) {
                CollisionEntry entry;
                entry.shapeNode = collisionShape;
                entry.parentObject = collider;
                flatEntries.push_back(entry);
            }
        }
    }

    void CollisionSystem3D::removeCollider(const std::shared_ptr<MeshNode3D> &collider) {
        std::erase_if(flatEntries,
                      [&collider](const CollisionEntry &entry) {
                          return entry.parentObject == collider;
                      });
    }

    void CollisionSystem3D::clearColliders() {
        flatEntries.clear();
    }

    void CollisionSystem3D::update() const {
        if (flatEntries.empty()) return;

        std::vector<AABB> worldAABBs(flatEntries.size());

        for (size_t i = 0; i < flatEntries.size(); ++i) {
            auto &entry = flatEntries[i];

            // Resetujeme stav kolize
            entry.shapeNode->getShape()->setColliding(false);

            // WorldMatrix = Parent (Object) * Local (Shape)
            const glm::mat4 worldMatrix = entry.parentObject->getModelMatrix() * entry.shapeNode->getModelMatrix();
            worldAABBs[i] = entry.shapeNode->getShape()->calculateAABB(worldMatrix);
        }

        for (size_t i = 0; i < flatEntries.size(); i++) {
            for (size_t j = i + 1; j < flatEntries.size(); j++) {
                // Owner filtering: Nepočítej kolize mezi tvary stejného objektu
                if (flatEntries[i].parentObject == flatEntries[j].parentObject) {
                    continue;
                }

                if (CollisionCheck::IntersectAABB(worldAABBs[i], worldAABBs[j])) {
                    // flatEntries[i].shapeNode->getShape()->setColliding(true);
                    // flatEntries[j].shapeNode->getShape()->setColliding(true);

                    if (CollisionCheck::IntersectExact(flatEntries[i], flatEntries[j])) {
                        flatEntries[i].shapeNode->getShape()->setColliding(true);
                        flatEntries[j].shapeNode->getShape()->setColliding(true);
                    }
                }
            }
        }
    }
} // Physic
