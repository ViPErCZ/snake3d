#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>
#include "../Manager/LevelManager.h"
#include "../Renderer/Opengl/SnakeRenderer.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Material/Uniform/TextureUniform.h"

using namespace Uniform;
using namespace Physic;
using namespace Scenes;
using namespace std;

#define MAX_POINT 6
#define MAX_LIVES 4
#define START_LEVEL 2

namespace Scenes {
    class MainScene final : public Scene {
    public:
        explicit MainScene(const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init() override;

        void render() override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const override;

    private:
        void initSkybox();

        void initPlane();

        void initSnake();

        void initSnakeMoveHandler();

        void initBarriers();

        void initLevelManager();

        void initEat();

        shared_ptr<Snake> snake;
        shared_ptr<Barriers> barriers;
        shared_ptr<ObjWall> objWall;
        unique_ptr<LevelManager> levelManager;
        shared_ptr<CollisionDetector> collisionDetector;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
        shared_ptr<SnakeRenderer> snakeRenderer; // TODO: jen docasne dokud neprejde pod svoji scenu a standardRenderer
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
