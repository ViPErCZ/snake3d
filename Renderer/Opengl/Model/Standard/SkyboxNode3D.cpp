#include "SkyboxNode3D.h"

#include "BoxMesh.h"
#include "../../Material/ShaderMaterial.h"
#include "../../Material/Uniform/CallbackUniform.h"
#include "../../Material/Uniform/TextureUniform.h"

namespace Model {
    SkyboxNode3D::SkyboxNode3D(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<Camera> &camera)
        : MeshNode3D(
            contextState,
            make_shared<BoxMesh>(resourceManager->getShader("skyboxShader"), 2.0f, 2.0f, 2.0f),
            nullptr
        ) {

        const auto skyboxMaterial = make_shared<ShaderMaterial>(resourceManager->getShader("skyboxShader"));
        const auto textureUniform = make_shared<TextureUniform>(0, resourceManager->getTexture("skybox"), true);
        CallbackUniform::CallbackType viewFunc = [camera](const string &name, const shared_ptr<ShaderManager> &shader) {
            const auto view = glm::mat4(glm::mat3(camera->getViewMatrix()));
            shader->setMat4(name, view);
        };
        const auto viewUniform = make_shared<CallbackUniform>(viewFunc);
        skyboxMaterial->addUniform("view", viewUniform);
        skyboxMaterial->addUniform("skybox", textureUniform);
        const auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), {1.0, 0.0, 0.0});
        skyboxMaterial->addUniform("model", model);
        mesh->setMaterial(skyboxMaterial);
        disablePlanarReflection();
    }

    void SkyboxNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform, const bool shadows) {
        if (visible) {
            contextState->setBlendingMode(mesh->getBlending());
            contextState->setDepthTest(mesh->getDepthTest());
            contextState->setDepthWrite(mesh->getDepthWrite());
            contextState->setDepthFunc(DepthFunc::Lequal);
            mesh->render(camera, projection, 1, glm::mat4(1.0f), false);
            contextState->setDepthFunc(DepthFunc::Less);
        }
    }
} // Model