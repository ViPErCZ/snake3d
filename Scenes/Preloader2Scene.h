#ifndef SNAKE3_PRELOADER2SCENE_H
#define SNAKE3_PRELOADER2SCENE_H

#include "../Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class Preloader2Scene : public Scene {
    public:
        Preloader2Scene(
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

#endif //SNAKE3_PRELOADER2SCENE_H
