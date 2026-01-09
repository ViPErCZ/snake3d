#ifndef SNAKE3_TORCHSCENE_H
#define SNAKE3_TORCHSCENE_H

#include <memory>

#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Standard/GPUParticle3D.h"
#include "../Renderer/Opengl/Model/Standard/QuadMesh3D.h"

using namespace Model;
using namespace std;

namespace Scenes {
    class TorchScene final : public Scene {
    public:
        TorchScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

    protected:
        void initTorch();

        shared_ptr<GPUParticle3D> initFire();
        shared_ptr<GPUParticle3D> initSmoke();

        shared_ptr<QuadMesh3D> quad;
        shared_ptr<MeshNode3D> torchNode;
    };
} // Scenes

#endif //SNAKE3_TORCHSCENE_H
