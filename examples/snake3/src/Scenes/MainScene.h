#ifndef SNAKE3_MAINSCENE_H
#define SNAKE3_MAINSCENE_H

#include <memory>

#include "BarriersScene.h"
#include "CoinScene.h"
#include "PlayerScene.h"
#include "Preloader2Scene.h"
#include "RemoteSnakeScene.h"
#include "WeatherScene.h"
#include "WinnerScene.h"
#include "MainMenuScene.h"
#include "../Manager/EatManager.h"
#include "Manager/TextureManager.h"
#include "Renderer/Opengl/Material/MaterialInstance.h"
#include "Renderer/Opengl/Material/Feature/HoleMapFeature.h"
#include "Renderer/Opengl/Material/Feature/PlanarReflectionFeature.h"
#include "Renderer/Opengl/Material/Feature/RainRippleFeature.h"
#include "Renderer/Opengl/Scene/Scene.h"
#include "../Network/Game/NetGameController.h"
#include "../Network/Game/NetGameSnapshot.h"
#include "SceneHud.h"

namespace Scenes {
    class MainScene final : public Scene, public Net::NetWorldSource, public Net::NetWorldSink {
    public:
        explicit MainScene(
            const std::shared_ptr<DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<PointLight> > &pointLights,
            const std::shared_ptr<RenderManager> &rendererManager,
            const std::shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const std::shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        void update() override;

        void physics() override;

        void resize(int width, int height, const glm::mat4 &projection) override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) override;
        void setCursorPosition(const glm::vec2 &position) const;
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        [[nodiscard]] bool isMenuVisible() const;

        bool collectWorldSnapshot(Net::WorldSnapshotState &out) const override;

        // Expose collisionSystem for debug-UI tooling (ImGuiOverlay needs it to
        // pause DynamicBody when user selects its CollisionShape3D). Scene base
        // class stores collisionSystem protected; getter sits on the concrete
        // scene to avoid touching the parked Scene.h header.
        [[nodiscard]] std::shared_ptr<Physic::CollisionSystem3D> getCollisionSystem() const { return collisionSystem; }

        void onClientHello(uint32_t peerId) override;
        void onWelcomeReceived() override;
        void onPeerDisconnected() override;
        void onRemoteInput(int8_t moveX, int8_t moveY, uint8_t actions) override;

        void requestLocalCrash() override;
        void requestRemoteCrash() override;
        void scheduleLocalRespawnAfterCrash(const std::vector<glm::vec3> &positions,
                                            Model::SnakeMeshNode3D::eDIRECTION direction) override;
        void applyLocalSnakePositions(const Net::SnakeSnapshotState &snake) override;
        void applyRemoteSnakePositions(const Net::SnakeSnapshotState &snake) override;

        void applyCoin(float x, float y, bool visible, bool eatenAnim) override;
        void applyHud(uint32_t level, uint32_t eatCounter, uint32_t lives) override;
        void applyWinning() override;

    private:
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
        void handleCoinEaten(const Manager::EatManager &manager);
        void initializeMultiplayerState();
        void shutdownMultiplayerState(bool showMenu);
        void rebuildRadarItems(bool includeRemote) const;
        void respawnLocalSnake();
        void respawnRemoteSnake();
        [[nodiscard]] bool localSnakeHitRemote() const;
        [[nodiscard]] bool remoteSnakeHitLocal() const;
        [[nodiscard]] glm::vec3 findRemoteSpawnPosition() const;
        static std::vector<glm::vec3> collectSnakePositions(const std::shared_ptr<Model::SnakeMeshNode3D> &snake);

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

        std::shared_ptr<PlayerScene> playerScene;
        std::shared_ptr<RemoteSnakeScene> remoteSnakeScene;
        std::shared_ptr<CoinScene> coinScene;
        std::shared_ptr<BarriersScene> barriersScene;
        std::shared_ptr<Handler::EatLocationHandler> eatLocationHandler;
        std::shared_ptr<Handler::EatLocationHandler> remoteEatLocationHandler;
        std::unique_ptr<Manager::EatManager> eatManager;
        std::unique_ptr<Manager::EatManager> remoteEatManager;
        std::shared_ptr<Manager::LevelManager> levelManager;
        std::shared_ptr<Handler::SnakeMoveHandler> snakeMoveHandler;
        std::unique_ptr<SceneHud> hud;
        std::shared_ptr<Material::MaterialInstance> planeMaterial;
        std::shared_ptr<Feature::HoleMapFeature> planeHoleMapFeature;
        std::shared_ptr<Feature::PlanarReflectionFeature> planeReflectionFeature;
        std::shared_ptr<Feature::RainRippleFeature> planeRainRippleFeature;
        std::shared_ptr<WeatherScene> weatherScene;
        std::shared_ptr<Manager::TextureManager> holeMapTexture;
        std::shared_ptr<Preloader2Scene> preLoader;
        std::shared_ptr<WinnerScene> winnerScene;
        std::shared_ptr<MainMenuScene> mainMenuScene;
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
