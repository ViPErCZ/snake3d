#ifndef SNAKE3_COINSCENE_H
#define SNAKE3_COINSCENE_H

#include <memory>

#include "../Renderer/Opengl/Model/Game/CoinMeshNode3D.h"
#include <snake3d/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h>
#include <snake3d/Renderer/Opengl/Scene/Scene.h>

namespace Scenes {
    class CoinScene final : public Scene {
    public:
        CoinScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] std::shared_ptr<Model::CoinMeshNode3D> getCoin() const;
        [[nodiscard]] std::shared_ptr<Model::CoinMeshNode3D> getRemoveCoin() const;

        void update() override;

    protected:
        void initCoin();

        std::shared_ptr<Model::CoinMeshNode3D> coin;
        std::shared_ptr<Model::CoinMeshNode3D> removeCoin;
    };
} // Scenes

#endif //SNAKE3_COINSCENE_H
