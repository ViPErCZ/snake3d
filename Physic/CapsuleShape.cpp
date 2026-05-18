#include "CapsuleShape.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Model/Standard/CapsuleMesh.h"

namespace Physic {
    CapsuleShape::CapsuleShape(const shared_ptr<ResourceManager> &resourceManager,
                             const shared_ptr<ContextState> &contextState,
                             const float radius, const float height)
        : radius(radius), height(height), contextState(contextState), resourceManager(resourceManager) {

        if constexpr (isDebug) {
            if (!resourceManager) return;
            const auto shader = resourceManager->getShader("basicShader");
            albedoFeature = make_shared<Feature::AlbedoFeature>(nullptr);
            albedoFeature->setAlpha(0.2f);
            material = Material::MaterialBuilder()
                .useMaster("basicShader")
                .with(albedoFeature)
                .build(*resourceManager->getShaderRegistry());
            material->setBlending(Blending::Translucent);

            auto capsuleMesh = make_shared<CapsuleMesh>(shader, height, radius);
            capsuleMesh->setMaterial(material);
            meshNode = make_shared<MeshNode3D>(contextState, capsuleMesh, resourceManager);
        }
    }

    CapsuleShape::CapsuleWorldData CapsuleShape::BuildCapsule(const glm::mat4 &modelMatrix) const {
        CapsuleWorldData data{};
        
        const auto scale = glm::vec3(
            glm::length(glm::vec3(modelMatrix[0])),
            glm::length(glm::vec3(modelMatrix[1])),
            glm::length(glm::vec3(modelMatrix[2]))
        );
        
        float cylinderHeight = height - 2.0f * radius;
        if (cylinderHeight < 0) cylinderHeight = 0;

        const float halfCylHeight = (cylinderHeight * 0.5f) * scale.y;

        const auto center = glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
        const glm::vec3 up = glm::normalize(glm::vec3(modelMatrix[1]));
        
        data.p0 = center - up * halfCylHeight;
        data.p1 = center + up * halfCylHeight;
        data.radius = radius * glm::max(scale.x, scale.z);
        
        return data;
    }

    AABB CapsuleShape::calculateAABB(const glm::mat4 &modelMatrix) {
        auto [p0, p1, radius] = BuildCapsule(modelMatrix);
        const glm::vec3 min = glm::min(p0, p1) - glm::vec3(radius);
        const glm::vec3 max = glm::max(p0, p1) + glm::vec3(radius);

        return {min, max};
    }

    void CapsuleShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) {
        if constexpr (isDebug) {
            const glm::vec3 color = isColliding() ? glm::vec3(1.0f, 0.2f, 0.2f) : glm::vec3(0.2f, 1.0f, 1.0f);
            albedoFeature->setColor(color);

            meshNode->setScale(glm::vec3(1.0f));
            meshNode->setPosition(glm::vec3(0.0f));
            meshNode->render(camera, projection, 0.0f, t, false);
        }
    }

    void CapsuleShape::setRadius(const float radius) {
        this->radius = radius;

        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
        auto capsuleMesh = make_shared<CapsuleMesh>(shader, height, radius);
        capsuleMesh->setMaterial(material);
        meshNode = make_shared<MeshNode3D>(contextState, capsuleMesh, resourceManager);
    }

    void CapsuleShape::setHeight(const float height) {
        this->height = height;

        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
        auto capsuleMesh = make_shared<CapsuleMesh>(shader, height, radius);
        capsuleMesh->setMaterial(material);
        meshNode = make_shared<MeshNode3D>(contextState, capsuleMesh, resourceManager);
    }
} // Physic
