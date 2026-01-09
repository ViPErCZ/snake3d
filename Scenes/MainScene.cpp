#include "MainScene.h"

#include <glm/gtc/random.hpp>

#include "PlayerScene.h"
#include "TorchScene.h"
#include "WeatherScene.h"
#include "../Resource/ShaderLoader.h"
#include "../Renderer/Opengl/SkyboxRenderer.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Material/Uniform/TextureUniform.h"
#include "../Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"
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
        spotLight5->setAmbient(glm::vec3(0.88, 0.00, 0.09));
        spotLight5->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight5->setSpecular({0.0f, 0.0f, 0.0f});
        spotLight5->setCutOff(7.5);
        spotLight5->setOuterCutOff(13.5);
        spotLight5->setPulse(true);

        this->spotLights.push_back(spotLight);
        this->spotLights.push_back(spotLight2);
        this->spotLights.push_back(spotLight3);
        this->spotLights.push_back(spotLight4);
        this->spotLights.push_back(spotLight5);
    }

    void MainScene::init(const int priority) {
        Scene::init(priority);

        positionHandler = make_shared<PositionHandler>(camera);
        keyboardManager->addEventHandler(positionHandler);
        collisionDetector = make_shared<CollisionDetector>();
        initPlayerScene();
        initBarriersScene();
        initCoinScene();
        initTorchScene();
        initWeatherScene();

        initSkybox();
        initPlane();
        initRadar();
        initEatManager();
        initLabels();

        buildStartMoveCallback();
        buildEatenUpCallback();

        // DEBUG
        // ====================
        const auto shader = resourceManager->getShader("arrowGizmo");
        const auto dirLightNode = make_shared<DirectionalLightNode3D>(contextState, shader, resourceManager);
        dirLightNode->setDirectionalLight(directionalLight);
        addMeshNode3D(dirLightNode);

        positionHandler->addItem(directionalLight);
        for (auto &spotLight : spotLights) {
            positionHandler->addItem(spotLight);
        }
    }

    void MainScene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
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

    void MainScene::initSkybox() {
        auto basicShader = resourceManager->getShader("skyboxShader");
        const auto skybox = make_shared<Cube>();
        const auto skybox2 = make_shared<BoxMesh>(basicShader, 100, 100, 100);
        // udelat shader material
        //const auto textureUniform = make_shared<TextureUniform>(11, resourceManager->getTexture("skybox"));
        //respawnMaterial->addUniform("u_NoiseTexture", textureUniform);
        // udelat skyboxNode + pretizit renderer
        const auto skyboxRenderer = make_shared<SkyboxRenderer>(skybox, camera, projection, resourceManager);
        rendererManager->addRenderer(skyboxRenderer, 1000);
        //addMeshNode3D(skybox2);
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

        // planeMaterialDirLight = make_shared<DirectionalLight>();
        // planeMaterialDirLight->setPosition({0.0f, 7.0f, 11.0f});
        // planeMaterialDirLight->setDirection({1, 1.0, -3});
        // planeMaterialDirLight->setAmbient({0.07f, 0.07f, 0.07f});
        // planeMaterialDirLight->setDiffuse({0.0f, 0.0f, 0.0f});
        // planeMaterialDirLight->setSpecular({.091f, .091f, .091f});

        planeMaterial->setDirectionalLight(directionalLight);
        planeMaterial->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        planeMaterial->setShadow(shadowMap);
        planeMaterial->setNormalEnabled(true);
        planeMaterial->setAlbedo(gamefieldAlbedo);
        planeMaterial->setNormal(gamefieldNormal);
        planeMaterial->setSpecular(gamefieldSpecular);
        planeMaterial->set_uv_scale(glm::vec2(48.0f, 48.0f));

        for (auto &spotLight : spotLights) {
            planeMaterial->addSpotLight(spotLight);
        }

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
        playerScene->init(2);
        snakeMoveHandler = playerScene->getSnakeMoveHandler();
        snakeMoveHandler->setCollisionDetector(collisionDetector);
        addNode(playerScene);
    }

    void MainScene::initBarriersScene() {
        barriersScene = make_shared<BarriersScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        barriersScene->init(3);
        levelManager = barriersScene->getLevelManager();
        addNode(barriersScene);
    }

    void MainScene::initCoinScene() {
        coinScene = make_shared<CoinScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        coinScene->init(4);
        addNode(coinScene);

        collisionDetector->addStaticItem(coinScene->getCoin());
    }

    void MainScene::initTorchScene() {
        const auto torchScene = make_shared<TorchScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        torchScene->init(1);
        addNode(torchScene);
    }

    void MainScene::initWeatherScene() {
        const auto weatherScene = make_shared<WeatherScene>(directionalLight, spotLights, pointLights, rendererManager, camera, projection, resourceManager, width, height);
        weatherScene->init(0);
        addNode(weatherScene);
    }

    void MainScene::initEatManager() {
        auto eatLocationHandler = make_shared<EatLocationHandler>(
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
        radarExpansionIn->addUniform("quadSize", glm::vec2(200, 200));
        radarExpansionIn->addUniform("borderColor", glm::vec3(1.0,0.0,0.0));
        radarExpansionIn->addUniform("borderWidth", 11.9f);
        radarExpansionIn->addUniform("radius", 8.0f);
        radarExpansionIn->addUniform("expansion", radarFadeInUniform);
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
        radarExpansionOut->addUniform("quadSize", glm::vec2(200, 200));
        radarExpansionOut->addUniform("borderColor", glm::vec3(1.0,0.0,0.0));
        radarExpansionOut->addUniform("borderWidth", 11.9f);
        radarExpansionOut->addUniform("radius", 8.0f);
        radarExpansionOut->addUniform("expansion", radarFadeOutUniform);

        radarNode = make_shared<QuadNode2D>(220, 220, nullptr);
        radarNode->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        radarNode->setMaterial(radarExpansionIn);
        radarMeshNode = make_shared<RadarMeshNode2D>(contextState, radarNode, resourceManager);
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
        shaderMaterial->addUniform("alpha", fadeOutUniform);
        shaderMaterial->addUniform("textColor", glm::vec3(1.0f));
        shaderMaterial->addUniform("textTexture", 0);
        fadeOutUniform->setFinishedCallback([this]() {
            helpText->setVisible(false);
        });

        label->setMaterial(shaderMaterial);
        helpText = make_shared<MeshNode2D>(contextState, label, resourceManager);

        tilesCounterText = make_shared<LabelNode2D>("", shader, settings);
        fadeInUniform = make_shared<FadeInUniform>();
        const auto shaderMaterial2 = make_shared<ShaderMaterial>(shader);
        shaderMaterial2->addUniform("alpha", fadeInUniform);
        shaderMaterial2->addUniform("textColor", glm::vec3(1.0f));
        shaderMaterial2->addUniform("textTexture", 0);

        tilesCounterText->setMaterial(shaderMaterial2);
        tilesCounterNode = make_shared<MeshNode2D>(contextState, tilesCounterText, resourceManager);
        tilesCounterNode->setVisible(false);

        addMeshNode2D(helpText);
        addMeshNode2D(tilesCounterNode);
    }

    void MainScene::buildEatenUpCallback() const {
        snakeMoveHandler->setEatenUpCallback([this]() {
            if (this->levelManager) {
                // alSourcePlay (coinSource);
                //
                // if (const ALCenum error = alGetError(); error != AL_NO_ERROR) {
                //     cout << "Sound error" << endl;
                // }

                coinScene->getRemoveCoin()->setTransform(coinScene->getCoin());
                coinScene->getRemoveCoin()->setVisible(true);
                coinScene->getRemoveCoin()->animationStart("eatenUp", false);

                this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

                if (this->levelManager->getEatCounter() == MAX_POINT) {
                    fadeOutUniform->setAlpha(1.0f);
                    coinScene->getCoin()->setVisible(false);
                    //     playerScene->getSnake()->respawn();
                    //     this->eat->setVisible(false);
                    //     this->levelManager->createLevel(this->levelManager->getLevel() + 1);
                    //     this->eatManager->run(Manager::EatManager::clean);
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
                fadeOutUniform->start();
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
                fadeInUniform->start();
                radarMeshNode->showItem("coin");
                coinScene->getCoin()->animationStart("coinRotation");
                playerScene->getSnake()->animationStart("KostraAction", true);
            }
        });
    }

    void MainScene::buildCrashCallback() {
        snakeMoveHandler->setCrashCallback([this]() {
            if (this->levelManager) {
                playerScene->getSnake()->respawn();
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
                if (this->levelManager->getLive() == 0) {
                    // Game Over
                    this->levelManager->createLevel(1, directionalLight);
                    fadeOutUniform->setAlpha(1.0f);
                    this->levelManager->setLive(3);
                    cout << "crash callback call" << endl;
                }
            }
        });
    }

    void MainScene::update() {
        Scene::update();

        if (radarMeshNode->isVisible() && radarFadeOutUniform->getAlpha() <= 0) {
            radarMeshNode->setVisible(false);
        }

        if (!helpText->isVisible()) {
            eatManager->run(EatManager::checkPlace);
        }
    }
} // Scenes
