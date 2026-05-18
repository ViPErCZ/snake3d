#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>

#include "BarriersScene.h"
#include "CoinScene.h"
#include "PlayerScene.h"
#include "Preloader2Scene.h"
#include "RemoteSnakeScene.h"
#include "WinnerScene.h"
#include "MainMenuScene.h"
#include "../Manager/EatManager.h"
#include "../Manager/TextureManager.h"
#include "../Renderer/Opengl/Material/MaterialInstance.h"
#include "../Renderer/Opengl/Material/Feature/HoleMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/PlanarReflectionFeature.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Network/Game/NetGameController.h"
#include "../Network/Game/NetGameSnapshot.h"
#include "SceneHud.h"

using namespace Uniform;
using namespace Physic;
using namespace Scenes;
using namespace std;

namespace Scenes {
    class MainScene final : public Scene, public Net::NetWorldSource, public Net::NetWorldSink {
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

        void resize(int width, int height, const glm::mat4 &projection) override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) override;
        void setCursorPosition(const glm::vec2 &position) const;
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        [[nodiscard]] bool isMenuVisible() const;

    private:
        bool collectWorldSnapshot(Net::WorldSnapshotState &out) const override;

        void onClientHello(uint32_t peerId) override;
        void onWelcomeReceived() override;
        void onPeerDisconnected() override;
        void onRemoteInput(int8_t moveX, int8_t moveY, uint8_t actions) override;

        void requestLocalCrash() override;
        void requestRemoteCrash() override;
        void scheduleLocalRespawnAfterCrash(const std::vector<glm::vec3> &positions,
                                            SnakeMeshNode3D::eDIRECTION direction) override;
        void applyLocalSnakePositions(const Net::SnakeSnapshotState &snake) override;
        void applyRemoteSnakePositions(const Net::SnakeSnapshotState &snake) override;

        void applyCoin(float x, float y, bool visible, bool eatenAnim) override;
        void applyHud(uint32_t level, uint32_t eatCounter, uint32_t lives) override;
        void applyWinning() override;

        void initSounds() const;

        void initLights();

        void initPlayerScene();
        void initRemoteSnakeScene();

        void initBarriersScene();

        void initCoinScene();

        void initTorchScene();

        void initWeatherScene();

        void initSkybox();

        void initPlane();

        void initEatManager();
        void handleCoinEaten(const EatManager &manager);
        void initializeMultiplayerState();
        void shutdownMultiplayerState(bool showMenu);
        void rebuildRadarItems(bool includeRemote) const;
        void respawnLocalSnake();
        void respawnRemoteSnake();
        [[nodiscard]] bool localSnakeHitRemote() const;
        [[nodiscard]] bool remoteSnakeHitLocal() const;
        [[nodiscard]] glm::vec3 findRemoteSpawnPosition() const;
        static std::vector<glm::vec3> collectSnakePositions(const shared_ptr<SnakeMeshNode3D> &snake);

        void initPreloader();
        void initMainMenu();

        void buildEatenUpCallback();

        void buildStartMoveCallback() const;

        void buildCrashCallback();

        void prepareScene();

        void applyHolesToPlane();

        void nextLevel();
        void showMenu(MainMenuScene::PrimaryAction action);
        void hideMenu();
        void initNetworking();
        void resetNetworkState();
        void startNetworkGame();
        void enterWinningState();

        shared_ptr<PlayerScene> playerScene;
        shared_ptr<RemoteSnakeScene> remoteSnakeScene;
        shared_ptr<CoinScene> coinScene;
        shared_ptr<BarriersScene> barriersScene;
        shared_ptr<EatLocationHandler> eatLocationHandler;
        shared_ptr<EatLocationHandler> remoteEatLocationHandler;
        unique_ptr<EatManager> eatManager;
        unique_ptr<EatManager> remoteEatManager;
        shared_ptr<LevelManager> levelManager;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
        unique_ptr<SceneHud> hud;
        // B5c: plane is now a builder-built MaterialInstance. Feature handles
        // are kept so runtime mutations (applyHolesToPlane texture rebuild,
        // F2 reflection toggle) can poke the specific feature without rebuilding
        // the whole material.
        shared_ptr<Material::MaterialInstance> planeMaterial;
        shared_ptr<Feature::HoleMapFeature> planeHoleMapFeature;
        shared_ptr<Feature::PlanarReflectionFeature> planeReflectionFeature;
        shared_ptr<Manager::TextureManager> holeMapTexture;
        shared_ptr<Preloader2Scene> preLoader;
        shared_ptr<WinnerScene> winnerScene;
        shared_ptr<MainMenuScene> mainMenuScene;
        glm::mat4 ortho{};
        bool loading = true;
        bool winning = false;
        bool menuVisible = false;
        bool gameStarted = false;
        bool multiplayerCrashInProgress = false;
        bool resumeLocalMovementAfterMenu = false;
        int progress = 0;
        Net::NetGameController netSession;
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
