#include "CollisionSystem3D.h"
#include "CollisionCheck.h"
#include <algorithm>
#include <limits>
#include <unordered_set>

namespace Physic {
    namespace {
        float computeBottomZ(const shared_ptr<MeshNode3D> &node) {
            float bottom = std::numeric_limits<float>::infinity();
            for (const auto &shape : node->getCollisionShapes()) {
                if (!shape || !shape->getShape() || !shape->getShape()->isCollisionEnabled()) continue;
                const glm::mat4 worldMatrix = node->getWorldMatrix() * shape->getModelMatrix();
                const auto [min, max] = shape->getShape()->calculateAABB(worldMatrix);
                bottom = std::min(bottom, min.z);
            }
            return bottom;
        }
    }

    void CollisionSystem3D::addCollider(const shared_ptr<MeshNode3D> &collider, const bool isStatic) {
        if (!collider) return;

        const auto &shapes = collider->getCollisionShapes();

        for (const auto &shapeNode: shapes) {
            if (const auto collisionShape = std::dynamic_pointer_cast<CollisionShape3D>(shapeNode)) {
                CollisionEntry entry;
                entry.shapeNode = collisionShape;
                entry.parentObject = collider;
                entry.isStatic = isStatic;
                flatEntries.push_back(entry);
            }
        }

        for (const auto &child: collider->getChildren()) {
            addCollider(child, isStatic);
        }
    }

    void CollisionSystem3D::removeCollider(const std::shared_ptr<MeshNode3D> &collider) {
        if (!collider) return;

        std::unordered_set<const MeshNode3D*> nodes;
        const auto collectNodes = [&nodes](const std::shared_ptr<MeshNode3D> &node, const auto &self) -> void {
            if (!node) return;
            nodes.insert(node.get());
            for (const auto &child : node->getChildren()) {
                self(child, self);
            }
        };
        collectNodes(collider, collectNodes);

        std::erase_if(flatEntries,
                      [&nodes](const CollisionEntry &entry) {
                          return nodes.contains(entry.parentObject.get());
                      });
    }

    void CollisionSystem3D::clearColliders() {
        flatEntries.clear();
    }

    void CollisionSystem3D::addDynamicBody(const shared_ptr<MeshNode3D> &node,
                                           const shared_ptr<DynamicBody> &body) {
        if (!node || !body) return;
        // Allow re-registering: existing entry is replaced.
        std::erase_if(dynamicBodies,
                      [&node](const DynamicBodyEntry &e) { return e.node == node; });
        DynamicBodyEntry entry;
        entry.node = node;
        entry.body = body;
        body->setPosition(node->getPosition());
        dynamicBodies.push_back(std::move(entry));
    }

    void CollisionSystem3D::removeDynamicBody(const shared_ptr<MeshNode3D> &node) {
        if (!node) return;
        std::erase_if(dynamicBodies,
                      [&node](const DynamicBodyEntry &e) { return e.node == node; });
    }

    void CollisionSystem3D::clearDynamicBodies() {
        dynamicBodies.clear();
    }

    void CollisionSystem3D::step(const float dt) {
        // Phase 1: cache pre-integration bottom Z (used by resolveTopContact)
        //          and sync body position from the (authoritative) node position.
        for (auto &[node, body, previousBottomZ] : dynamicBodies) {
            if (!node || !body || !body->isEnabled()) continue;
            body->setPosition(node->getPosition());
            previousBottomZ = computeBottomZ(node);
        }

        // Phase 2: integrate each enabled body and apply to its node.
        for (const auto &entry : dynamicBodies) {
            if (!entry.node || !entry.body || !entry.body->isEnabled()) continue;
            // Transform::getModelMatrix multiplies setPosition by scale (S*T*R
            // composition), so node.position lives in setPos-units while the
            // world gravity is in world-units. Scale gravity inversely so the
            // integrated setPos change, once multiplied by scale at render-time,
            // matches real-world fall rate.
            const auto scale = entry.node->getScale();
            glm::vec3 scaledGravity = worldGravity;
            if (scale.x != 0.0f) scaledGravity.x /= scale.x;
            if (scale.y != 0.0f) scaledGravity.y /= scale.y;
            if (scale.z != 0.0f) scaledGravity.z /= scale.z;
            entry.body->integrate(dt, scaledGravity);
            entry.node->setPosition(entry.body->getPosition());
            // World matrix must reflect the new position before broad-phase runs.
            // Assumes the node is root-level (or transformDetached) - matches every
            // current use site.
            entry.node->computeWorldMatrix(glm::mat4(1.0f));
        }

        // Phase 3: existing broad + narrow phase (writes collidingBodies on shapes).
        update();

        // Phase 4: resolve top contacts (snap onto surface, kill downward velocity).
        for (auto &entry : dynamicBodies) {
            if (!entry.node || !entry.body || !entry.body->isEnabled()) continue;
            resolveTopContact(entry);
        }
    }

