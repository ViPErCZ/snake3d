#include "MainScene.h"

#include <algorithm>
#include <glm/gtc/random.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include <GLFW/glfw3.h>

#include "../Network/Game/NetUtils.h"
#include "../Network/NetDispatcher.h"
#include "../Network/Game/SnakeSnapshotApplier.h"
#include "PlayerScene.h"
#include "SceneLightFactory.h"
#include "TorchScene.h"
#include "WeatherScene.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h"
#include "../Renderer/Opengl/Model/Game/MarkRingNode3D.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include "../Renderer/Opengl/Model/Standard/SkyboxNode3D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"
#include "../Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "../Tools/Layers.h"

namespace Scenes {
    MainScene::MainScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
        collisionSystem = make_shared<CollisionSystem3D>();
    }

    void MainScene::init(const int priority) {
        Scene::init(priority);

        initPreloader();
        initMainMenu();
        initNetworking();
        prepareScene();

        if constexpr (isDebug) {
            manipulatorHandler = make_shared<ManipulatorHandler>(contextState, resourceManager, camera);
            std::cout << "Manipulator initialized" << std::endl;

            keyboardManager->addEventHandler(manipulatorHandler);

            const auto markRing = make_shared<MarkRingNode3D>(contextState, resourceManager, manipulatorHandler);
            markRing->init();
            addMeshNode3D(markRing);
        }
    }

    void MainScene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) {
        if (keyCode == GLFW_KEY_B && action == GLFW_PRESS) {
            rendererManager->toggleBloom();
            return;
        }

        if (winning) {
            if (keyCode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                if (netSession.isEnabled()) {
                    netSession.notifyDisconnect();
                }
                glfwSetWindowShouldClose(window, true);
            }
            return;
        }

        if (keyCode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            if (menuVisible) {
                if (gameStarted) {
                    this->hideMenu();
                }
            } else if (gameStarted) {
                if (netSession.isEnabled() && netSession.isClient()) {
                    const auto snake = playerScene ? playerScene->getSnake() : nullptr;
                    this->resumeLocalMovementAfterMenu =
                        snake && snake->getDirection() > SnakeMeshNode3D::STOP && snake->getDirection() < SnakeMeshNode3D::CRASH;
                    netSession.sendPauseToggle();
                } else if (snakeMoveHandler) {
                    this->resumeLocalMovementAfterMenu = !snakeMoveHandler->isStopped();
                    snakeMoveHandler->setStopped(true);
                }
                this->showMenu(MainMenuScene::PrimaryAction::Resume);
            }
            return;
        }

        if (menuVisible) {
            if (action == GLFW_PRESS && mainMenuScene) {
                mainMenuScene.get()->handleKeyInput(keyCode, action);
                if (mainMenuScene.get()->consumeJoinRequest()) {
                    if (netSession.isEnabled()) {
                        this->resetNetworkState();
                    }
                    const std::string ip = mainMenuScene->getJoinIp();
                    if (netSession.join(ip, "Player")) {
                        mainMenuScene->setNetworkStatus("Status: connecting");
                        mainMenuScene->setNetworkSessionState(MainMenuScene::NetworkSessionState::Client);
                        if constexpr (isDebug) {
                            std::cout << "[Net] Joining " << ip << ":" << netSession.getPort() << std::endl;
                        }
                    } else {
                        mainMenuScene->setNetworkStatus("Status: join failed");
                        if constexpr (isDebug) {
                            std::cout << "[Net] Failed to join " << ip << ":" << netSession.getPort() << std::endl;
                        }
                    }
                }
            }
            return;
        }

        if (action != GLFW_PRESS) {
            Scene::keyboardInput(window, keyCode, scancode, action, mods);
            return;
        }

        if (netSession.isEnabled() && netSession.isClient()) {
            Net::InputMsg input{};
            input.tick = netSession.getTick();
            input.actions = 0;
            switch (keyCode) {
                case GLFW_KEY_J: input.moveX = -1; input.moveY = 0; break;
                case GLFW_KEY_L: input.moveX = 1; input.moveY = 0; break;
                case GLFW_KEY_I: input.moveX = 0; input.moveY = 1; break;
                case GLFW_KEY_K: input.moveX = 0; input.moveY = -1; break;
                case GLFW_KEY_SPACE: input.actions = 1; break;
                default: break;
            }
            if (input.moveX != 0 || input.moveY != 0 || input.actions != 0) {
                const bool sent = netSession.sendInput(input);
                if (!sent) {
                    if constexpr (isDebug) {
                        std::cout << "[Net] Failed to send input to server" << std::endl;
                    }
                }
                return;
            }
        }

        Scene::keyboardInput(window, keyCode, scancode, action, mods);

        switch (keyCode) {
            case GLFW_KEY_V:
                rendererManager->toggleShadows();
                break;
            case GLFW_KEY_F2:
                if (planeMaterial) {
                    rendererManager->toggleReflections();
                    planeMaterial->setReflectionEnabled(rendererManager->isReflectionsEnabled());
                }
                break;
            case GLFW_KEY_F:
                rendererManager->toggleFog();
                break;
            case GLFW_KEY_M:
                if (soundManager->isPlaying("music")) {
                    soundManager->pause("music");
                } else {
                    soundManager->play("music", {{AL_LOOPING, AL_TRUE}});
                }
                break;
            case GLFW_KEY_N:
                if (playerScene) {
                    playerScene->getSnake()->respawn();
                }
                break;
            case GLFW_KEY_R:
                if (radarMeshNode) {
                    if (radarMeshNode->isVisible()) {
                        radarNode->setMaterial(radarExpansionOut);
                        radarFadeOutUniform->start();
                        radarMeshNode->hideItems();
                    } else {
                        radarFadeOutUniform->setAlpha(1.0);
                        radarMeshNode->setVisible(true);
                        radarNode->setMaterial(radarExpansionIn);
                        radarFadeInUniform->start();
                    }
                }
                break;
            default:
                break;
        }
    }

    void MainScene::physics() {
        if (netSession.isEnabled() && netSession.isClient()) {
            return;
        }
        if (menuVisible && !(netSession.isEnabled() && netSession.isServer())) {
            return;
        }
        Scene::physics();
    }

    void MainScene::resize(const int width, const int height, const glm::mat4 &projection) {
        Scene::resize(width, height, projection);
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
        if (mainMenuScene) {
            mainMenuScene->resize(width, height, projection);
        }
        if (radarMeshNode) {
            radarMeshNode->setPosition({width - 240 + 100, 30.0f + 110, 0.0f});
        }
    }

    void MainScene::initSounds() const {
        const bool musicLoaded = soundManager->addSoundFromFile("music", "Assets/Sounds/snake.wav", true);
        const bool coinLoaded = soundManager->addSoundFromFile("coin", "Assets/Sounds/coin.wav");
        if (!musicLoaded || !coinLoaded) {
            std::cerr << "Sound loading error in MainScene." << std::endl;
            return;
        }
        //soundManager->play("music", {{AL_LOOPING, AL_TRUE}});
    }

    void MainScene::initLights() {
        auto [spotLights, pointLights] = SceneLightFactory::create();
        this->spotLights.insert(this->spotLights.end(), spotLights.begin(), spotLights.end());
        this->pointLights.insert(this->pointLights.end(), pointLights.begin(), pointLights.end());

        if (manipulatorHandler) {
            for (const auto &point : pointLights) {
                manipulatorHandler->getLightsHandler()->addItem(point);
            }
        }
    }

    void MainScene::initSkybox() {
        const auto skyboxNode = make_shared<SkyboxNode3D>(contextState, resourceManager, camera);
        addMeshNode3D(skyboxNode, 1000);
    }

    void MainScene::initPlane() {
        auto basicShader = resourceManager->getShader("basicShader");
        auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
        const auto shadowMap = resourceManager->getTexture("depth");
        const auto gamefieldAlbedo = resourceManager->getTexture("tile.png");
        const auto gamefieldNormal = resourceManager->getTexture("gamefield_normal.jpg");
        const auto gamefieldSpecular = resourceManager->getTexture("gamefield_specular.jpg");
        planeMaterial = make_shared<PlanarReflectionMaterial>(basicShader, shadowDepthShader);
        planeMaterial->setReflectionTexture(resourceManager->getTexture("PlanarReflectionTexture"));
        planeMaterial->setReflectionEnabled(rendererManager->isReflectionsEnabled());

        planeMaterial->setDirectionalLight(directionalLight);
        planeMaterial->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        planeMaterial->setShadow(shadowMap);
        planeMaterial->setNormalEnabled(true);
        planeMaterial->setAlbedo(gamefieldAlbedo);
        planeMaterial->setNormal(gamefieldNormal);
        planeMaterial->setSpecular(gamefieldSpecular);
        planeMaterial->set_uv_scale(glm::vec2(48.0f, 48.0f));
        planeMaterial->setSpotLights(spotLights);
        planeMaterial->setPointLights(pointLights);

        auto planeMesh = make_shared<PlaneMesh>(basicShader, 4, 4);
        planeMesh->setMaterial(planeMaterial);
        const auto node3d = make_shared<MeshNode3D>(contextState, planeMesh, resourceManager);
        node3d->disablePlanarReflection();
        node3d->setRotationX(90);
        node3d->setPosition({1.0, 1.0, -1.0});

        addMeshNode3D(node3d, 200);
    }

    void MainScene::initPlayerScene() {
        playerScene = make_shared<PlayerScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        playerScene->setCollisionSystem(collisionSystem);
        playerScene->setManipulatorHandler(manipulatorHandler);
        playerScene->init(2);
        snakeMoveHandler = playerScene->getSnakeMoveHandler();
        addNode("player", playerScene);
        initRemoteSnakeScene();
    }

    void MainScene::initRemoteSnakeScene() {
        remoteSnakeScene = make_shared<RemoteSnakeScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        remoteSnakeScene->setCollisionSystem(collisionSystem);
        remoteSnakeScene->setManipulatorHandler(manipulatorHandler);
        remoteSnakeScene->init(5);
        remoteSnakeScene->setActive(false);
        addNode("remotePlayer", remoteSnakeScene);
    }

    void MainScene::initBarriersScene() {
        barriersScene = make_shared<BarriersScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        barriersScene->setCollisionSystem(collisionSystem);
        barriersScene->setManipulatorHandler(manipulatorHandler);
        barriersScene->init(3);
        levelManager = barriersScene->getLevelManager();
        addNode("barriers", barriersScene);
    }

    void MainScene::initCoinScene() {
        coinScene = make_shared<CoinScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        coinScene->setCollisionSystem(collisionSystem);
        coinScene->setManipulatorHandler(manipulatorHandler);
        coinScene->init(4);
        addNode("coin", coinScene);
    }

    void MainScene::initTorchScene() {
        const auto torchScene = make_shared<TorchScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        torchScene->setCollisionSystem(collisionSystem);
        torchScene->setManipulatorHandler(manipulatorHandler);
        torchScene->init(1);
        addNode("torch", torchScene);
    }

    void MainScene::initWeatherScene() {
        const auto weatherScene = make_shared<WeatherScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        weatherScene->init(0);
        addNode("weather", weatherScene);
    }

    void MainScene::initEatManager() {
        eatLocationHandler = make_shared<EatLocationHandler>(
            barriersScene->getLevelBoxes(),
            playerScene->getSnake(),
            coinScene->getCoin()
        );
        eatManager = make_unique<EatManager>(eatLocationHandler);

        if (remoteSnakeScene && remoteSnakeScene->getSnake()) {
            remoteEatLocationHandler = make_shared<EatLocationHandler>(
                barriersScene->getLevelBoxes(),
                remoteSnakeScene->getSnake(),
                coinScene->getCoin()
            );
            remoteEatManager = make_unique<EatManager>(remoteEatLocationHandler);
        }
    }

    void MainScene::initRadar() {
        radarFadeInUniform = make_shared<FadeInUniform>();
        radarFadeInUniform->setStep(5.0f);
        radarExpansionIn = make_shared<ShaderMaterial>(resourceManager->getShader("quadCorner"));
        radarExpansionIn->setUniform("quadSize", glm::vec2(200, 200));
        radarExpansionIn->setUniform("borderColor", glm::vec3(1.0,0.0,0.0));
        radarExpansionIn->setUniform("borderWidth", 11.9f);
        radarExpansionIn->setUniform("radius", 8.0f);
        radarExpansionIn->setUniform("expansion", radarFadeInUniform);
        radarFadeInUniform->setFinishedCallback([this]() {
            radarMeshNode->showItems();
            if (coinScene->getCoin()->isVisible() == false) {
                radarMeshNode->hideItem("coin");
            }
        });
        radarFadeInUniform->start();

        radarFadeOutUniform = make_shared<FadeOutUniform>();
        radarFadeOutUniform->setStep(5.0f);
        radarExpansionOut = make_shared<ShaderMaterial>(resourceManager->getShader("quadCorner"));
        radarExpansionOut->setUniform("quadSize", glm::vec2(200, 200));
        radarExpansionOut->setUniform("borderColor", glm::vec3(1.0,0.0,0.0));
        radarExpansionOut->setUniform("borderWidth", 11.9f);
        radarExpansionOut->setUniform("radius", 8.0f);
        radarExpansionOut->setUniform("expansion", radarFadeOutUniform);

        radarNode = make_shared<QuadNode2D>(220, 220, nullptr);
        radarNode->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        radarNode->setMaterial(radarExpansionIn);
        radarMeshNode = make_shared<RadarMeshNode2D>(contextState, radarNode, resourceManager);
        radarNode->setBlending(Blending::Translucent);
        radarMeshNode->setPosition({width - 240 + 100, 30.0 + 110, 0.0}); // + 100 kvuli tomu, ze stred neni 0,0 ale stred quadu
        rebuildRadarItems(false);
        radarMeshNode->hideItems();

        addMeshNode2D(radarMeshNode);

        if (menuVisible) {
            radarMeshNode->setVisible(false);
            hudRadarVisible = true;
        }
    }

    void MainScene::initLabels() {
        const auto shader = resourceManager->getShader("textShader");
        const auto font = make_shared<Font>("Assets/Fonts/OCRAEXT.TTF", 26);
        const auto settings = make_shared<LabelSettings>(font);
        const auto label = make_shared<LabelNode2D>("Press start I, K or L...", shader, settings);
        label->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

        fadeOutUniform = make_shared<FadeOutUniform>();
        const auto shaderMaterial = make_shared<ShaderMaterial>(shader);
        shaderMaterial->setUniform("alpha", fadeOutUniform);
        shaderMaterial->setUniform("textColor", glm::vec3(1.0f));
        shaderMaterial->setUniform("textTexture", 0);
        fadeOutUniform->setFinishedCallback([this]() {
            helpText->setVisible(false);
        });

        label->setMaterial(shaderMaterial);
        helpText = make_shared<MeshNode2D>(contextState, label, resourceManager);

        tilesCounterText = make_shared<LabelNode2D>("", shader, settings);
        fadeInUniform = make_shared<FadeInUniform>();
        const auto shaderMaterial2 = make_shared<ShaderMaterial>(shader);
        shaderMaterial2->setUniform("alpha", fadeInUniform);
        shaderMaterial2->setUniform("textColor", glm::vec3(1.0f));
        shaderMaterial2->setUniform("textTexture", 0);

        tilesCounterText->setMaterial(shaderMaterial2);
        tilesCounterNode = make_shared<MeshNode2D>(contextState, tilesCounterText, resourceManager);
        tilesCounterNode->setVisible(false);

        addMeshNode2D(helpText);
        addMeshNode2D(tilesCounterNode);

        if (menuVisible) {
            helpText->setVisible(false);
            tilesCounterNode->setVisible(false);
        }
    }

    void MainScene::initPreloader() {
        preLoader = make_shared<Preloader2Scene>(nullptr,
            vector<shared_ptr<SpotLight> >{},
            vector<shared_ptr<PointLight> >{},
            rendererManager, camera, projection, resourceManager, width, height);
        preLoader->init(-10);
        addNode("scenePreloader", preLoader);
    }

    void MainScene::initMainMenu() {
        mainMenuScene = make_shared<MainMenuScene>(directionalLight, spotLights, pointLights, rendererManager, camera,
            projection, resourceManager, width, height);
        mainMenuScene->init(200);
        showMenu(MainMenuScene::PrimaryAction::Start);
    }

    void MainScene::initNetworking() {
        netSession.loadConfig();

        if (mainMenuScene) {
            mainMenuScene->setLocalIp(Net::getLocalIpAddress());
            mainMenuScene->setNetworkStatus("Status: idle");
        }
    }

    void MainScene::resetNetworkState() {
        netSession.reset();
        if (playerScene) {
            playerScene->setInputEnabled(true);
        }
        if (remoteSnakeScene) {
            remoteSnakeScene->setServerControlled(false);
            remoteSnakeScene->setActive(false);
        }
        if (mainMenuScene) {
            mainMenuScene->setNetworkStatus("Status: idle");
            mainMenuScene->setNetworkSessionState(MainMenuScene::NetworkSessionState::Idle);
            mainMenuScene->setMenuView(MainMenuScene::MenuView::Main);
        }
    }

    void MainScene::rebuildRadarItems(const bool includeRemote) const {
        if (!radarMeshNode || !playerScene || !coinScene || !barriersScene) {
            return;
        }

        radarMeshNode->clearItems();
        radarMeshNode->addItem(playerScene->getSnake(), glm::vec3(0.0,1.0,0.0), "snake");
        if (includeRemote && remoteSnakeScene && remoteSnakeScene->getSnake()) {
            radarMeshNode->addItem(remoteSnakeScene->getSnake(), glm::vec3(0.1f,0.45f,1.0f), "remote-snake");
        }
        radarMeshNode->addItem(coinScene->getCoin(), glm::vec3(1.0,1.0,0.0), "coin");
        radarMeshNode->addItem(barriersScene->getLevelBoxes(), glm::vec3(1.0,0.0,0.0), "barriers");
    }

    void MainScene::initializeMultiplayerState() {
        gameStarted = true;
            if (playerScene) {
                playerScene->setInputEnabled(!netSession.isClient());
                if (const auto snake = playerScene->getSnake()) {
                snake->setDirection(SnakeMeshNode3D::NONE);
                snake->setRotationX(90.0f);
                snake->setRotationY(netSession.isClient() ? 180.0f : 0.0f);
                if (netSession.isClient()) {
                    const glm::vec3 clientHead = findRemoteSpawnPosition();
                    snake->respawn();
                    if (const auto moveHandler = playerScene->getSnakeMoveHandler()) {
                        moveHandler->resetState();
                        moveHandler->setInitialBodyDirection(SnakeMeshNode3D::LEFT);
                    }
                    const auto spawnPositions = Net::buildStraightSnakePositions({clientHead.x, clientHead.y}, 6,
                                                                                SnakeMeshNode3D::LEFT);
                    Net::applyExactSnakePositions(snake, spawnPositions, SnakeMeshNode3D::LEFT, false);
                } else {
                    netSession.clearPendingLocalRespawn();
                    snake->respawn();
                }
                }
            }
        if (remoteSnakeScene) {
            remoteSnakeScene->setServerControlled(netSession.isServer());
            remoteSnakeScene->setActive(true);
            if (netSession.isServer()) {
                remoteSnakeScene->setSpawnLayout(findRemoteSpawnPosition(), SnakeMeshNode3D::LEFT);
            } else {
                remoteSnakeScene->setSpawnLayout({23.0f, -3.0f, -23.0f}, SnakeMeshNode3D::RIGHT);
            }
        }
        rebuildRadarItems(true);
        if (mainMenuScene) {
            mainMenuScene->setNetworkSessionState(netSession.isServer() ? MainMenuScene::NetworkSessionState::Hosting
                                                             : MainMenuScene::NetworkSessionState::Client);
            mainMenuScene->setMenuView(MainMenuScene::MenuView::Main);
        }
        if (menuVisible) {
            hideMenu();
        }
    }

    void MainScene::shutdownMultiplayerState(const bool showMenuAfter) {
        resetNetworkState();
        gameStarted = false;

        if (playerScene && playerScene->getSnake()) {
            playerScene->getSnake()->respawn();
        }
        if (remoteSnakeScene) {
            remoteSnakeScene->setActive(false);
        }
        rebuildRadarItems(false);

        if (showMenuAfter) {
            showMenu(MainMenuScene::PrimaryAction::Start);
        }
    }

    void MainScene::respawnLocalSnake() {
        if (!levelManager || !playerScene || !coinScene) {
            return;
        }

        if (netSession.isEnabled() && netSession.isServer()) {
            netSession.bumpLocalRespawnSerial();
        }

        if (eatManager) {
            eatManager->run(EatManager::clean);
        }
        playerScene->getSnake()->crash();
        levelManager->setLive(levelManager->getLive() - 1);
        levelManager->setEatCounter(0);

        char buff[100];
        snprintf(buff, sizeof(buff),
                 "%s %d, %s %d, %s %d",
                 "Level:",
                 levelManager->getLevel(),
                 "Lives:",
                 levelManager->getLive(),
                 "Points left:",
                 MAX_POINT - levelManager->getEatCounter());
        tilesCounterText->setText(buff);
        if (!netSession.isEnabled()) {
            coinScene->getCoin()->setVisible(false);
            coinScene->getCoin()->animationStop("coinRotation");
            radarMeshNode->hideItem("coin");
        }
        playerScene->getSnake()->animationStop("KostraAction");
    }

    void MainScene::respawnRemoteSnake() {
        if (remoteSnakeScene) {
            remoteSnakeScene->respawnAt(findRemoteSpawnPosition(), SnakeMeshNode3D::LEFT);
        }
        if (netSession.isEnabled() && netSession.isServer()) {
            netSession.bumpRemoteRespawnSerial();
        }
        if (remoteEatManager) {
            remoteEatManager->run(EatManager::clean);
        }
    }

    bool MainScene::localSnakeHitRemote() const {
        if (!playerScene || !playerScene->getSnake()) {
            return false;
        }

        for (const auto &shapeNode : playerScene->getSnake()->getCollisionShapes()) {
            for (const auto &body : shapeNode->getCollidingBodies()) {
                for (const auto &bodyShape : body->getCollisionShapes()) {
                    const uint32_t layer = bodyShape->getCollisionLayer();
                    if (layer == ENEMY || layer == ENEMY_BODY) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool MainScene::remoteSnakeHitLocal() const {
        if (!remoteSnakeScene || !remoteSnakeScene->getSnake()) {
            return false;
        }

        for (const auto &shapeNode : remoteSnakeScene->getSnake()->getCollisionShapes()) {
            for (const auto &body : shapeNode->getCollidingBodies()) {
                for (const auto &bodyShape : body->getCollisionShapes()) {
                    const uint32_t layer = bodyShape->getCollisionLayer();
                    if (layer == PLAYER || layer == PLAYER_BODY) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void MainScene::startNetworkGame() {
        initializeMultiplayerState();
    }

    bool MainScene::collectWorldSnapshot(Net::WorldSnapshotState &out) const {
        if (!playerScene || !coinScene || !levelManager) {
            return false;
        }
        const auto snake = playerScene->getSnake();
        const auto coin = coinScene->getCoin();
        const auto remoteSnake = remoteSnakeScene ? remoteSnakeScene->getSnake() : nullptr;
        if (!snake || !coin || !remoteSnake) {
            return false;
        }

        out.localSnake.positions = collectSnakePositions(snake);
        out.localSnake.direction = snake->getDirection();
        out.localSnake.segmentCount = static_cast<uint32_t>(snake->getChildren().size() + 1);
        out.localSnake.crashActive = snake->isCrashing();
        out.localSnake.stopped = snakeMoveHandler && snakeMoveHandler->isStopped();

        out.remoteSnake.positions = remoteSnakeScene->collectPositions();
        out.remoteSnake.direction = remoteSnake->getDirection();
        out.remoteSnake.segmentCount = static_cast<uint32_t>(remoteSnake->getChildren().size() + 1);
        out.remoteSnake.crashActive = remoteSnake->isCrashing();
        out.remoteSnake.stopped = remoteSnakeScene->getMoveHandler() &&
                                  remoteSnakeScene->getMoveHandler()->isStopped();

        const glm::vec3 coinPos = coin->getPosition();
        out.coinX = coinPos.x;
        out.coinY = coinPos.y;
        out.coinVisible = coin->isVisible();
        out.level = static_cast<uint32_t>(levelManager->getLevel());
        out.eatCounter = static_cast<uint32_t>(levelManager->getEatCounter());
        out.lives = static_cast<uint32_t>(levelManager->getLive());
        out.winning = winning;
        return true;
    }

    void MainScene::onClientHello(uint32_t) {
        if (mainMenuScene) {
            mainMenuScene->setNetworkStatus("Status: client connected");
        }
        startNetworkGame();
    }

    void MainScene::onWelcomeReceived() {
        if (mainMenuScene) {
            mainMenuScene->setNetworkStatus("Status: connected");
        }
        startNetworkGame();
    }

    void MainScene::onPeerDisconnected() {
        shutdownMultiplayerState(true);
    }

    void MainScene::onRemoteInput(const int8_t moveX, const int8_t moveY, const uint8_t actions) {
        if (remoteSnakeScene) {
            remoteSnakeScene->applyNetworkInput(moveX, moveY, actions);
        }
    }

    void MainScene::requestLocalCrash() {
        if (!playerScene) {
            return;
        }
        if (const auto snake = playerScene->getSnake()) {
            snake->crash();
        }
        playerScene->setInputEnabled(false);
    }

    void MainScene::requestRemoteCrash() {
        if (!remoteSnakeScene) {
            return;
        }
        if (const auto remoteSnake = remoteSnakeScene->getSnake()) {
            remoteSnake->crash();
        }
    }

    void MainScene::applyLocalSnakePositions(const Net::SnakeSnapshotState &snake) {
        if (!playerScene) {
            return;
        }
        if (const auto local = playerScene->getSnake()) {
            Net::applyExactSnakePositions(local, snake.positions, snake.direction, snake.stopped);
        }
    }

    void MainScene::applyRemoteSnakePositions(const Net::SnakeSnapshotState &snake) {
        if (!remoteSnakeScene) {
            return;
        }
        if (const auto remote = remoteSnakeScene->getSnake()) {
            Net::applyExactSnakePositions(remote, snake.positions, snake.direction, snake.stopped);
        }
        remoteSnakeScene->setActive(true);
    }

    void MainScene::applyCoin(const float x, const float y, const bool visible, const bool eatenAnim) {
        if (!coinScene) {
            return;
        }
        const auto coin = coinScene->getCoin();
        if (!coin) {
            return;
        }
        if (eatenAnim) {
            coinScene->getRemoveCoin()->setTransform(coin);
            coinScene->getRemoveCoin()->setVisible(true);
            coinScene->getRemoveCoin()->animationStart("eatenUp", false);
        }
        coin->setPosition({x, y, coin->getPosition().z});
        coin->setVisible(visible);
        if (visible) {
            coin->animationStart("coinRotation", true);
        } else {
            coin->animationStop("coinRotation");
            if (radarMeshNode) {
                radarMeshNode->hideItem("coin");
            }
        }
        if (eatLocationHandler) {
            eatLocationHandler->fixVirtualPosition(coin->getPosition());
        }
        if (remoteEatLocationHandler) {
            remoteEatLocationHandler->fixVirtualPosition(coin->getPosition());
        }
        if (visible && radarMeshNode) {
            radarMeshNode->showItem("coin");
        }
    }

    void MainScene::applyHud(const uint32_t level, const uint32_t eatCounter, const uint32_t lives) {
        if (!levelManager) {
            return;
        }
        levelManager->setLevel(static_cast<int>(level));
        levelManager->setEatCounter(static_cast<int>(eatCounter));
        levelManager->setLive(static_cast<int>(lives));
        char buff[100];
        snprintf(buff, sizeof(buff),
                 "%s %d, %s %d, %s %d",
                 "Level:", levelManager->getLevel(),
                 "Lives:", levelManager->getLive(),
                 "Points left:", MAX_POINT - levelManager->getEatCounter());
        tilesCounterText->setText(buff);
        if (!winning && tilesCounterNode) {
            tilesCounterNode->setVisible(true);
        }
    }

    void MainScene::applyWinning() {
        enterWinningState();
    }

    std::vector<glm::vec2> MainScene::collectSnakePositions(const shared_ptr<SnakeMeshNode3D> &snake) {
        std::vector<glm::vec2> positions;
        if (!snake) {
            return positions;
        }

        positions.emplace_back(snake->getPosition().x, snake->getPosition().y);
        for (const auto &child : snake->getChildren()) {
            positions.emplace_back(child->getPosition().x, child->getPosition().y);
        }
        return positions;
    }

    glm::vec3 MainScene::findRemoteSpawnPosition() const {
        const auto isBlocked = [this](const glm::vec2 &pos) {
            auto matchesNode = [&pos](const shared_ptr<MeshNode3D> &node) {
                if (!node) {
                    return false;
                }
                constexpr float epsilon = 0.01f;
                const auto nodePos = node->getPosition();
                return std::abs(nodePos.x - pos.x) < epsilon && std::abs(nodePos.y - pos.y) < epsilon;
            };

            if (playerScene && playerScene->getSnake()) {
                const auto snakePositions = collectSnakePositions(playerScene->getSnake());
                for (const auto &snakePos : snakePositions) {
                    if (std::abs(snakePos.x - pos.x) < 0.01f && std::abs(snakePos.y - pos.y) < 0.01f) {
                        return true;
                    }
                }
            }

            if (!barriersScene || !barriersScene->getLevelBoxes()) {
                return false;
            }

            if (matchesNode(barriersScene->getLevelBoxes())) {
                return true;
            }

            return std::ranges::any_of(
                barriersScene->getLevelBoxes()->getChildren(),
                matchesNode);
        };

        for (int yIndex = 0; yIndex <= 46; ++yIndex) {
            constexpr float step = 2.0f;
            const float y = 71.0f - static_cast<float>(yIndex) * step;
            for (int xIndex = 0; xIndex <= 36; ++xIndex) {
                constexpr float startX = 61.0f;
                const float x = startX - static_cast<float>(xIndex) * step;
                bool valid = true;
                for (int segment = 0; segment < 6; ++segment) {
                    if (isBlocked({x + static_cast<float>(segment) * step, y})) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    return {x, y, -23.0f};
                }
            }
        }

        return {49.0f, 55.0f, -23.0f};
    }

    void MainScene::handleCoinEaten(const EatManager &manager) {
        if (!levelManager || !coinScene || winning) {
            return;
        }

        soundManager->play("coin");

        coinScene->getRemoveCoin()->setTransform(coinScene->getCoin());
        coinScene->getRemoveCoin()->setVisible(true);
        coinScene->getRemoveCoin()->animationStart("eatenUp", false);

        levelManager->setEatCounter(levelManager->getEatCounter() + 1);

        if (levelManager->getEatCounter() == MAX_POINT) {
            fadeOutUniform->setAlpha(1.0f);
            coinScene->getCoin()->setVisible(false);
            if (radarMeshNode) {
                radarMeshNode->hideItem("coin");
            }
            if (levelManager->getLevel() < 9) {
                nextLevel();
            } else {
                cout << "End game... you win !" << endl;
                enterWinningState();
            }
        } else {
            manager.run(EatManager::eatenUp);
            if (coinScene->getCoin()->isVisible()) {
                coinScene->getCoin()->animationStart("coinRotation", true);
                if (radarMeshNode) {
                    radarMeshNode->showItem("coin");
                }
            }
        }

        char buff[100];
        snprintf(buff, sizeof(buff),
                 "%s %d, %s %d, %s %d",
                 "Level:",
                 levelManager->getLevel(),
                 "Lives:",
                 levelManager->getLive(),
                 "Points left:",
                 MAX_POINT - levelManager->getEatCounter()
        );
        tilesCounterText->setText(buff);
    }

    void MainScene::enterWinningState() {
        if (winning) {
            return;
        }

        if (tilesCounterNode) {
            tilesCounterNode->setVisible(false);
        }
        if (playerScene) {
            playerScene->setInputEnabled(false);
            playerScene->winning();
        }
        if (remoteSnakeScene) {
            remoteSnakeScene->setServerControlled(false);
            remoteSnakeScene->setActive(false);
        }
        if (winnerScene && !hasNode("winner")) {
            addNode("winner", winnerScene);
        }
        if (radarMeshNode) {
            radarMeshNode->setVisible(false);
        }
        winning = true;
    }

    void MainScene::buildEatenUpCallback() {
        snakeMoveHandler->setEatenUpCallback([this]() {
            if (this->eatManager) {
                handleCoinEaten(*this->eatManager);
            }
        });

        if (remoteSnakeScene && remoteSnakeScene->getMoveHandler()) {
            remoteSnakeScene->getMoveHandler()->setEatenUpCallback([this]() {
                if (this->remoteEatManager) {
                    handleCoinEaten(*this->remoteEatManager);
                }
            });
        }
    }

    void MainScene::buildStartMoveCallback() const {
        snakeMoveHandler->addStartMoveCallback([this]() {
            if (this->levelManager) {
                if ((!netSession.isEnabled() || netSession.isServer()) && this->eatManager && coinScene && !coinScene->getCoin()->isVisible()) {
                    this->eatManager->run(EatManager::firstPlace);
                }
                if (fadeOutUniform->getAlpha() != 0.0f) {
                    fadeOutUniform->start();
                }
                char buff[100];
                snprintf(buff, sizeof(buff),
                         "%s %d, %s %d, %s %d",
                         "Level:",
                         this->levelManager->getLevel(),
                         "Lives:",
                         this->levelManager->getLive(),
                         "Points left:",
                         MAX_POINT - this->levelManager->getEatCounter()
                );
                const std::string buffAsStdStr = buff;
                tilesCounterNode->setVisible(true);
                tilesCounterText->setText(buffAsStdStr);
                if (fadeInUniform->getAlpha() != 1.0f) {
                    fadeInUniform->start();
                }
                radarMeshNode->showItem("coin");
                coinScene->getCoin()->animationStart("coinRotation");
                playerScene->getSnake()->animationStart("KostraAction", true);
            }
        });

        if (remoteSnakeScene && remoteSnakeScene->getMoveHandler()) {
            remoteSnakeScene->getMoveHandler()->addStartMoveCallback([this]() {
                if ((!netSession.isEnabled() || netSession.isServer()) && this->eatManager && coinScene && !coinScene->getCoin()->isVisible()) {
                    this->eatManager->run(EatManager::firstPlace);
                }
            });
        }
    }

    void MainScene::buildCrashCallback() {
        snakeMoveHandler->setCrashCallback([this]() {
            if (multiplayerCrashInProgress) {
                return;
            }

            multiplayerCrashInProgress = true;
            if (localSnakeHitRemote()) {
                respawnLocalSnake();
                respawnRemoteSnake();
            } else {
                respawnLocalSnake();
            }

            if (this->levelManager && this->levelManager->getLive() == 0) {
                cout << "crash callback call" << endl;
            }
        });

        if (remoteSnakeScene && remoteSnakeScene->getMoveHandler()) {
            remoteSnakeScene->getMoveHandler()->setCrashCallback([this]() {
                if (multiplayerCrashInProgress) {
                    return;
                }

                multiplayerCrashInProgress = true;
                if (remoteSnakeHitLocal()) {
                    respawnLocalSnake();
                    respawnRemoteSnake();
                } else {
                    respawnRemoteSnake();
                }
            });
        }
    }

    void MainScene::prepareScene() {
        switch (progress) {
            case 0:
                initSkybox();
                initLights();
                break;
            case 10:
                initPlane();
                initLabels();
                break;
            case 20:
                initPlayerScene();
                break;
            case 30:
                initBarriersScene();
                break;
            case 40:
                initCoinScene();
                break;
            case 50:
                initTorchScene();
                break;
            case 60:
                initRadar();
                break;
            case 70:
                initEatManager();
                initWeatherScene();
                break;
            case 80:
                buildStartMoveCallback();
                buildEatenUpCallback();
                buildCrashCallback();
                winnerScene = make_shared<WinnerScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
                winnerScene->init(0);
                break;
            case 90:
                if (manipulatorHandler != nullptr) {
                    manipulatorHandler->getLightsHandler()->addItem(directionalLight);

                    const auto focusTextNode = manipulatorHandler->getLightsHandler()->getFocusLabel();
                    const auto colorTextNode = manipulatorHandler->getLightsHandler()->getColorLabel();
                    const auto positionTextNode = manipulatorHandler->getLightsHandler()->getPositionLabel();
                    const auto directionTextNode = manipulatorHandler->getLightsHandler()->getDirectionLabel();
                    focusTextNode->setPosition(glm::vec3(10, height - 120, 0.0f));
                    colorTextNode->setPosition(glm::vec3(10, height - 100, 0.0f));
                    directionTextNode->setPosition(glm::vec3(10, height - 80, 0.0f));
                    positionTextNode->setPosition(glm::vec3(10, height - 60, 0.0f));
                    const auto dirLightNode = manipulatorHandler->getLightsHandler()->getDirLightNode();
                    dirLightNode->setDirectionalLight(directionalLight);
                    addMeshNode3D(dirLightNode);
                    addMeshNode2D(focusTextNode);
                    addMeshNode2D(colorTextNode);
                    addMeshNode2D(positionTextNode);
                    addMeshNode2D(directionTextNode);
                }
                break;
            case 100:
                initSounds();
                removeNode("scenePreloader");
                loading = false;
            default:
                break;
        }

        progress++;
    }

    void MainScene::nextLevel() {
        levelManager->setLevel(levelManager->getLevel() + 1);
        eatLocationHandler->clearBarriers();
        rebuildRadarItems(netSession.isEnabled());
        removeNode("barriers");
        barriersScene->nextLevel();
        addNode("barriers", barriersScene);
        eatLocationHandler->setBarriers(barriersScene->getLevelBoxes());
        if (remoteEatLocationHandler) {
            remoteEatLocationHandler->setBarriers(barriersScene->getLevelBoxes());
        }
        playerScene->getSnake()->respawn();
        eatManager->run(EatManager::clean);
        if (remoteSnakeScene) {
            remoteSnakeScene->setSpawnLayout(findRemoteSpawnPosition(), SnakeMeshNode3D::LEFT);
        }
        if (remoteEatManager) {
            remoteEatManager->run(EatManager::clean);
        }
    }

    void MainScene::update() {
        multiplayerCrashInProgress = false;
        netSession.tick(*this, *this);
        if (netSession.isEnabled() && netSession.isServer() && remoteSnakeScene) {
            remoteSnakeScene->updateAuthoritative();
        }
        Scene::update();
        if (netSession.hasPendingLocalRespawn() && playerScene && playerScene->getSnake() && playerScene->getSnake()->isReady()) {
            const auto &pending = netSession.getPendingLocalRespawn();
            if constexpr (isDebug) {
                std::cout << "[Net][ClientRespawnApply] dir=" << static_cast<int>(pending.direction)
                          << " count=" << pending.segmentCount
                          << " positions=" << pending.positions.size() << std::endl;
            }
            const auto snake = playerScene->getSnake();
            snake->respawn();
            if (const auto moveHandler = playerScene->getSnakeMoveHandler()) {
                moveHandler->resetState();
                moveHandler->setInitialBodyDirection(
                    pending.direction == SnakeMeshNode3D::NONE ? SnakeMeshNode3D::LEFT : pending.direction);
            }
            Net::applyExactSnakePositions(snake, pending.positions, pending.direction, false);
            netSession.clearPendingLocalRespawn();
            playerScene->setInputEnabled(!netSession.isClient());
        }
        if (loading) {
            prepareScene();
        }

        if (radarMeshNode != nullptr) {
            if (radarMeshNode->isVisible() && radarFadeOutUniform->getAlpha() <= 0) {
                radarMeshNode->setVisible(false);
            }
        }

        if (helpText != nullptr) {
            if (!helpText->isVisible() && eatManager && (!netSession.isEnabled() || netSession.isServer())) {
                eatManager->run(EatManager::checkPlace);
            }
        }
    }

    void MainScene::setCursorPosition(const glm::vec2 &position) const {
        if (mainMenuScene) {
            mainMenuScene->setCursorPosition(position);
        }
    }

    void MainScene::mouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods) {
        if (!menuVisible || !mainMenuScene) {
            return;
        }

        switch (mainMenuScene->handleMouseButton(button, action)) {
            case MainMenuScene::MenuAction::Host: {
                if (netSession.isEnabled()) {
                    this->resetNetworkState();
                }
                if (netSession.host()) {
                    mainMenuScene->setNetworkStatus("Status: hosting");
                    mainMenuScene->setNetworkSessionState(MainMenuScene::NetworkSessionState::Hosting);
                    if constexpr (isDebug) {
                        std::cout << "[Net] Hosting on port " << netSession.getPort() << std::endl;
                    }
                } else {
                    mainMenuScene->setNetworkStatus("Status: host failed");
                    if constexpr (isDebug) {
                        std::cout << "[Net] Failed to host on port " << netSession.getPort() << std::endl;
                    }
                }
                break;
            }
            case MainMenuScene::MenuAction::Disconnect:
                shutdownMultiplayerState(true);
                break;
            case MainMenuScene::MenuAction::Join: {
                if (netSession.isEnabled()) {
                    this->resetNetworkState();
                }
                const std::string ip = mainMenuScene->getJoinIp();
                if (netSession.join(ip, "Player")) {
                    mainMenuScene->setNetworkStatus("Status: connecting");
                    mainMenuScene->setNetworkSessionState(MainMenuScene::NetworkSessionState::Client);
                    if constexpr (isDebug) {
                        std::cout << "[Net] Joining " << ip << ":" << netSession.getPort() << std::endl;
                    }
                } else {
                    mainMenuScene->setNetworkStatus("Status: join failed");
                    if constexpr (isDebug) {
                        std::cout << "[Net] Failed to join " << ip << ":" << netSession.getPort() << std::endl;
                    }
                }
                break;
            }
            case MainMenuScene::MenuAction::Start:
                if (netSession.isEnabled()) {
                    startNetworkGame();
                } else {
                    gameStarted = true;
                    hideMenu();
                    if (eatManager && coinScene && !coinScene->getCoin()->isVisible()) {
                        eatManager->run(EatManager::firstPlace);
                        coinScene->getCoin()->animationStart("coinRotation", true);
                    }
                }
                break;
            case MainMenuScene::MenuAction::Resume:
                hideMenu();
                break;
            case MainMenuScene::MenuAction::NewGame:
                if (netSession.isEnabled()) {
                    shutdownMultiplayerState(false);
                }
                gameStarted = false;
                if (eatManager) {
                    eatManager->run(EatManager::clean);
                }
                if (coinScene) {
                    coinScene->getCoin()->setVisible(false);
                    coinScene->getCoin()->animationStop("coinRotation");
                }
                if (levelManager) {
                    levelManager->setLive(MAX_LIVES);
                    eatLocationHandler->clearBarriers();
                    removeNode("barriers");
                    barriersScene->nextLevel(START_LEVEL);
                    addNode("barriers", barriersScene);
                    eatLocationHandler->setBarriers(barriersScene->getLevelBoxes());
                }
                if (playerScene && playerScene->getSnake()) {
                    playerScene->getSnake()->respawn();
                }
                showMenu(MainMenuScene::PrimaryAction::Start);
                break;
            case MainMenuScene::MenuAction::Quit:
                if (netSession.isEnabled()) {
                    netSession.notifyDisconnect();
                }
                glfwSetWindowShouldClose(window, true);
                break;
            default:
                break;
        }
    }

    bool MainScene::isMenuVisible() const {
        return menuVisible;
    }

    void MainScene::showMenu(const MainMenuScene::PrimaryAction action) {
        if (!mainMenuScene) {
            return;
        }
        if (menuVisible) {
            mainMenuScene->setPrimaryAction(action);
            return;
        }

        menuVisible = true;
        mainMenuScene->resize(width, height, projection);
        mainMenuScene->setPrimaryAction(action);
        if (!hasNode("mainMenu")) {
            addNode("mainMenu", mainMenuScene);
        }
        saveHudVisibility();
    }

    void MainScene::hideMenu() {
        if (!menuVisible) {
            return;
        }
        menuVisible = false;
        removeNode("mainMenu");
        restoreHudVisibility();
        if (playerScene) {
            camera->setStickyPoint(playerScene->getSnake());
        }
        if (netSession.isEnabled() && netSession.isClient()) {
            if (resumeLocalMovementAfterMenu) {
                netSession.sendPauseToggle();
            }
        } else if (snakeMoveHandler && resumeLocalMovementAfterMenu) {
            snakeMoveHandler->setStopped(false);
        }
        resumeLocalMovementAfterMenu = false;
    }

    void MainScene::saveHudVisibility() {
        if (hudStateSaved) {
            return;
        }

        hudHelpVisible = helpText ? helpText->isVisible() : false;
        hudTilesVisible = tilesCounterNode ? tilesCounterNode->isVisible() : false;
        hudRadarVisible = radarMeshNode ? radarMeshNode->isVisible() : false;

        if (helpText) {
            helpText->setVisible(false);
        }
        if (tilesCounterNode) {
            tilesCounterNode->setVisible(false);
        }
        if (radarMeshNode) {
            radarMeshNode->setVisible(false);
        }

        hudStateSaved = true;
    }

    void MainScene::restoreHudVisibility() {
        if (!hudStateSaved) {
            return;
        }

        if (helpText) {
            helpText->setVisible(hudHelpVisible);
        }
        if (tilesCounterNode) {
            tilesCounterNode->setVisible(hudTilesVisible);
        }
        if (radarMeshNode) {
            radarMeshNode->setVisible(hudRadarVisible);
        }

        hudStateSaved = false;
    }
} // Scenes
