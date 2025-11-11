#include "MeshNode2D.h"

namespace Model {
    MeshNode2D::MeshNode2D(const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager)
        : mesh(mesh), resourceManager(resourceManager), transformDetached(false){
    }

    void MeshNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, const float dt,
                            const glm::mat4 &parentTransform) const {
        if (visible) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            mesh->render(camera, ortho, 1, finalTransform);

            for (auto &node: children) {
                node->render(camera, ortho, dt, transformDetached ? glm::mat4(1.0f) : finalTransform);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->render(camera, ortho, dt, glm::mat4(1.0f));
            }
        }
    }

    void MeshNode2D::update(const float dt) {
        mesh->update(dt);
        for (const auto &node: children) {
            node->update(dt);
        }
    }
} // Model