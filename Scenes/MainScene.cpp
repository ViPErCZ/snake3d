#include "MainScene.h"
#include "PlayerScene.h"
#include "../Renderer/Opengl/AnimRenderer.h"
#include "../Renderer/Opengl/BarrierRenderer.h"
#include "../Renderer/Opengl/SkyboxRenderer.h"
#include "../Renderer/Opengl/SnakeRenderer.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

namespace Scenes {
    MainScene::MainScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    }

    void MainScene::init() {
        Scene::init();
        initPlayerScene();
        initBarriersScene();
        initSkybox();
        initPlane();
        initEat();
        initRadar();
        initEatManager();
        initLabels();

        buildStartMoveCallback();
        buildEatenUpCallback();
    }

    void MainScene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
        Scene::keyboardInput(window, keyCode, scancode, action, mods);

        switch (keyCode) {
            case GLFW_KEY_V:
                rendererManager->toggleShadows();
                break;
            case GLFW_KEY_B:
                rendererManager->toggleBloom();
                if (snakeRenderer) {
                     snakeRenderer->toggleBlur();
                }
                break;
            case GLFW_KEY_F:
                rendererManager->toggleFog();
                break;
            case GLFW_KEY_M:
                playerScene->getSnake()->respawn();
                break;
            case GLFW_KEY_R:
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
                break;
            default:
                break;
        }
    }

    void MainScene::initPlayerScene() {
        playerScene = make_shared<PlayerScene>(rendererManager, camera, projection, resourceManager, width, height);
        playerScene->init();
        snakeMoveHandler = playerScene->getSnakeMoveHandler();
        collisionDetector = make_shared<CollisionDetector>();
        snakeMoveHandler->setCollisionDetector(collisionDetector);
        addNode(playerScene);
    }

    void MainScene::initSkybox() {
        const auto skybox = make_shared<Cube>();
        const auto skyboxRenderer = make_shared<SkyboxRenderer>(skybox, camera.get(), projection, resourceManager.get());
        rendererManager->addRenderer(skyboxRenderer);
    }

    void MainScene::initPlane() {
        auto basicShader = resourceManager->getShader("basicShader");
        auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
        const auto shadowMap = resourceManager->getTexture("depth");
        const auto gamefieldAlbedo = resourceManager->getTexture("tile.png");
        const auto gamefieldNormal = resourceManager->getTexture("gamefield_normal.jpg");
        const auto gamefieldSpecular = resourceManager->getTexture("gamefield_specular.jpg");
        const auto planeMaterial = make_shared<StandardMaterial>(basicShader, shadowDepthShader);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.07f, 0.07f, 0.07f});
        directionalLight->setDiffuse({0.0f, 0.0f, 0.0f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        planeMaterial->setDirectionalLight(directionalLight);
        planeMaterial->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        planeMaterial->setShadow(shadowMap);
        planeMaterial->setNormalEnabled(true);
        planeMaterial->setAlbedo(gamefieldAlbedo);
        planeMaterial->setNormal(gamefieldNormal);
        planeMaterial->setSpecular(gamefieldSpecular);
        planeMaterial->set_uv_scale(glm::vec2(48.0f, 48.0f));

        auto planeMesh = make_shared<PlaneMesh>(basicShader, 4, 4);
        planeMesh->setMaterial(planeMaterial);
        const auto node3d = make_shared<MeshNode3D>(planeMesh, resourceManager);
        node3d->setRotationX(90);
        node3d->setPosition({1.0, 1.0, -1.0});

        meshNode3d.push_back(node3d);
    }

    void MainScene::initBarriersScene() {
        barriersScene = make_shared<BarriersScene>(rendererManager, camera, projection, resourceManager, width, height);
        barriersScene->init();
        levelManager = barriersScene->getLevelManager();
        addNode(barriersScene);
    }

    void MainScene::initEat() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const std::shared_ptr<ObjItem> coinModel(
            resourceManager->getModel("coin"), [](ObjItem *) {
        });
        const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(shader));
        coinMesh->fromObj(coinModel);

        coinMeshNode3D = make_shared<CoinMeshNode3D>(coinMesh, resourceManager);
        coinMeshNode3D->setPosition({-69.0, -69, -70.0f});
        coinMeshNode3D->setScale({0.013888889, 0.013888889, 0.013888889});
        coinMeshNode3D->setRotationX(90);
        coinMeshNode3D->setVisible(false);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
        const auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
        const auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
        auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");
        const auto coinMaterial = make_shared<StandardMaterial>(shader, shadowsShader);
        coinMaterial->setAlbedo(coinAlbedo);
        coinMaterial->setNormal(coinNormal);
        coinMaterial->setSpecular(coinMetalness);
        coinMaterial->setNormalEnabled(true);
        coinMaterial->setDirectionalLight(directionalLight);

        coinMesh->setMaterial(coinMaterial);

        collisionDetector->addStaticItem(coinMeshNode3D);

        meshNode3d.push_back(coinMeshNode3D);
    }

    void MainScene::initEatManager() {
        auto eatLocationHandler = make_shared<EatLocationHandler>(
            barriersScene->getLevelBoxes(),
            playerScene->getSnake(),
            coinMeshNode3D
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

        //const auto shader = resourceManager->getShader("basic2d");
        radarNode = make_shared<QuadNode2D>(220, 220, nullptr);
        radarNode->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        radarNode->setMaterial(radarExpansionIn);
        radarMeshNode = make_shared<RadarMeshNode2D>(radarNode, resourceManager);
        radarMeshNode->setPosition({width - 240 + 100, 30.0 + 110, 0.0}); // + 100 kvuli tomu, ze stred neni 0,0 ale stred quadu
        radarMeshNode->addItem(playerScene->getSnake(), glm::vec3(0.0,1.0,0.0), "snake");
        radarMeshNode->addItem(coinMeshNode3D, glm::vec3(1.0,1.0,0.0), "coin");
        radarMeshNode->addItem(barriersScene->getLevelBoxes(), glm::vec3(1.0,0.0,0.0), "barriers");
        radarMeshNode->hideItems();

        meshNode2d.push_back(radarMeshNode);
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

        label->setMaterial(shaderMaterial);
        helpText = make_shared<MeshNode2D>(label, resourceManager);

        tilesCounterText = make_shared<LabelNode2D>("", shader, settings);
        fadeInUniform = make_shared<FadeInUniform>();
        const auto shaderMaterial2 = make_shared<ShaderMaterial>(shader);
        shaderMaterial2->addUniform("alpha", fadeInUniform);
        shaderMaterial2->addUniform("textColor", glm::vec3(1.0f));
        shaderMaterial2->addUniform("textTexture", 0);

        tilesCounterText->setMaterial(shaderMaterial2);
        tilesCounterNode = make_shared<MeshNode2D>(tilesCounterText, resourceManager);
        tilesCounterNode->setVisible(false);

        meshNode2d.push_back(helpText);
        meshNode2d.push_back(tilesCounterNode);
    }

    void MainScene::buildEatenUpCallback() const {
        snakeMoveHandler->setEatenUpCallback([this]() {
            if (this->levelManager) {
                // alSourcePlay (coinSource);
                //
                // if (const ALCenum error = alGetError(); error != AL_NO_ERROR) {
                //     cout << "Sound error" << endl;
                // }
                //
                // if (this->eatRemoveAnimateRenderer && this->animateEat) {
                //     this->animateEat->setPosition(eat->getPosition());
                //     this->animateEat->setVisible(true);
                //     this->animateEat->fadeOut();
                // }

                this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

                if (this->levelManager->getEatCounter() == MAX_POINT) {
                    //     this->startText->setVisible(true);
                    //     playerScene->getSnake()->respawn();
                    //     this->eat->setVisible(false);
                    //     this->levelManager->createLevel(this->levelManager->getLevel() + 1);
                    //     this->eatManager->run(Manager::EatManager::clean);
                    //     initRadar();
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
                this->eatManager->run(Manager::EatManager::firstPlace);
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
            }
        });
    }

    void MainScene::buildCrashCallback() const {
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
                coinMeshNode3D->setVisible(false);
                if (this->levelManager->getLive() == 0) {
                    // Game Over
                    this->levelManager->createLevel(1);
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

        if (!helpText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
            eatManager->run(EatManager::checkPlace);
        }
    }
} // Scenes
