#include "MainScene.h"

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
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height),
          netClient(netManager),
          netServer(netManager) {
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
                if (netEnabled && netIsClient) {
                    const auto snake = playerScene ? playerScene->getSnake() : nullptr;
                    this->resumeLocalMovementAfterMenu =
                        snake && snake->getDirection() > SnakeMeshNode3D::STOP && snake->getDirection() < SnakeMeshNode3D::CRASH;
                    this->sendClientPauseToggle();
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
                    if (netEnabled) {
                        this->resetNetworkState();
                    }
                    const std::string ip = mainMenuScene->getJoinIp();
                    this->netIsClient = netClient.connect(ip, netPort, "Player");
                    if (netIsClient) {
                        this->netEnabled = true;
                        mainMenuScene.get()->setNetworkStatus("Status: connecting");
                        mainMenuScene.get()->setNetworkSessionState(MainMenuScene::NetworkSessionState::Client);
                        if constexpr (isDebug) {
                            std::cout << "[Net] Joining " << ip << ":" << netPort << std::endl;
                        }
                    } else {
                        mainMenuScene.get()->setNetworkStatus("Status: join failed");
                        if constexpr (isDebug) {
                            std::cout << "[Net] Failed to join " << ip << ":" << netPort << std::endl;
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

        if (netEnabled && netIsClient) {
            Net::InputMsg input{};
            input.tick = netClock.getTick();
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
                const bool sent = netClient.sendInput(input);
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
        if (netEnabled && netIsClient) {
            return;
        }
        if (menuVisible && !(netEnabled && netIsServer)) {
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
        constexpr auto spotAmbientColor = glm::vec3(0.5f, 0.5f, 0.5f);
        constexpr auto spotSpecularColor = glm::vec3(0.1f, 0.1f, 0.1f);
        const auto spotLight = make_shared<SpotLight>();
        spotLight->setPosition({-1.7521, -0.75, -1.5f});
        spotLight->setDirection({0.0, -1.0, 0.0f});
        spotLight->setAmbient(spotAmbientColor);
        spotLight->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight->setSpecular(spotSpecularColor);
        spotLight->setCutOff(12.5);
        spotLight->setOuterCutOff(17.5);
        spotLight->setPulse(true);

        const auto spotLight2 = make_shared<SpotLight>();
        spotLight2->setPosition({3.67, 2.81, -1.5f});
        spotLight2->setDirection({1.9, 3.0, 0.0f});
        spotLight2->setAmbient(spotAmbientColor);
        spotLight2->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight2->setSpecular(spotSpecularColor);
        spotLight2->setCutOff(12.5);
        spotLight2->setOuterCutOff(17.5);
        spotLight2->setPulse(true);

        const auto spotLight3 = make_shared<SpotLight>();
        spotLight3->setPosition({-1.7521, 2.81, -1.5f});
        spotLight3->setDirection({0.0, 3.0, 0.0f});
        spotLight3->setAmbient(spotAmbientColor);
        spotLight3->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight3->setSpecular(spotSpecularColor);
        spotLight3->setCutOff(12.5);
        spotLight3->setOuterCutOff(17.5);
        spotLight3->setPulse(true);

        const auto spotLight4 = make_shared<SpotLight>();
        spotLight4->setPosition({3.67, -0.75, -1.5f});
        spotLight4->setDirection({2.0f, -1.0, 0.0f});
        spotLight4->setAmbient(spotAmbientColor);
        spotLight4->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight4->setSpecular(spotSpecularColor);
        spotLight4->setCutOff(12.5);
        spotLight4->setOuterCutOff(17.5);
        spotLight4->setPulse(true);

        const auto spotLight5 = make_shared<SpotLight>();
        spotLight5->setPosition({1.93, 0.43, -1.5f});
        spotLight5->setDirection({1.93f, 0.43, 0.0f});
        spotLight5->setAmbient(glm::vec3(1.0f, 0.95f, 0.8f));
        spotLight5->setDiffuse({0.991f, 0.982f, 0.305f});
        spotLight5->setSpecular({0.4f, 0.4f, 0.4f});
        spotLight5->setConstant(0.9f);
        spotLight5->setCutOff(7.5);
        spotLight5->setOuterCutOff(13.5);
        spotLight5->setPulse(true);
        spotLight5->setVisible(false);

        const auto pointLight1 = make_shared<PointLight>();
        pointLight1->setPosition({-0.03, 0.201801, -0.656399});
        pointLight1->setAmbient({0.05f, 0.05f, 0.05f});
        pointLight1->setDiffuse({1.0f, 0.95f, 0.8f});
        pointLight1->setSpecular({0.01f, 0.01f, 0.01f});
        pointLight1->setConstant(1.0f);
        pointLight1->setLinear(8.09f);
        pointLight1->setQuadratic(0.032f);

        const auto pointLight2 = make_shared<PointLight>();
        pointLight2->setPosition({2.15, 1.2218, -0.656399});
        pointLight2->setAmbient({0.1f, 0.0f, 0.0f});
        pointLight2->setDiffuse({0.88f, 0.0f, 0.00f});
        pointLight2->setSpecular({0.0f, 0.0f, 0.0f});
        pointLight2->setConstant(1.0f);
        pointLight2->setLinear(0.7f);
        pointLight2->setQuadratic(20.8f);

        this->spotLights.push_back(spotLight);
        this->spotLights.push_back(spotLight2);
        this->spotLights.push_back(spotLight3);
        this->spotLights.push_back(spotLight4);
        this->spotLights.push_back(spotLight5);

        this->pointLights.push_back(pointLight1);
        this->pointLights.push_back(pointLight2);

        if (manipulatorHandler) {
            manipulatorHandler->getLightsHandler()->addItem(pointLight1);
            manipulatorHandler->getLightsHandler()->addItem(pointLight2);
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
        netEnabled = false;
        netIsServer = false;
        netIsClient = false;
        netPeerId = 0;
        netSeed = 0;
        netClock.reset(0);

        netPort = 7777;
        std::ifstream file("Assets/config.json");
        if (file.is_open()) {
            try {
                nlohmann::json j;
                file >> j;
                if (j.contains("network") && j["network"].contains("port")) {
                    netPort = j["network"]["port"].get<uint16_t>();
                }
            } catch (...) {
                netPort = 7777;
            }
        }

        if (mainMenuScene) {
            mainMenuScene->setLocalIp(Net::getLocalIpAddress());
            mainMenuScene->setNetworkStatus("Status: idle");
        }
    }

    void MainScene::resetNetworkState() {
        notifyNetworkDisconnect();
        netManager.shutdown();
        netEnabled = false;
        netIsServer = false;
        netIsClient = false;
        netPeerId = 0;
        netSeed = 0;
        netLastSnapshotLevel = 0;
        netLastSnapshotEatCounter = 0;
        localRespawnSerial = 0;
        remoteRespawnSerial = 0;
        netLastSeenLocalRespawnSerial = 0;
        netLastSeenRemoteRespawnSerial = 0;
        netLastSeenLocalCrash = false;
        netLastSeenRemoteCrash = false;
        localMultiplayerSpawnPos = {23.0f, -3.0f, -23.0f};
        pendingLocalRespawn = {};
        netClock.reset(0);
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

    void MainScene::notifyNetworkDisconnect() const {
        if (!netEnabled) {
            return;
        }

        if (netIsServer) {
            netManager.disconnectAllNow(0);
        } else if (netIsClient) {
            netManager.disconnectNow(0, 0);
        }

        netManager.flush();
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

    void MainScene::sendClientPauseToggle() const {
        if (!netEnabled || !netIsClient) {
            return;
        }

        Net::InputMsg input{};
        input.tick = netClock.getTick();
        input.actions = 1;
        input.moveX = 0;
        input.moveY = 0;
        const bool sent = netClient.sendInput(input);
        if (!sent) {
            if constexpr (isDebug) {
                std::cout << "[Net] Failed to send pause toggle to server" << std::endl;
            }
        }
    }

    void MainScene::initializeMultiplayerState() {
        gameStarted = true;
            if (playerScene) {
                playerScene->setInputEnabled(!netIsClient);
                if (const auto snake = playerScene->getSnake()) {
                snake->setDirection(SnakeMeshNode3D::NONE);
                snake->setRotationX(90.0f);
                snake->setRotationY(netIsClient ? 180.0f : 0.0f);
                if (netIsClient) {
                    const glm::vec3 clientHead = findRemoteSpawnPosition();
                    localMultiplayerSpawnPos = clientHead;
                    snake->respawn();
                    if (const auto moveHandler = playerScene->getSnakeMoveHandler()) {
                        moveHandler->resetState();
                        moveHandler->setInitialBodyDirection(SnakeMeshNode3D::LEFT);
                    }
                    const auto spawnPositions = Net::buildStraightSnakePositions({clientHead.x, clientHead.y}, 6,
                                                                                SnakeMeshNode3D::LEFT);
                    Net::applyExactSnakePositions(snake, spawnPositions, SnakeMeshNode3D::LEFT, false);
                } else {
                    localMultiplayerSpawnPos = {23.0f, -3.0f, -23.0f};
                    pendingLocalRespawn = {};
                    snake->respawn();
                }
                }
            }
        if (remoteSnakeScene) {
            remoteSnakeScene->setServerControlled(netIsServer);
            remoteSnakeScene->setActive(true);
            if (netIsServer) {
                remoteSnakeScene->setSpawnLayout(findRemoteSpawnPosition(), SnakeMeshNode3D::LEFT);
            } else {
                remoteSnakeScene->setSpawnLayout({23.0f, -3.0f, -23.0f}, SnakeMeshNode3D::RIGHT);
            }
        }
        rebuildRadarItems(true);
        if (mainMenuScene) {
            mainMenuScene->setNetworkSessionState(netIsServer ? MainMenuScene::NetworkSessionState::Hosting
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

        if (netEnabled && netIsServer) {
            ++localRespawnSerial;
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
        if (!netEnabled) {
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
        if (netEnabled && netIsServer) {
            ++remoteRespawnSerial;
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

    void MainScene::updateNetworking() {
        if (!netEnabled) {
            return;
        }

        netClock.advance(1);

        Net::NetEvent event{};
        Net::PacketView packet{};

        if (netIsServer) {
            while (netServer.poll(event, packet)) {
                if (event.type == Net::NetEventType::Connect) {
                    if constexpr (isDebug) {
                        std::cout << "[Net] Client connected: " << event.peerId << std::endl;
                    }
                    continue;
                }
                if (event.type == Net::NetEventType::Disconnect) {
                    if constexpr (isDebug) {
                        std::cout << "[Net] Client disconnected: " << event.peerId << std::endl;
                    }
                    shutdownMultiplayerState(true);
                    continue;
                }
                if (event.type != Net::NetEventType::Receive) {
                    continue;
                }

                const auto decoded = Net::NetDispatcher::decode(packet);
                if (!decoded.has_value()) {
                    continue;
                }

                if (auto *hello = std::get_if<Net::HelloMsg>(&decoded.value())) {
                    Net::WelcomeMsg welcome{};
                    welcome.protocolVersion = Net::Protocol::kProtocolVersion;
                    welcome.assignedPeerId = event.peerId;
                    welcome.serverTick = netClock.getTick();
                    welcome.seed = netSeed;
                    bool result = netServer.sendWelcome(event.peerId, welcome);
                    if (mainMenuScene) {
                        mainMenuScene->setNetworkStatus("Status: client connected");
                    }
                    startNetworkGame();
                    if constexpr (isDebug) {
                        std::cout << "[Net] Hello from peer " << event.peerId << " name=" << hello->name << std::endl;
                    }
                } else if (auto *input = std::get_if<Net::InputMsg>(&decoded.value())) {
                    if (remoteSnakeScene) {
                        remoteSnakeScene->applyNetworkInput(input->moveX, input->moveY, input->actions);
                    }
                } else if (auto *ping = std::get_if<Net::PingMsg>(&decoded.value())) {
                    Net::PongMsg pong{};
                    pong.timeMs = ping->timeMs;
                    bool result = netServer.sendPong(event.peerId, pong);
                }
            }

            if (netEnabled) {
                if (playerScene && coinScene && levelManager) {
                    const auto snake = playerScene->getSnake();
                    const auto coin = coinScene->getCoin();
                    const auto remoteSnake = remoteSnakeScene ? remoteSnakeScene->getSnake() : nullptr;
                    if (snake && coin && remoteSnake) {
                        const auto localPositions = collectSnakePositions(snake);
                        const auto remotePositions = remoteSnakeScene->collectPositions();

                        Net::BufferWriter writer(4096);
                        Net::WorldSnapshotState snapshot{};
                        snapshot.localSnake.positions = localPositions;
                        snapshot.localSnake.direction = snake->getDirection();
                        snapshot.localSnake.segmentCount = static_cast<uint32_t>(snake->getChildren().size() + 1);
                        snapshot.localSnake.respawnSerial = localRespawnSerial;
                        snapshot.localSnake.crashActive = snake->isCrashing();
                        snapshot.localSnake.stopped = snakeMoveHandler && snakeMoveHandler->isStopped();
                        snapshot.remoteSnake.positions = remotePositions;
                        snapshot.remoteSnake.direction = remoteSnake->getDirection();
                        snapshot.remoteSnake.segmentCount = static_cast<uint32_t>(remoteSnake->getChildren().size() + 1);
                        snapshot.remoteSnake.respawnSerial = remoteRespawnSerial;
                        snapshot.remoteSnake.crashActive = remoteSnake->isCrashing();
                        snapshot.remoteSnake.stopped = remoteSnakeScene->getMoveHandler() &&
                                                       remoteSnakeScene->getMoveHandler()->isStopped();
                        const glm::vec3 coinPos = coin->getPosition();
                        snapshot.coinX = coinPos.x;
                        snapshot.coinY = coinPos.y;
                        snapshot.coinVisible = coin->isVisible();
                        snapshot.level = static_cast<uint32_t>(levelManager->getLevel());
                        snapshot.eatCounter = static_cast<uint32_t>(levelManager->getEatCounter());
                        snapshot.lives = static_cast<uint32_t>(levelManager->getLive());
                        snapshot.winning = winning;
                        Net::writeWorldSnapshotState(writer, snapshot);

                        Net::SnapshotMsg snap{};
                        snap.tick = netClock.getTick();
                        snap.payload = writer.data();
                        netServer.broadcastSnapshot(snap);
                    }
                }
            }
        } else if (netIsClient) {
            while (netClient.poll(event, packet)) {
                if (event.type == Net::NetEventType::Disconnect) {
                    if constexpr (isDebug) {
                        std::cout << "[Net] Disconnected from server" << std::endl;
                    }
                    shutdownMultiplayerState(true);
                    continue;
                }
                if (event.type != Net::NetEventType::Receive) {
                    continue;
                }

                const auto decoded = Net::NetDispatcher::decode(packet);
                if (!decoded.has_value()) {
                    continue;
                }

                if (auto *welcome = std::get_if<Net::WelcomeMsg>(&decoded.value())) {
                    netPeerId = welcome->assignedPeerId;
                    netSeed = welcome->seed;
                    if (mainMenuScene) {
                        mainMenuScene->setNetworkStatus("Status: connected");
                    }
                    startNetworkGame();
                    if constexpr (isDebug) {
                        std::cout << "[Net] Welcome: peerId=" << netPeerId << " seed=" << netSeed << std::endl;
                    }
                } else if (auto *ping = std::get_if<Net::PingMsg>(&decoded.value())) {
                    Net::PongMsg pong{};
                    pong.timeMs = ping->timeMs;
                    bool result = netClient.sendPong(pong);
                } else if (auto *pong = std::get_if<Net::PongMsg>(&decoded.value())) {
                    if constexpr (isDebug) {
                        std::cout << "[Net] Pong: " << pong->timeMs << std::endl;
                    }
                } else if (auto *snap = std::get_if<Net::SnapshotMsg>(&decoded.value())) {
                    if (snap->payload.size() >= 4) {
                        Net::BufferReader reader(snap->payload.data(), snap->payload.size());
                        Net::WorldSnapshotState snapshot{};
                        if (Net::readWorldSnapshotState(reader, snapshot)) {
                            if (playerScene && coinScene) {
                                const auto snake = playerScene->getSnake();
                                const auto coin = coinScene->getCoin();
                                const auto &authoritativeSnake = snapshot.localSnake;
                                const auto &clientSnake = snapshot.remoteSnake;
                                if (!snapshot.winning) {
                                    if (snake && remoteSnakeScene && remoteSnakeScene->getSnake()) {
                                        if (clientSnake.positions.empty()) {
                                            continue;
                                        }
                                        const bool localRespawnDetected =
                                            clientSnake.respawnSerial != netLastSeenLocalRespawnSerial;
                                        const bool localCrashActive = clientSnake.crashActive;
                                        const bool remoteCrashActive = authoritativeSnake.crashActive;
                                        netLastSeenLocalRespawnSerial = clientSnake.respawnSerial;
                                        netLastSeenRemoteRespawnSerial = authoritativeSnake.respawnSerial;
                                        if (localRespawnDetected && !pendingLocalRespawn.active) {
                                            if constexpr (isDebug) {
                                                std::cout << "[Net][ClientRespawnDetected] serial=" << clientSnake.respawnSerial
                                                          << " dir=" << static_cast<int>(clientSnake.direction)
                                                          << " count=" << clientSnake.segmentCount
                                                          << " positions=" << clientSnake.positions.size() << std::endl;
                                            }
                                            snake->crash();
                                            playerScene->setInputEnabled(false);
                                            pendingLocalRespawn.positions = clientSnake.positions;
                                            pendingLocalRespawn.segmentCount = clientSnake.segmentCount;
                                            pendingLocalRespawn.direction = clientSnake.direction;
                                            pendingLocalRespawn.active = true;
                                        } else if (localCrashActive) {
                                            if (!netLastSeenLocalCrash) {
                                                snake->crash();
                                                playerScene->setInputEnabled(false);
                                            }
                                        } else if (!pendingLocalRespawn.active) {
                                            Net::applyExactSnakePositions(snake, clientSnake.positions,
                                                                         clientSnake.direction, clientSnake.stopped);
                                        }
                                        if (remoteCrashActive) {
                                            if (const auto remoteSnake = remoteSnakeScene->getSnake()) {
                                                if (!netLastSeenRemoteCrash) {
                                                    remoteSnake->crash();
                                                }
                                            }
                                        }
                                        if (const auto remoteSnake = remoteSnakeScene->getSnake()) {
                                            Net::applyExactSnakePositions(remoteSnake, authoritativeSnake.positions,
                                                                         authoritativeSnake.direction,
                                                                         authoritativeSnake.stopped);
                                        }
                                        netLastSeenLocalCrash = localCrashActive;
                                        netLastSeenRemoteCrash = remoteCrashActive;
                                        remoteSnakeScene->setActive(true);
                                    } else if (snake) {
                                        if (!authoritativeSnake.positions.empty()) {
                                            Net::applyExactSnakePositions(snake, authoritativeSnake.positions,
                                                                         authoritativeSnake.direction,
                                                                         authoritativeSnake.stopped);
                                        }
                                    }
                                }
                                if (coin) {
                                    const bool coinAdvanced = (snapshot.level == netLastSnapshotLevel &&
                                                               snapshot.eatCounter != netLastSnapshotEatCounter);
                                    if (coinAdvanced && snapshot.coinVisible) {
                                        coinScene->getRemoveCoin()->setTransform(coin);
                                        coinScene->getRemoveCoin()->setVisible(true);
                                        coinScene->getRemoveCoin()->animationStart("eatenUp", false);
                                    }
                                    coin->setPosition({snapshot.coinX, snapshot.coinY, coin->getPosition().z});
                                    coin->setVisible(snapshot.coinVisible);
                                    if (snapshot.coinVisible) {
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
                                    if (snapshot.coinVisible && radarMeshNode) {
                                        radarMeshNode->showItem("coin");
                                    }
                                }
                                if (levelManager) {
                                    levelManager->setLevel(static_cast<int>(snapshot.level));
                                    levelManager->setEatCounter(static_cast<int>(snapshot.eatCounter));
                                    levelManager->setLive(static_cast<int>(snapshot.lives));
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
                                    if (!winning && tilesCounterNode) {
                                        tilesCounterNode->setVisible(true);
                                    }
                                }
                                if (snapshot.winning) {
                                    enterWinningState();
                                }
                                netLastSnapshotLevel = snapshot.level;
                                netLastSnapshotEatCounter = snapshot.eatCounter;
                            }
                        }
                    }
                }
            }
        }
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

            for (const auto &child : barriersScene->getLevelBoxes()->getChildren()) {
                if (matchesNode(child)) {
                    return true;
                }
            }
            return false;
        };

        for (float y = 71.0f; y >= -21.0f; y -= 2.0f) {
            for (float x = 61.0f; x >= -11.0f; x -= 2.0f) {
                bool valid = true;
                for (int segment = 0; segment < 6; ++segment) {
                    if (isBlocked({x + static_cast<float>(segment * 2), y})) {
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
                if ((!netEnabled || netIsServer) && this->eatManager && coinScene && !coinScene->getCoin()->isVisible()) {
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
                if ((!netEnabled || netIsServer) && this->eatManager && coinScene && !coinScene->getCoin()->isVisible()) {
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
        rebuildRadarItems(netEnabled);
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
        updateNetworking();
        if (netEnabled && netIsServer && remoteSnakeScene) {
            remoteSnakeScene->updateAuthoritative();
        }
        Scene::update();
        if (pendingLocalRespawn.active && playerScene && playerScene->getSnake() && playerScene->getSnake()->isReady()) {
            if constexpr (isDebug) {
                std::cout << "[Net][ClientRespawnApply] dir=" << static_cast<int>(pendingLocalRespawn.direction)
                          << " count=" << pendingLocalRespawn.segmentCount
                          << " positions=" << pendingLocalRespawn.positions.size() << std::endl;
            }
            const auto snake = playerScene->getSnake();
            snake->respawn();
            if (const auto moveHandler = playerScene->getSnakeMoveHandler()) {
                moveHandler->resetState();
                moveHandler->setInitialBodyDirection(
                    pendingLocalRespawn.direction == SnakeMeshNode3D::NONE ? SnakeMeshNode3D::LEFT : pendingLocalRespawn.direction);
            }
            Net::applyExactSnakePositions(snake, pendingLocalRespawn.positions, pendingLocalRespawn.direction, false);
            pendingLocalRespawn = {};
            playerScene->setInputEnabled(!netIsClient);
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
            if (!helpText->isVisible() && eatManager && (!netEnabled || netIsServer)) {
                eatManager->run(EatManager::checkPlace);
            }
        }
    }

    void MainScene::setCursorPosition(const glm::vec2 &position) {
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
                if (netEnabled) {
                    this->resetNetworkState();
                }
                this->netIsServer = netServer.start(netPort);
                if (netIsServer) {
                    this->netEnabled = true;
                    this->netSeed = static_cast<uint32_t>(std::random_device{}());
                    mainMenuScene.get()->setNetworkStatus("Status: hosting");
                    mainMenuScene.get()->setNetworkSessionState(MainMenuScene::NetworkSessionState::Hosting);
                    if constexpr (isDebug) {
                        std::cout << "[Net] Hosting on port " << netPort << std::endl;
                    }
                } else {
                    mainMenuScene.get()->setNetworkStatus("Status: host failed");
                    if constexpr (isDebug) {
                        std::cout << "[Net] Failed to host on port " << netPort << std::endl;
                    }
                }
                break;
            }
            case MainMenuScene::MenuAction::Disconnect:
                shutdownMultiplayerState(true);
                break;
            case MainMenuScene::MenuAction::Join: {
                if (netEnabled) {
                    this->resetNetworkState();
                }
                const std::string ip = mainMenuScene->getJoinIp();
                this->netIsClient = netClient.connect(ip, netPort, "Player");
                if (netIsClient) {
                    this->netEnabled = true;
                    mainMenuScene.get()->setNetworkStatus("Status: connecting");
                    mainMenuScene.get()->setNetworkSessionState(MainMenuScene::NetworkSessionState::Client);
                    if constexpr (isDebug) {
                        std::cout << "[Net] Joining " << ip << ":" << netPort << std::endl;
                    }
                } else {
                    mainMenuScene.get()->setNetworkStatus("Status: join failed");
                    if constexpr (isDebug) {
                        std::cout << "[Net] Failed to join " << ip << ":" << netPort << std::endl;
                    }
                }
                break;
            }
            case MainMenuScene::MenuAction::Start:
                if (netEnabled) {
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
                if (netEnabled) {
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
                if (netEnabled) {
                    notifyNetworkDisconnect();
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
        if (netEnabled && netIsClient) {
            if (resumeLocalMovementAfterMenu) {
                sendClientPauseToggle();
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
