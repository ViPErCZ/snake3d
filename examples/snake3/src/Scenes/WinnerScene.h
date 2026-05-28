#ifndef SNAKE3_WINNERSCENE_H
#define SNAKE3_WINNERSCENE_H

#include <memory>
#include <vector>

#include <snake3d/Renderer/Opengl/Model/Standard/QuadMesh3D.h>
#include "OrbitSceneBase.h"

namespace Model {
    class GPUParticle3D;
}

namespace Scenes {
    class WinnerScene final : public OrbitSceneBase {
    public:
        WinnerScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;
    protected:
        void initExplosion();
        static glm::vec3 randomExplosionPosition();

        std::shared_ptr<Model::QuadMesh3D> quad;
        std::vector<std::shared_ptr<Model::GPUParticle3D>> explosions;
        std::vector<float> explosionMoveTimers;
        float explosionCycleDuration = 0.0f;
    };
} // Scenes

#endif //SNAKE3_WINNERSCENE_H
