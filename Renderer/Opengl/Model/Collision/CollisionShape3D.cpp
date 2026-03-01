#include "CollisionShape3D.h"

namespace CollisionShape {
    CollisionShape3D::CollisionShape3D(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<Shape> &shape)
        : MeshNode3D(contextState, nullptr, resourceManager), shape(shape) {
    }

    void CollisionShape3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
        const glm::mat4 &parentTransform, const bool shadows) {
        if (visible) {
            glm::mat4 finalTransform = worldMatrixCache;
            const auto parent = getParent();
            if (parent != nullptr) {
                const auto animationPlayer = parent->getMesh()->getAnimationPlayer();
                if (animationPlayer != nullptr) {
                    finalTransform *= animationPlayer->getMetadata(parent->getAnimation())->world_transform;
                }
            }
            // contextState->setBlendingMode(mesh->getBlending());
            // contextState->setDepthTest(mesh->getDepthTest());
            // contextState->setDepthWrite(mesh->getDepthWrite());
            shape->render(camera, projection, finalTransform);

            for (auto &node: children) {
                node->render(camera, projection, dt, transformDetached ? glm::mat4(1.0f) : finalTransform, shadows);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->render(camera, projection, dt, glm::mat4(1.0f), shadows);
            }
        }
    }

    shared_ptr<StandardMesh> CollisionShape3D::getMesh() const { // only for mark ring
        return getParent()->getMesh();
    }
} // CollisionShape