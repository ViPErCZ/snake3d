#include "MarkRingNode3D.h"

#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Uniform/CallbackUniform.h"
#include "Renderer/Opengl/Model/Standard/QuadMesh3D.h"

using namespace Material;

namespace Model {
    MarkRingNode3D::MarkRingNode3D(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<ManipulatorHandler> &manipulatorHandler)
            : MeshNode3D(contextState, nullptr, resourceManager), manipulatorHandler(manipulatorHandler)
    {
    }

    void MarkRingNode3D::init() {
        material = make_shared<ShaderMaterial>(resourceManager->getShader("markRingShader"));
        material->setBlending(Blending::Translucent);
        CallbackUniform::CallbackType pulseFunc = [](const string &name, const shared_ptr<ShaderProgram> &shader) {
            const auto time = static_cast<float>(glfwGetTime());
            const float pulse = 0.5f + 0.5f * sin(time * 4.0f);

            shader->setFloat(name, pulse);
        };
        const auto pulseUniform = make_shared<CallbackUniform>(pulseFunc);
        material->setUniform("pulse", pulseUniform);
        material->setUniform("radius", 0.1f);

        mesh = make_shared<QuadMesh3D>(resourceManager->getShader("markRingShader"), 1, 1);
        mesh->setMaterial(material);
        mesh->setBlending(Blending::Translucent);
        setPosition(glm::vec3(0.0, 0.5, 0.0));
    }

    void MarkRingNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
        const glm::mat4 &parentTransform, const bool shadows) {

        if (manipulatorHandler->isActive()) {
            const auto worldMin = manipulatorHandler->getWorldMin();
            const auto worldMax = manipulatorHandler->getWorldMax();
            const auto worldCenter = manipulatorHandler->getItemWorldCenter();

            setPosition(glm::vec3(worldCenter.x, worldCenter.y, worldMin.z + 0.002f));

            const float sizeX = (worldMax.x - worldMin.x);
            const float sizeY = (worldMax.y - worldMin.y);
            float radius = glm::max(sizeX, sizeY) * 0.8f;

            material->setUniform("radius", radius);

            auto color = glm::vec3(1.0f, 1.0f, 0.0f);

            if (manipulatorHandler->isScaleHandlerActive()) {
                color = glm::vec3(0.0f, 0.2f, 0.8f);
            } else if (manipulatorHandler->isRotationHandlerActive()) {
                color = glm::vec3(0.8f, 0.2f, 0.0f);
            }

            material->setUniform("color", color);

            MeshNode3D::render(camera, projection, dt, parentTransform, shadows);
        }
    }

    void MarkRingNode3D::renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform) const {
    }

} // Model