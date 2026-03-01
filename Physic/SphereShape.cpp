#include "SphereShape.h"

#include "../Renderer/Opengl/Model/Standard/SphereMesh.h"

namespace Physic {
    SphereShape::SphereShape(const shared_ptr<ResourceManager> &resourceManager,
        const shared_ptr<ContextState> &contextState, const float radius) : radius(radius), contextState(contextState), resourceManager(resourceManager) {
        if constexpr (isDebug) {
            const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
            const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
            // material
            material = make_shared<StandardMaterial>(shader, shadowsShader);
            material->setNormalEnabled(false);
            material->setAlpha(0.2);
            material->setBlending(Blending::Translucent);
            // SphereMesh default radius is 0.5, we want to match our radius
            auto sphereMesh = make_shared<SphereMesh>(shader, radius * 2.0f, radius);
            sphereMesh->setMaterial(material);
            meshNode = make_shared<MeshNode3D>(contextState, sphereMesh, resourceManager);
        }
    }

    SphereShape::SphereWorldData SphereShape::BuildSphere(const glm::mat4 &modelMatrix) const {
        SphereWorldData data{};
        // V S*T*R matici je světová pozice ovlivněna škálováním.
        data.center = glm::vec3(modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        const auto scale = glm::vec3(
            glm::length(glm::vec3(modelMatrix[0])),
            glm::length(glm::vec3(modelMatrix[1])),
            glm::length(glm::vec3(modelMatrix[2]))
        );
        data.radius = radius * glm::max(scale.x, glm::max(scale.y, scale.z));

        return data;
    }

    void SphereShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const glm::mat4 t) {
        if constexpr (isDebug) {
            const glm::vec3 color = isColliding() ? glm::vec3(1.0f, 0.2f, 0.2f) : glm::vec3(0.2f, 1.0f, 1.0f);
            material->setColor(color);

            meshNode->setScale(glm::vec3(1.0f));
            meshNode->setPosition(glm::vec3(0.0f));
            meshNode->render(camera, projection, 0.0f, t, false);
        }
    }

    void SphereShape::setRadius(const float radius) {
        this->radius = radius;
        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;

        auto cylinderMesh = make_shared<SphereMesh>(shader, radius * 2.0f, radius);
        cylinderMesh->setMaterial(material);

        meshNode = make_shared<MeshNode3D>(contextState, cylinderMesh, resourceManager);
    }
} // Physic