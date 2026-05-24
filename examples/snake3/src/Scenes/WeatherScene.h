#ifndef SNAKE3_WEATHERSCENE_H
#define SNAKE3_WEATHERSCENE_H

#include <memory>

#include "Renderer/Opengl/Model/Standard/QuadMesh3D.h"
#include "Renderer/Opengl/Scene/Scene.h"

using namespace std;

namespace Scenes {
    class WeatherScene final : public Scene {
    public:
        WeatherScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        bool isRainActive() const { return rainActive; }
    protected:
        void initRain(bool lightning = false);
        void initRainDrop();
        void initSnow();

        shared_ptr<QuadMesh3D> quad;
        bool rainActive = false;
    };
} // Scenes

#endif //SNAKE3_WEATHERSCENE_H