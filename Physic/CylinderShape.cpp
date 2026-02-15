#include "CylinderShape.h"
#include "../Renderer/Opengl/Model/Standard/CylinderMesh.h"

namespace Physic {
    CylinderShape::CylinderShape(const shared_ptr<ResourceManager> &resourceManager,
                                 const shared_ptr<ContextState> &contextState,
                                 const float radius, const float height)
        : radius(radius), height(height) {

        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;

        material = make_shared<StandardMaterial>(shader, shadowsShader);
        material->setNormalEnabled(false);
        material->setAlpha(0.2);
        material->setBlending(Blending::Translucent);

        auto cylinderMesh = make_shared<CylinderMesh>(shader, radius, radius, height, 8, 32);
        cylinderMesh->setMaterial(material);
        meshNode = make_shared<MeshNode3D>(contextState, cylinderMesh, resourceManager);
    }

    CylinderShape::CylinderWorldData CylinderShape::BuildCylinder(const glm::mat4 &modelMatrix) const {
        CylinderWorldData data{};

        const auto scale = glm::vec3(
            glm::length(glm::vec3(modelMatrix[0])),
            glm::length(glm::vec3(modelMatrix[1])),
            glm::length(glm::vec3(modelMatrix[2]))
        );

        const float halfH = (height * 0.5f) * scale.y;

        const auto center = glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
        const glm::vec3 up = glm::normalize(glm::vec3(modelMatrix[1]));

        data.p0 = center - up * halfH;
        data.p1 = center + up * halfH;
        data.radius = radius * glm::max(scale.x, scale.z);

        return data;
    }

    AABB CylinderShape::calculateAABB(const glm::mat4 &modelMatrix) {
        auto data = BuildCylinder(modelMatrix);
        const glm::vec3 min = glm::min(data.p0, data.p1) - glm::vec3(data.radius);
        const glm::vec3 max = glm::max(data.p0, data.p1) + glm::vec3(data.radius);
        
        // const glm::vec3 d = data.p1 - data.p0;
        // glm::vec3 e = data.radius * glm::sqrt(1.0f - (d * d) / glm::dot(d, d));
        
        // Simplify for now as we did for Capsule
        return {min, max};
    }

    void CylinderShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) {
        const glm::vec3 color = isColliding() ? glm::vec3(1.0f, 0.2f, 0.2f) : glm::vec3(0.2f, 1.0f, 1.0f);
        material->setColor(color);

        meshNode->setScale(glm::vec3(1.0f));
        meshNode->setPosition(glm::vec3(0.0f));
        meshNode->render(camera, projection, 0.0f, t, false);
    }
} // Physic
