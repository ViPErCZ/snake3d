#ifndef SNAKE3_MARKRINGNODE3D_H
#define SNAKE3_MARKRINGNODE3D_H

#include <memory>

#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include "Handler/Debug/ManipulatorHandler.h"
#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Uniform/CallbackUniform.h"

namespace Model {
    class MarkRingNode3D : public MeshNode3D {
    public:
        explicit MarkRingNode3D(
            const std::shared_ptr<Tools::ContextState> &contextState,
            const std::shared_ptr<Manager::ResourceManager> &resourceManager,
            const std::shared_ptr<Handler::Debug::ManipulatorHandler> &manipulatorHandler);

        void init();

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void renderShadows(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
            const glm::mat4 &parentTransform) const override;

    private:
        std::shared_ptr<Material::ShaderMaterial> material;
        std::shared_ptr<Handler::Debug::ManipulatorHandler> manipulatorHandler;
    };
} // Model

#endif //SNAKE3_MARKRINGNODE3D_H
