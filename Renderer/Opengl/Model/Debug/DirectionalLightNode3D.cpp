#include "DirectionalLightNode3D.h"

namespace Model {
    DirectionalLightNode3D::DirectionalLightNode3D(const shared_ptr<ContextState> &contextState,
                                                   const shared_ptr<ShaderManager> &baseShader,
                                                   const shared_ptr<ResourceManager> &resourceManager)
        : LightNode3D(baseShader), MeshNode3D(contextState, arrowMesh, resourceManager) {
    }

    DirectionalLightNode3D::~DirectionalLightNode3D() = default;

    void DirectionalLightNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
        const glm::mat4 &parentTransform, const bool shadows) {

        const glm::mat4 model = calculateArrowTransform(directionalLight->getPosition(), -directionalLight->getDirection());

        contextState->setBlendingMode(mesh->getBlending());
        contextState->setDepthTest(mesh->getDepthTest());
        contextState->setDepthWrite(mesh->getDepthWrite());
        reinterpret_pointer_cast<WireframeArrowMesh>(mesh)->setColor(directionalLight->getAmbient());
        mesh->render(camera, projection, 1, model, shadows);
    }
} // Model
