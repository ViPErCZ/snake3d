#ifndef SNAKE3_PRELOADERSCENE_H
#define SNAKE3_PRELOADERSCENE_H

#include "../Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class PreloaderScene : public Scene {
    public:
        PreloaderScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

    protected:
        [[nodiscard]] shared_ptr<MeshNode2D> initPreloader() const;
    };
} // Scenes

#endif //SNAKE3_PRELOADERSCENE_H
