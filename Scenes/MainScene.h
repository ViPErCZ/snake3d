#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>

#include "PlayerScene.h"
#include "../Manager/EatManager.h"
#include "../Manager/LevelManager.h"
#include "../Renderer/Opengl/SnakeRenderer.h"
#include "../Renderer/Opengl/Material/Uniform/FadeInUniform.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Game/CoinMeshNode3D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"

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

        void update() override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const override;

    private:
        void initPlayerScene();

        void initSkybox();

        void initPlane();

        void initBarriers();

        void initLevelManager();

        void initEat();

        void initEatManager();

        void initRadar();

        void initLabels();

        void resetRadar() const;

        void buildEatenUpCallback() const;

        void buildStartMoveCallback() const;

        void buildCrashCallback() const;

        shared_ptr<PlayerScene> playerScene;
        unique_ptr<LevelManager> levelManager;
        unique_ptr<EatManager> eatManager;
        shared_ptr<Radar> radar;
        shared_ptr<CoinMeshNode3D> coinMeshNode3D;
        shared_ptr<CollisionDetector> collisionDetector;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
        shared_ptr<SnakeRenderer> snakeRenderer; // TODO: jen docasne dokud neprejde pod svoji scenu a standardRenderer
        shared_ptr<MeshNode3D> levelBoxes;
        shared_ptr<FadeOutUniform> fadeOutUniform;
        shared_ptr<FadeInUniform> fadeInUniform;
        shared_ptr<LabelNode2D> tilesCounterText;
        shared_ptr<MeshNode2D> helpText;
        shared_ptr<MeshNode2D> tilesCounterNode;
        glm::mat4 ortho{};
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
