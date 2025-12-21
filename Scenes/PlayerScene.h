#ifndef SNAKE3_PLAYERSCENE_H
#define SNAKE3_PLAYERSCENE_H

#include <memory>

#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "../Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h"
#include "../Renderer/Opengl/Scene/Scene.h"

using namespace Model;
using namespace Animations;

namespace Scenes {
    class PlayerScene final : public Scene {
    public:
        PlayerScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
                    const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] shared_ptr<SnakeMeshNode3D> getSnake() const;

        [[nodiscard]] shared_ptr<SnakeMoveHandler> getSnakeMoveHandler() const;

    protected:
        void initSnake();

        void initSnakeMoveHandler();

        void buildStartMoveCallback() const;

        void buildStopMoveCallback() const;

        shared_ptr<SnakeMeshNode3D> snake;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
    };
} // Scenes

#endif //SNAKE3_PLAYERSCENE_H
