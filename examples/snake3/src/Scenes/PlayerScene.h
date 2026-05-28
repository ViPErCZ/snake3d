#ifndef SNAKE3_PLAYERSCENE_H
#define SNAKE3_PLAYERSCENE_H

#include <memory>

#include "Physic/Dynamics/DynamicBody.h"
#include "../Handler/SnakeMoveHandler.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h"
#include "Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class PlayerScene final : public Scene {
    public:
        PlayerScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager, const std::shared_ptr<Manager::Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] std::shared_ptr<Model::SnakeMeshNode3D> getSnake() const;

        [[nodiscard]] std::shared_ptr<Handler::SnakeMoveHandler> getSnakeMoveHandler() const;

        void winning() const;
        void setInputEnabled(bool enabled) const;

    protected:
        void initSnake();

        void initSnakeMoveHandler();

        void buildStartMoveCallback() const;

        void buildStopMoveCallback() const;

        std::shared_ptr<Model::SnakeMeshNode3D> snake;
        std::shared_ptr<Handler::SnakeMoveHandler> snakeMoveHandler;
        std::shared_ptr<Physic::Dynamics::DynamicBody> snakeBody;
    };
} // Scenes

#endif //SNAKE3_PLAYERSCENE_H
