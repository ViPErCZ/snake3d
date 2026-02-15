#include "BoxShape.h"

#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"

namespace Physic {
    BoxShape::BoxShape(const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<ContextState> &contextState,
                       const glm::vec3 boxSize) : size(boxSize) {
        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
        // material
        material = make_shared<StandardMaterial>(shader, shadowsShader);
        material->setNormalEnabled(false);
        material->setAlpha(0.2);
        material->setBlending(Blending::Translucent);
        auto boxMesh = make_shared<BoxMesh>(shader, 1.0f, 1.0f, 1.0f);
        boxMesh->setMaterial(material);
        meshNode = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
    }

    void BoxShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const glm::mat4 t) {
        const glm::vec3 color = isColliding() ? glm::vec3(1.0f, 0.2f, 0.2f) : glm::vec3(0.2f, 1.0f, 1.0f);
        material->setColor(color);

        meshNode->setScale(this->size);
        meshNode->setPosition(glm::vec3(0.0f));
        meshNode->render(camera, projection, 0.0f, t, false);
    }

    BoxShape::OBB BoxShape::BuildOBB(const glm::mat4 &modelMatrix) const {
        OBB obb{};

        obb.center = glm::vec3(modelMatrix[3]);

        const auto right = glm::vec3(modelMatrix[0]);
        const auto up = glm::vec3(modelMatrix[1]);
        const auto forward = glm::vec3(modelMatrix[2]);

        const float scaleX = glm::length(right);
        const float scaleY = glm::length(up);
        const float scaleZ = glm::length(forward);

        obb.axes[0] = (scaleX > 0.0001f) ? right / scaleX : glm::vec3(1, 0, 0);
        obb.axes[1] = (scaleY > 0.0001f) ? up / scaleY : glm::vec3(0, 1, 0);
        obb.axes[2] = (scaleZ > 0.0001f) ? forward / scaleZ : glm::vec3(0, 0, 1);

        obb.halfExtents.x = (this->size.x * 0.5f) * scaleX;
        obb.halfExtents.y = (this->size.y * 0.5f) * scaleY;
        obb.halfExtents.z = (this->size.z * 0.5f) * scaleZ;

        return obb;
    }

    AABB BoxShape::calculateAABB(const glm::mat4 &modelMatrix) {
        return CalculateAABB(modelMatrix, this->size * 0.5f);
    }
} // Physic
