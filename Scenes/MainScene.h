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
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeInUniform.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"
#include "../Network/NetClientServer.h"
#include "../Network/NetClock.h"
#include "../Network/Game/NetGameSnapshot.h"

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

        void resize(int width, int height, const glm::mat4 &projection) override;

        void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) override;
        void setCursorPosition(const glm::vec2 &position) const;
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        [[nodiscard]] bool isMenuVisible() const;

    private:
        struct PendingRespawnState {
            std::vector<glm::vec2> positions;
            size_t segmentCount = 0;
            SnakeMeshNode3D::eDIRECTION direction = SnakeMeshNode3D::NONE;
            bool active = false;
        };

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
        void notifyNetworkDisconnect() const;
        void rebuildRadarItems(bool includeRemote) const;
        void sendClientPauseToggle() const;
        void respawnLocalSnake();
        void respawnRemoteSnake();
        [[nodiscard]] bool localSnakeHitRemote() const;
        [[nodiscard]] bool remoteSnakeHitLocal() const;
        [[nodiscard]] glm::vec3 findRemoteSpawnPosition() const;
        static std::vector<glm::vec2> collectSnakePositions(const shared_ptr<SnakeMeshNode3D> &snake);

        void initRadar();

        void initLabels();

        void initPreloader();
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
        void initNetworking();
        void updateNetworking();
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
        bool loading = true;
        bool winning = false;
        bool menuVisible = false;
        bool gameStarted = false;
        bool hudStateSaved = false;
        bool hudHelpVisible = false;
        bool hudTilesVisible = false;
        bool hudRadarVisible = false;
        bool multiplayerCrashInProgress = false;
        bool resumeLocalMovementAfterMenu = false;
        int progress = 0;
        uint16_t netPort = 7777;
        Net::NetManager netManager;
        Net::NetClient netClient;
        Net::NetServer netServer;
        Net::NetClock netClock{60};
        bool netEnabled = false;
        bool netIsServer = false;
        bool netIsClient = false;
        uint32_t netPeerId = 0;
        uint32_t netSeed = 0;
        uint32_t netLastSnapshotLevel = 0;
        uint32_t netLastSnapshotEatCounter = 0;
        uint32_t localRespawnSerial = 0;
        uint32_t remoteRespawnSerial = 0;
        uint32_t netLastSeenLocalRespawnSerial = 0;
        uint32_t netLastSeenRemoteRespawnSerial = 0;
        bool netLastSeenLocalCrash = false;
        bool netLastSeenRemoteCrash = false;
        glm::vec3 localMultiplayerSpawnPos{23.0f, -3.0f, -23.0f};
        PendingRespawnState pendingLocalRespawn;
    };
} // Scenes

#endif //SNAKE3_MAINSCENE_H
