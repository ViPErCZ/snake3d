#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>

#include "BarriersScene.h"
#include "CoinScene.h"
#include "PlayerScene.h"
#include "Preloader2Scene.h"
#include "WinnerScene.h"
#include "MainMenuScene.h"
#include "../Manager/EatManager.h"
#include "../Manager/TextureManager.h"
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeInUniform.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/ImageNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"
#include "../Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h"

using namespace Uniform;
using namespace Physic;
using namespace Scenes;
using namespace std;

namespace Scenes {
    class MainScene final : public Scene {
    public:
        explicit MainScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        void update() override;

        void physics() override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const override;
        void setCursorPosition(const glm::vec2 &position);
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        [[nodiscard]] bool isMenuVisible() const;

    private:
        void initSounds() const;

        void initLights();

        void initPlayerScene();

        void initBarriersScene();

        void initCoinScene();

        void initTorchScene();

        void initWeatherScene();

        void initSkybox();

        void initPlane();

        void initEatManager();

        void initRadar();

        void initLabels();

        void initPreloader();
        void initCursor();
        void initMainMenu();

        void buildEatenUpCallback();

        void buildStartMoveCallback() const;

        void buildCrashCallback();

        void prepareScene();

        void nextLevel();
        void showMenu(MainMenuScene::PrimaryAction action);
        void hideMenu();
        void saveHudVisibility();
        void restoreHudVisibility();

        shared_ptr<PlayerScene> playerScene;
        shared_ptr<CoinScene> coinScene;
        shared_ptr<BarriersScene> barriersScene;
        shared_ptr<EatLocationHandler> eatLocationHandler;
        unique_ptr<EatManager> eatManager;
        shared_ptr<LevelManager> levelManager;
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
        shared_ptr<PlanarReflectionMaterial> planeMaterial;
        shared_ptr<Preloader2Scene> preLoader;
        shared_ptr<WinnerScene> winnerScene;
        shared_ptr<MainMenuScene> mainMenuScene;
        glm::mat4 ortho{};
        shared_ptr<ImageNode2D> cursorMesh;
        shared_ptr<MeshNode2D> cursorNode;
        shared_ptr<ParticleProcessMaterial> cursorTrailMaterial;
        shared_ptr<GPUParticle2D> cursorTrail;
        shared_ptr<TextureManager> cursorTexture;
        glm::vec2 cursorScreenPos{0.0f, 0.0f};
        glm::vec2 lastCursorScreenPos{0.0f, 0.0f};
        glm::vec2 cursorSize{0.0f, 0.0f};
        glm::vec2 cursorHotspot{0.0f, 0.0f};
        bool cursorInitialized = false;
        bool hasCursorLastPos = false;
        bool loading = true;
        bool winning = false;
        bool menuVisible = false;
        bool gameStarted = false;
        bool hudStateSaved = false;
        bool hudHelpVisible = false;
        bool hudTilesVisible = false;
        bool hudRadarVisible = false;
        int progress = 0;
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
