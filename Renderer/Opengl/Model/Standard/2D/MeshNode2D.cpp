#include "MeshNode2D.h"

#include <ranges>

namespace Model {
    MeshNode2D::MeshNode2D(const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager)
        : mesh(mesh), resourceManager(resourceManager), transformDetached(false){
    }

    void MeshNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, const float dt,
                            const glm::mat4 &parentTransform) const {
        if (visible) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            mesh->render(camera, ortho, 1, finalTransform);

            for (const auto &snd: children | views::values) {
                snd->render(camera, ortho, dt, transformDetached ? glm::mat4(1.0f) : finalTransform);
            }
        } else if (transformDetached) {
            for (const auto &snd: children | views::values) {
                snd->render(camera, ortho, dt, glm::mat4(1.0f));
            }
        }
    }

    void MeshNode2D::update(const float dt, const uint64_t frameId) {
        mesh->update(dt);
        for (const auto &snd: children | views::values) {
            snd->update(dt, frameId);
        }
    }

    void MeshNode2D::addNode(const std::shared_ptr<MeshNode2D> &node, const std::string &name) {
        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 20) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 20");
        }
        const auto [fst, snd] = children.emplace(name, node);
        if (!snd) {
            throw std::runtime_error("Duplicate node item key: " + name);
        }
    }

    void MeshNode2D::setTransformDetached(const bool transform_detached, const bool recursive) {
        transformDetached = transform_detached;

        if (recursive) {
            for (const auto &snd: children | views::values) {
                snd->setTransformDetached(transform_detached, recursive);
            }
        }
    }

    const map<std::string, shared_ptr<MeshNode2D> > & MeshNode2D::getChildren() const {
        return children;
    }
} // Model