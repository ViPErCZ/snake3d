#include "MeshNode3D.h"

namespace Model {
    MeshNode3D::MeshNode3D(const shared_ptr<StandardMesh> &mesh,
                           const shared_ptr<ResourceManager> &resourceManager)
        : mesh(mesh), resourceManager(resourceManager), transformDetached(false) {
    }

    void MeshNode3D::addNode(const std::shared_ptr<MeshNode3D> &node) {
        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 20) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 20");
        }
        children.push_back(node);
    }

    void MeshNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                            const glm::mat4 &parentTransform, const bool shadows) const {
        if (visible) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            mesh->render(camera, projection, 1, finalTransform, shadows);

            for (auto &node: children) {
                node->render(camera, projection, dt, transformDetached ? glm::mat4(1.0f) : finalTransform, shadows);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->render(camera, projection, dt, glm::mat4(1.0f), shadows);
            }
        }
    }

    void MeshNode3D::update(const float dt) {
        mesh->update(dt);
        for (const auto &node: children) {
            node->update(dt);
        }
    }

    void MeshNode3D::renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                                   const glm::mat4 &parentTransform) const {
        if (visible) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            mesh->renderShadowMap(camera, projection, dt, finalTransform);
            for (const auto &node: children) {
                node->renderShadows(camera, projection, dt, transformDetached ? glm::mat4(1.0f) : finalTransform);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->renderShadows(camera, projection, dt, glm::mat4(1.0f));
            }
        }
    }

    shared_ptr<BaseItem> MeshNode3D::getBaseItem() const {
        return mesh->getBaseItem();
    }

    glm::mat4 MeshNode3D::getModelMatrix() const {
        auto model = glm::mat4(1.0f);

        model = glm::scale(model, scale);

        glm::vec3 origin = position;
        origin *= mesh->getBaseItem()->getScale();
        model = glm::translate(model, origin);

        model = glm::rotate(model, glm::radians(rotationX),
                            glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationY),
                            glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationZ),
                            glm::vec3(0.0, 0.0, 1.0));

        return model;
    }

    const vector<shared_ptr<MeshNode3D>> &MeshNode3D::getChildren() const {
        return children;
    }

    void MeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
    }

    void MeshNode3D::setTransformDetached(const bool transform_detached, bool recursive) {
        transformDetached = transform_detached;

        if (recursive) {
            for (const auto &child: children) {
                child->setTransformDetached(transform_detached, recursive);
            }
        }
    }

    std::shared_ptr<MeshNode3D> MeshNode3D::deepCopy() const {
        auto copyMesh = std::make_shared<StandardMesh>(*mesh);
        auto copyNode = std::make_shared<MeshNode3D>(copyMesh, resourceManager);

        copyNode->setPosition(this->getPosition());
        copyNode->setScale(this->getScale());
        copyNode->setRotationX(this->rotationX);
        copyNode->setRotationY(this->rotationY);
        copyNode->setRotationZ(this->rotationZ);

        for (auto &child: this->children) {
            auto childCopy = child->deepCopy();
            copyNode->addNode(childCopy);
        }

        return copyNode;
    }

    void MeshNode3D::make_unique() {
        const auto copy = this->deepCopy();
        *this = *copy;
    }

} // Model
