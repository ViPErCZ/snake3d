#ifndef SNAKE3_WINNERSCENE_H
#define SNAKE3_WINNERSCENE_H

#include <memory>
#include <vector>

#include "Renderer/Opengl/Model/Standard/QuadMesh3D.h"
#include "OrbitSceneBase.h"

namespace Model {
    class GPUParticle3D;
}

using namespace Model;
using namespace Tools;
using namespace std;

namespace Scenes {
    class WinnerScene final : public OrbitSceneBase {
    public:
        WinnerScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;
    protected:
        void initExplosion();
        static glm::vec3 randomExplosionPosition();

        shared_ptr<QuadMesh3D> quad;
        std::vector<std::shared_ptr<GPUParticle3D>> explosions;
        std::vector<float> explosionMoveTimers;
        float explosionCycleDuration = 0.0f;
    };
} // Scenes

#endif //SNAKE3_WINNERSCENE_H
