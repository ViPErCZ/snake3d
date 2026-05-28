#ifndef SNAKE3_WEATHERSCENE_H
#define SNAKE3_WEATHERSCENE_H

#include <memory>

#include <snake3d/Renderer/Opengl/Model/Standard/QuadMesh3D.h>
#include <snake3d/Renderer/Opengl/Scene/Scene.h>

namespace Scenes {
    class WeatherScene final : public Scene {
    public:
        WeatherScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        bool isRainActive() const { return rainActive; }
    protected:
        void initRain(bool lightning = false);
        void initRainDrop();
        void initSnow();

        std::shared_ptr<Model::QuadMesh3D> quad;
        bool rainActive = false;
    };
} // Scenes

#endif //SNAKE3_WEATHERSCENE_H