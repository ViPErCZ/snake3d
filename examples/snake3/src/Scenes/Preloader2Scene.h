#ifndef SNAKE3_PRELOADER2SCENE_H
#define SNAKE3_PRELOADER2SCENE_H

#include "Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class Preloader2Scene : public Scene {
    public:
        Preloader2Scene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;

    protected:
        [[nodiscard]] std::shared_ptr<Model::MeshNode2D> initPreloader() const;
    };
} // Scenes

#endif //SNAKE3_PRELOADER2SCENE_H
