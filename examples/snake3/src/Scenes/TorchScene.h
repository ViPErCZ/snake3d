#ifndef SNAKE3_TORCHSCENE_H
#define SNAKE3_TORCHSCENE_H

#include <memory>

#include "Renderer/Opengl/Scene/Scene.h"
#include "Renderer/Opengl/Model/Standard/GPUParticle3D.h"
#include "Renderer/Opengl/Model/Standard/QuadMesh3D.h"

namespace Scenes {
    class TorchScene final : public Scene {
    public:
        TorchScene(
            const std::shared_ptr<DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<PointLight> > &pointLights,
            const std::shared_ptr<RenderManager> &rendererManager, const std::shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<ResourceManager> &rm, int width, int height
        );

        void init(int priority) override;

    protected:
        void initTorch();

        std::shared_ptr<Model::GPUParticle3D> initFire();
        std::shared_ptr<Model::GPUParticle3D> initSmoke();

        std::shared_ptr<Model::QuadMesh3D> quad;
        std::shared_ptr<Model::MeshNode3D> torchNode;
    };
} // Scenes

#endif //SNAKE3_TORCHSCENE_H
