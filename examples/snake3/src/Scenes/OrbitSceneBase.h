#ifndef SNAKE3_ORBITSCENEBASE_H
#define SNAKE3_ORBITSCENEBASE_H

#include "Renderer/Opengl/Scene/Scene.h"
#include <snake3d/Tools/CameraOrbit.h>

namespace Scenes {
    class OrbitSceneBase : public Scene {
    public:
        OrbitSceneBase(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager,
            const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection,
            const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

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
