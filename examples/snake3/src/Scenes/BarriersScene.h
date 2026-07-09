#ifndef SNAKE3_BARRIERSSCENE_H
#define SNAKE3_BARRIERSSCENE_H

#include "../Manager/LevelManager.h"
#include <snake3d/Renderer/Opengl/Scene/Scene.h>

namespace Scenes {
    class BarriersScene final : public Scene {
    public:
        BarriersScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] std::shared_ptr<Manager::LevelManager> getLevelManager() const;

        [[nodiscard]] std::shared_ptr<Model::MeshNode3D> getLevelBoxes() const;

        void nextLevel(int targetLevel = -1);

    protected:
        void initBarriers();

        void initLevelManager();

        std::shared_ptr<Manager::LevelManager> levelManager;
        std::shared_ptr<Model::MeshNode3D> levelBoxes;
        std::shared_ptr<Model::MeshNode3D> perimeterBoxes;
    };
} // Scenes

#endif //SNAKE3_BARRIERSSCENE_H
