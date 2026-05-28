#include "DirectionalLightNode3D.h"

using namespace std;
using namespace Manager;
using namespace Tools;
using namespace Lights;

namespace Model {
    DirectionalLightNode3D::DirectionalLightNode3D(const shared_ptr<ContextState> &contextState,
                                                   const shared_ptr<ShaderProgram> &baseShader,
                                                   const shared_ptr<ResourceManager> &resourceManager)
        : LightNode3D(baseShader), MeshNode3D(contextState, arrowMesh, resourceManager) {
    }

    DirectionalLightNode3D::~DirectionalLightNode3D() = default;

    void DirectionalLightNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
        const glm::mat4 &parentTransform, const bool shadows) {

        if (visible && orientableLight) {
            const glm::mat4 model = calculateArrowTransform(orientableLight->getPosition(), -orientableLight->getDirection());

            contextState->setBlendingMode(mesh->getBlending());
            contextState->setDepthTest(mesh->getDepthTest());
            contextState->setDepthWrite(mesh->getDepthWrite());
            static_pointer_cast<WireframeArrowMesh>(mesh)->setColor(orientableLight->getAmbient());
            mesh->render(camera, projection, 1, model, shadows);
        }
    }
} // Model
