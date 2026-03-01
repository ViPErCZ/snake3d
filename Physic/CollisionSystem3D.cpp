#include "CollisionSystem3D.h"
#include "CollisionCheck.h"

namespace Physic {
    void CollisionSystem3D::addCollider(const shared_ptr<MeshNode3D> &collider) {
        if (!collider) return;

        const auto &shapes = collider->getCollisionShapes();

        for (const auto &shapeNode: shapes) {
            if (const auto collisionShape = std::dynamic_pointer_cast<CollisionShape3D>(shapeNode)) {
                CollisionEntry entry;
                entry.shapeNode = collisionShape;
                entry.parentObject = collider;
                flatEntries.push_back(entry);
            }
        }

        for (const auto &child: collider->getChildren()) {
            addCollider(child);
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

            entry.shapeNode->getShape()->setColliding(false);
            entry.shapeNode->clearCollisions();

            const glm::mat4 worldMatrix = entry.parentObject->getWorldMatrix() * entry.shapeNode->getModelMatrix();
            worldAABBs[i] = entry.shapeNode->getShape()->calculateAABB(worldMatrix);
        }

        for (size_t i = 0; i < flatEntries.size(); i++) {
            for (size_t j = i + 1; j < flatEntries.size(); j++) {
                if (flatEntries[i].parentObject == flatEntries[j].parentObject) {
                    continue;
                }

                if (!CollisionShape3D::shouldCollide(
                flatEntries[i].shapeNode->getCollisionLayer(), flatEntries[i].shapeNode->getCollisionMask(),
                flatEntries[j].shapeNode->getCollisionLayer(), flatEntries[j].shapeNode->getCollisionMask()))
                {
                    continue;
                }

                if (CollisionCheck::IntersectAABB(worldAABBs[i], worldAABBs[j])) {

                    if (CollisionCheck::IntersectExact(flatEntries[i], flatEntries[j])) {
                        flatEntries[i].shapeNode->getShape()->setColliding(true);
                        flatEntries[j].shapeNode->getShape()->setColliding(true);
                        flatEntries[i].shapeNode->addCollidingBody(flatEntries[j].parentObject);
                        flatEntries[j].shapeNode->addCollidingBody(flatEntries[i].parentObject);
                    }
                }
            }
        }
    }
} // Physic
