#include "MeshNode3D.h"

namespace Model {
    MeshNode3D::MeshNode3D(const shared_ptr<StandardMesh> &mesh,
                           const shared_ptr<ResourceManager> &resourceManager)
        : mesh(mesh), resourceManager(resourceManager) {
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
                            const glm::mat4 &parentTransform) const {
        if (!mesh->getBaseItem()->isVisible()) {
            return;
        }

        const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();

        mesh->render(camera, projection, 1, finalTransform);
        for (auto &node: children) {
            node->render(camera, projection, dt, finalTransform);
        }
    }

    void MeshNode3D::update(const float dt) const {
        mesh->update(dt);
        for (const auto &node: children) {
            node->update(dt);
        }
    }

    void MeshNode3D::renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                                   const glm::mat4 &parentTransform) const {
        if (!mesh->getBaseItem()->isVisible()) {
            return;
        }

        const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
        mesh->renderShadowMap(camera, projection, dt, finalTransform);
        for (const auto &node: children) {
            node->renderShadows(camera, projection, dt, finalTransform);
        }
    }

    shared_ptr<BaseItem> MeshNode3D::getBaseItem() const {
        return mesh->getBaseItem();
    }

    glm::mat4 MeshNode3D::getModelMatrix() const {
        auto model = glm::mat4(1.0f);

        model = glm::scale(model, zoom);

        glm::vec3 origin = position;
        origin *= mesh->getBaseItem()->getZoom();
        model = glm::translate(model, origin);

        if (rotate[0].w != 0.0f)
            model = glm::rotate(model, glm::radians(rotate[0].w),
                                glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z));
        if (rotate[1].w != 0.0f)
            model = glm::rotate(model, glm::radians(rotate[1].w),
                                glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z));
        if (rotate[2].w != 0.0f)
            model = glm::rotate(model, glm::radians(rotate[2].w),
                                glm::vec3(rotate[2].x, rotate[2].y, rotate[2].z));

        return model;
    }

    void MeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
    }

    std::shared_ptr<MeshNode3D> MeshNode3D::deepCopy() const {
        auto copyMesh = std::make_shared<StandardMesh>(*mesh);
        auto copyNode = std::make_shared<MeshNode3D>(copyMesh, resourceManager);

        copyNode->setPosition(this->getPosition());
        copyNode->setZoom(this->getZoom());
        copyNode->setRotate(this->getRotate()[0], this->getRotate()[1], this->getRotate()[2]);

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
