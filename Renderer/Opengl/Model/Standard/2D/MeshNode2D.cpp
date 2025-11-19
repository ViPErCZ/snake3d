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

    void MeshNode2D::addNode(const std::shared_ptr<MeshNode2D> &node) {
        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 20) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 20");
        }
        children.push_back(node);
    }

    void MeshNode2D::setTransformDetached(const bool transform_detached, const bool recursive) {
        transformDetached = transform_detached;

        if (recursive) {
            for (const auto &child: children) {
                child->setTransformDetached(transform_detached, recursive);
            }
        }
    }

    const vector<shared_ptr<MeshNode2D>> & MeshNode2D::getChildren() const {
        return children;
    }
} // Model