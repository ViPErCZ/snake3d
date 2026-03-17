#ifndef SNAKE3_ORBITSCENEBASE_H
#define SNAKE3_ORBITSCENEBASE_H

#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Tools/CameraOrbit.h"

namespace Scenes {
    class OrbitSceneBase : public Scene {
    public:
        OrbitSceneBase(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;

    protected:
        void configureOrbit();
        virtual glm::vec3 getOrbitCenter() const;
        virtual float getOrbitSpeed() const;
        virtual float getOrbitBackOffset() const;
        virtual glm::vec3 getOrbitWorldUp() const;

        Tools::CameraOrbit cameraOrbit;
    };
} // Scenes

#endif // SNAKE3_ORBITSCENEBASE_H
