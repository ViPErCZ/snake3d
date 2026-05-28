#include "BoxShape.h"

#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"

using namespace Model;
using namespace Build;
using namespace std;
using namespace Manager;
using namespace Tools;

namespace Physic {
    BoxShape::BoxShape(const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<ContextState> &contextState,
                       const glm::vec3 boxSize) : size(boxSize), resourceManager(resourceManager), contextState(contextState) {
        if constexpr (isDebug) {
            // Tests construct shapes with a null ResourceManager - skip the
            // debug visualization material entirely in that case.
            if (!resourceManager) return;
            const auto shader = resourceManager->getShader("basicShader");
            albedoFeature = make_shared<Feature::AlbedoFeature>(nullptr);
            albedoFeature->setAlpha(0.2f);
            material = Material::MaterialBuilder()
                .useMaster("basicShader")
                .with(albedoFeature)
                .build(*resourceManager->getShaderRegistry());
            material->setBlending(Blending::Translucent);
            auto boxMesh = make_shared<BoxMesh>(shader, size.x, size.y, size.z);
            boxMesh->setMaterial(material);
            meshNode = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
        }
    }

    void BoxShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const glm::mat4 t) {
        if constexpr (isDebug) {
            const glm::vec3 color = isColliding() ? glm::vec3(1.0f, 0.2f, 0.2f) : glm::vec3(0.2f, 1.0f, 1.0f);
            albedoFeature->setColor(color);

            meshNode->setScale(glm::vec3(1.0f));
            meshNode->setPosition(glm::vec3(0.0f));
            meshNode->render(camera, projection, 0.0f, t, false);
        }
    }

    BoxShape::OBB BoxShape::BuildOBB(const glm::mat4 &modelMatrix) const {
        OBB obb{};

        obb.center = glm::vec3(modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // Osy v S*T*R jsou v prvních třech sloupcích, ale jsou škálované.
        obb.axes[0] = glm::normalize(glm::vec3(modelMatrix[0]));
        obb.axes[1] = glm::normalize(glm::vec3(modelMatrix[1]));
        obb.axes[2] = glm::normalize(glm::vec3(modelMatrix[2]));

        const auto scale = glm::vec3(
            glm::length(glm::vec3(modelMatrix[0])),
            glm::length(glm::vec3(modelMatrix[1])),
            glm::length(glm::vec3(modelMatrix[2]))
        );

        obb.halfExtents.x = (this->size.x * 0.5f) * scale.x;
        obb.halfExtents.y = (this->size.y * 0.5f) * scale.y;
        obb.halfExtents.z = (this->size.z * 0.5f) * scale.z;

        return obb;
    }

    AABB BoxShape::calculateAABB(const glm::mat4 &modelMatrix) {
        return CalculateAABB(modelMatrix, this->size * 0.5f);
    }

    void BoxShape::setSize(const glm::vec3 &size) {
        this->size = size;
        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
        auto boxMesh = make_shared<BoxMesh>(shader, size.x, size.y, size.z);
        boxMesh->setMaterial(material);
        meshNode = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
    }
} // Physic
