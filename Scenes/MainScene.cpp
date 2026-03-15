#include "MainScene.h"

#include <glm/gtc/random.hpp>
#include <iostream>

#include "PlayerScene.h"
#include "TorchScene.h"
#include "WeatherScene.h"
#include "../Resource/ShaderLoader.h"
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

    void MainScene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
        if (winning) {
            return;
        }

        Scene::keyboardInput(window, keyCode, scancode, action, mods);

        switch (keyCode) {
            case GLFW_KEY_V:
                rendererManager->toggleShadows();
                break;
            case GLFW_KEY_B:
                rendererManager->toggleBloom();
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

    void MainScene::initSounds() const {
        const bool musicLoaded = soundManager->addSoundFromFile("music", "Assets/Sounds/snake.wav", true);
        const bool coinLoaded = soundManager->addSoundFromFile("coin", "Assets/Sounds/coin.wav");
        if (!musicLoaded || !coinLoaded) {
            std::cerr << "Sound loading error in MainScene." << std::endl;
            return;
        }
        soundManager->play("music", {{AL_LOOPING, AL_TRUE}});
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
        radarMeshNode->addItem(playerScene->getSnake(), glm::vec3(0.0,1.0,0.0), "snake");
        radarMeshNode->addItem(coinScene->getCoin(), glm::vec3(1.0,1.0,0.0), "coin");
        radarMeshNode->addItem(barriersScene->getLevelBoxes(), glm::vec3(1.0,0.0,0.0), "barriers");
        radarMeshNode->hideItems();

        addMeshNode2D(radarMeshNode);
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
    }

    void MainScene::initPreloader() {
        preLoader = make_shared<Preloader2Scene>(nullptr,
            vector<shared_ptr<SpotLight> >{},
            vector<shared_ptr<PointLight> >{},
            rendererManager, camera, projection, resourceManager, width, height);
        preLoader->init(-10);
        addNode("scenePreloader", preLoader);
    }

    void MainScene::buildEatenUpCallback() {
        snakeMoveHandler->setEatenUpCallback([this]() {
            if (this->levelManager) {
                soundManager->play("coin");

                coinScene->getRemoveCoin()->setTransform(coinScene->getCoin());
                coinScene->getRemoveCoin()->setVisible(true);
                coinScene->getRemoveCoin()->animationStart("eatenUp", false);

                this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

                if (this->levelManager->getEatCounter() == MAX_POINT) {
                    fadeOutUniform->setAlpha(1.0f);
                    coinScene->getCoin()->setVisible(false);
                    if (levelManager->getLevel() < 9) {
                        nextLevel();
                    } else {
                        // end game....winner
                        cout << "End game... you win !" << endl;
                        tilesCounterNode->setVisible(false);
                        playerScene->winning();
                        // zastavit hada a schovat ho (nova funkce do playerScene)
                        addNode("winner", winnerScene);
                        radarMeshNode->setVisible(false);
                        // zobrazi napis, ze player vyhral
                        // nastavit kameru do nejakeho winning mode (oddalit a rotovat nad scenou)
                        // disablovat ovladani hada
                        winning = true;
                    }
                } else {
                    this->eatManager->run(EatManager::eatenUp);
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
                tilesCounterText->setText(buffAsStdStr);
            }
        });
    }

    void MainScene::buildStartMoveCallback() const {
        snakeMoveHandler->addStartMoveCallback([this]() {
            if (this->levelManager) {
                this->eatManager->run(EatManager::firstPlace);
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
    }

    void MainScene::buildCrashCallback() {
        snakeMoveHandler->setCrashCallback([this]() {
            if (this->levelManager) {
                this->eatManager->run(EatManager::clean);
                playerScene->getSnake()->crash();
                this->levelManager->setLive(this->levelManager->getLive() - 1);
                this->levelManager->setEatCounter(0);
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
                tilesCounterText->setText(buffAsStdStr);
                coinScene->getCoin()->setVisible(false);
                coinScene->getCoin()->animationStop("coinRotation");
                playerScene->getSnake()->animationStop("KostraAction");
                radarMeshNode->hideItem("coin");
                if (this->levelManager->getLive() == 0) {
                    // Game Over
                    // this->levelManager->createLevel(1, directionalLight, spotLights, pointLights);
                    // fadeOutUniform->setAlpha(1.0f);
                    // this->levelManager->setLive(3);
                    cout << "crash callback call" << endl;
                }
            }
        });
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
        radarMeshNode->clearItems();
        radarMeshNode->addItem(playerScene->getSnake(), glm::vec3(0.0,1.0,0.0), "snake");
        radarMeshNode->addItem(coinScene->getCoin(), glm::vec3(1.0,1.0,0.0), "coin");
        removeNode("barriers");
        barriersScene->nextLevel();
        addNode("barriers", barriersScene);
        eatLocationHandler->setBarriers(barriersScene->getLevelBoxes());
        radarMeshNode->addItem(barriersScene->getLevelBoxes(), glm::vec3(1.0,0.0,0.0), "barriers");
        playerScene->getSnake()->respawn();
        eatManager->run(EatManager::clean);
    }

    void MainScene::update() {
        Scene::update();
        if (loading) {
            prepareScene();
        }

        if (radarMeshNode != nullptr) {
            if (radarMeshNode->isVisible() && radarFadeOutUniform->getAlpha() <= 0) {
                radarMeshNode->setVisible(false);
            }
        }

        if (helpText != nullptr) {
            if (!helpText->isVisible()) {
                eatManager->run(EatManager::checkPlace);
            }
        }
    }
} // Scenes
