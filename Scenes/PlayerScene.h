#ifndef SNAKE3_PLAYERSCENE_H
#define SNAKE3_PLAYERSCENE_H

#include <memory>

#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "../Renderer/Opengl/Scene/Scene.h"

using namespace Model;

namespace Scenes {
    class PlayerScene final : public Scene {
    public:
        PlayerScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
                    const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init() override;

    protected:
        void initSnake();

        shared_ptr<SnakeMeshNode3D> snake;
    };
} // Scenes

#endif //SNAKE3_PLAYERSCENE_H