    void CollisionSystem3D::resolveTopContact(const DynamicBodyEntry &entry) {
        // Tolerance for "was effectively above this surface". Must be larger
        // than per-frame gravity drop (~1 mm at 60 fps) so a resting body
        // recovers, but small enough that once a body starts free-falling past
        // a tile boundary it cannot "climb out" by snagging the next cell's
        // floor with the edge of its AABB.
        constexpr float epsilon = 0.01f;
        float bestSupportTop = -std::numeric_limits<float>::infinity();
        bool hasSupport = false;

        for (const auto &shape : entry.node->getCollisionShapes()) {
            if (!shape) continue;
            for (const auto &otherBody : shape->getCollidingBodies()) {
                if (!otherBody) continue;
                for (const auto &otherShape : otherBody->getCollisionShapes()) {
                    if (!otherShape || !otherShape->getShape()) continue;
                    if (!otherShape->getShape()->isCollisionEnabled()) continue;
                    const glm::mat4 wm = otherBody->getWorldMatrix() * otherShape->getModelMatrix();
                    const auto [min, max] = otherShape->getShape()->calculateAABB(wm);
                    // Pre-integration we were at or above this surface => landing from above.
                    if (entry.previousBottomZ >= max.z - epsilon) {
                        if (max.z > bestSupportTop) {
                            bestSupportTop = max.z;
                            hasSupport = true;
                        }
                    }
                }
            }
        }

        if (!hasSupport) return;

        // Only push the body up when it has actually penetrated the support
        // surface this frame. If its bottom is still above the support, gravity
        // is still pulling it in - don't yank it down onto the floor.
        const float currentBottom = computeBottomZ(entry.node);
        if (currentBottom >= bestSupportTop) {
            // Even when resting at zero penetration, kill residual downward
            // velocity so gravity doesn't accumulate frame after frame.
            glm::vec3 v = entry.body->getVelocity();
            if (v.z < 0.0f) {
                v.z = 0.0f;
                entry.body->setVelocity(v);
            }
            return;
        }

        // Snap so the body's current bottom sits on bestSupportTop.
        // adjust is in world units; node.position is in setPos units (scale
        // baked in by getModelMatrix). Convert before applying.
        const float adjust = bestSupportTop - currentBottom;
        const auto scale = entry.node->getScale();
        const float setPosAdjustZ = scale.z != 0.0f ? adjust / scale.z : adjust;
        glm::vec3 pos = entry.node->getPosition();
        pos.z += setPosAdjustZ;
        entry.node->setPosition(pos);
        entry.node->computeWorldMatrix(glm::mat4(1.0f));
        entry.body->setPosition(pos);

        // Kill downward velocity only; upward (e.g. jump impulse) is preserved.
        glm::vec3 v = entry.body->getVelocity();
        if (v.z < 0.0f) {
            v.z = 0.0f;
            entry.body->setVelocity(v);
        }
    }

    void CollisionSystem3D::update() const {
        if (flatEntries.empty()) return;

        std::vector<AABB> worldAABBs(flatEntries.size());

        for (size_t i = 0; i < flatEntries.size(); ++i) {
            const auto &entry = flatEntries[i];
            const auto &shape = entry.shapeNode->getShape();

            shape->setColliding(false);
            entry.shapeNode->clearCollisions();
            if (!shape->isCollisionEnabled()) {
                continue;
            }

            if (entry.isStatic && entry.aabbCached) {
                worldAABBs[i] = entry.cachedAABB;
            } else {
                const glm::mat4 worldMatrix = entry.parentObject->getWorldMatrix() * entry.shapeNode->getModelMatrix();
                worldAABBs[i] = shape->calculateAABB(worldMatrix);
                if (entry.isStatic) {
                    entry.cachedAABB = worldAABBs[i];
                    entry.aabbCached = true;
                }
            }
        }

        for (size_t i = 0; i < flatEntries.size(); i++) {
            // A static entry never moves, so its pair against another static is
            // already settled and would never produce a contact event we care about.
            const bool iStatic = flatEntries[i].isStatic;
            for (size_t j = i + 1; j < flatEntries.size(); j++) {
                if (iStatic && flatEntries[j].isStatic) continue;

                if (!flatEntries[i].shapeNode->getShape()->isCollisionEnabled() ||
                    !flatEntries[j].shapeNode->getShape()->isCollisionEnabled()) {
                    continue;
                }

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
