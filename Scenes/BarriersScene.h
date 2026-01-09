#ifndef SNAKE3_BARRIERSSCENE_H
#define SNAKE3_BARRIERSSCENE_H

#include "../Manager/LevelManager.h"
#include "../Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class BarriersScene final : public Scene {
    public:
        BarriersScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] shared_ptr<LevelManager> getLevelManager() const;

        [[nodiscard]] shared_ptr<MeshNode3D> getLevelBoxes() const;

    protected:
        void initBarriers();

        void initLevelManager();

        shared_ptr<LevelManager> levelManager;
        shared_ptr<MeshNode3D> levelBoxes;
    };
} // Scenes

#endif //SNAKE3_BARRIERSSCENE_H
