#include "StandardMesh.h"

namespace Model {
    StandardMesh::StandardMesh(shared_ptr<BaseItem> baseItem, shared_ptr<ShaderManager> baseShader,
                               const float width,
                               const float height) : item(std::move(baseItem)), baseShader(std::move(baseShader)),
                                                     width(width), height(height), localMin(+FLT_MAX),
                                                     localMax(-FLT_MIN) {
    }

    shared_ptr<Mesh> StandardMesh::getMesh() const {
        return mesh;
    }

    shared_ptr<BaseItem> StandardMesh::getBaseItem() const {
        return item;
    }

    void StandardMesh::setMaterial(const shared_ptr<BaseMaterial> &material) {
        this->material = material;
    }

    void StandardMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const {
        if (item->isVisible()) {
            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial.get()->bind(
                    camera->getPosition(),
                    camera->getViewMatrix(),
                    projection,
                    getBaseItem()->getModelMatrix()
                );
            }
            mesh->bind();
            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial.get()->unbind();
            }
        }
    }

    void StandardMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const {
        if (item->isVisible()) {
            const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material);
            if (standardMaterial && standardMaterial->isShadowEnabled()) {
                standardMaterial.get()->bindShadow(getBaseItem()->getModelMatrix());

                mesh->bind();
                glLoadIdentity();
                glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                               nullptr);

                standardMaterial.get()->unbind();
            }
        }
    }

    glm::vec3 StandardMesh::getMin() const {
        const glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMin(+FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = item->getModelMatrix() * glm::vec4(corner, 1.0f);
            worldMin = glm::min(worldMin, glm::vec3(worldPos));
        }
        return worldMin;
    }

    glm::vec3 StandardMesh::getMax() const {
        glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMax(-FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = item->getModelMatrix() * glm::vec4(corner, 1.0f);
            worldMax = glm::max(worldMax, glm::vec3(worldPos));
        }
        return worldMax;
    }
} // Model
