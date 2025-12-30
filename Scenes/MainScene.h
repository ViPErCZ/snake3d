#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>

#include "BarriersScene.h"
#include "CoinScene.h"
#include "PlayerScene.h"
#include "../Handler/Debug/PositionHandler.h"
#include "../Manager/EatManager.h"
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeInUniform.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

using namespace Uniform;
using namespace Physic;
using namespace Scenes;
using namespace std;

namespace Scenes {
    class MainScene final : public Scene {
    public:
        explicit MainScene(const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        void update() override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const override;

    private:
        void initPlayerScene();

        void initBarriersScene();

        void initCoinScene();

        void initSkybox();

        void initPlane();

        void initEatManager();

        void initRadar();

        void initLabels();

        void buildEatenUpCallback() const;

        void buildStartMoveCallback() const;

        void buildCrashCallback() const;

        shared_ptr<PlayerScene> playerScene;
        shared_ptr<CoinScene> coinScene;
        shared_ptr<BarriersScene> barriersScene;
        unique_ptr<EatManager> eatManager;
        shared_ptr<LevelManager> levelManager;
        shared_ptr<CollisionDetector> collisionDetector;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
        shared_ptr<FadeOutUniform> fadeOutUniform;
        shared_ptr<FadeInUniform> fadeInUniform;
        shared_ptr<LabelNode2D> tilesCounterText;
        shared_ptr<MeshNode2D> helpText;
        shared_ptr<MeshNode2D> tilesCounterNode;
        shared_ptr<ShaderMaterial> radarExpansionIn;
        shared_ptr<ShaderMaterial> radarExpansionOut;
        shared_ptr<FadeInUniform> radarFadeInUniform;
        shared_ptr<FadeOutUniform> radarFadeOutUniform;
        shared_ptr<QuadNode2D> radarNode;
        shared_ptr<RadarMeshNode2D> radarMeshNode;
        shared_ptr<PositionHandler> positionHandler;
        shared_ptr<PlanarReflectionMaterial> planeMaterial;
        shared_ptr<DirectionalLight> planeMaterialDirLight;
        glm::mat4 ortho{};
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
