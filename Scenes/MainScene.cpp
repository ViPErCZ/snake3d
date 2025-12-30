#include "MainScene.h"

#include <glm/gtc/random.hpp>

#include "PlayerScene.h"
#include "../Resource/ShaderLoader.h"
#include "../Renderer/Opengl/SkyboxRenderer.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/PlanarReflectionMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/GPUParticle3D.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include "../Renderer/Opengl/Model/Standard/QuadMesh3D.h"
#include "../Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

namespace Scenes {
    MainScene::MainScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    }

    void MainScene::init(const int priority) {
        planarReflectionRenderer = make_shared<PlanarReflectionRenderer>(resourceManager, camera, projection, width, height);
        rendererManager->setPlanarReflectionRenderer(planarReflectionRenderer);

        Scene::init(priority);

        positionHandler = make_shared<PositionHandler>(camera);
        keyboardManager->addEventHandler(positionHandler);
        collisionDetector = make_shared<CollisionDetector>();
        initPlayerScene();
        initBarriersScene();
        initCoinScene();

        initSkybox();
        initPlane();
        initRadar();
        initEatManager();
        initLabels();

        buildStartMoveCallback();
        buildEatenUpCallback();

        // GPU Particle TEST
        // ===========================
        const auto quad = make_shared<QuadMesh3D>(resourceManager->getShader("basicShader"), 1.7, 1.7);
        const auto quad2 = make_shared<QuadMesh3D>(resourceManager->getShader("basicShader"), 1.7, 1.7);
        const auto fire = make_shared<GPUParticle3D>(camera, quad, resourceManager, 1000);
        const auto smoke = make_shared<GPUParticle3D>(camera, quad, resourceManager, 10);
        const auto rain = make_shared<GPUParticle3D>(camera, quad, resourceManager, 6000);
        const auto snow = make_shared<GPUParticle3D>(camera, quad, resourceManager, 6000);
        const auto explosion = make_shared<GPUParticle3D>(camera, quad, resourceManager, 500);
        const auto explosion2 = make_shared<GPUParticle3D>(camera, quad, resourceManager, 500);

        const auto quad2D = make_shared<QuadNode2D>(0.9, 1.2);
        const auto rainDrop2D = make_shared<GPUParticle2D>(quad2D, resourceManager, 5);

        rainDrop2D->setPreset(GPUParticle2D::Preset::RainOnGlass);
        // auto& rp2d = rainDrop2D->getParams();
        // rp2d.spawnRate = 2.0f;
        // rp2d.turbulence = 0.2f;
        // rp2d.lifeMin = 10.0f;
        // rp2d.lifeMax = 20.0f;

        fire->setPosition(glm::vec3(0.0, 0.6, 0.0));
        explosion->setPosition(glm::vec3(0.0, 0.6, 0.0));
        explosion2->setPosition(glm::vec3(2.0, 0.6, 0.0));
        smoke->setPosition(glm::vec3(0.0, 0.783, 0.0));
        fire->setScale({2.2, 2.2, 2.2});
        smoke->setScale({2.0, 2.0, 2.0});
        fire->setRotationX(-90);
        smoke->setRotationX(-90);

        fire->setPreset(GPUParticle3D::Preset::Fire);
        smoke->setPreset(GPUParticle3D::Preset::Smoke);
        rain->setPreset(GPUParticle3D::Preset::Rain);
        snow->setPreset(GPUParticle3D::Preset::Snow);
        explosion->setPreset(GPUParticle3D::Preset::Explosion);
        explosion2->setPreset(GPUParticle3D::Preset::Explosion);
        auto fp = fire->getParams();
        fp.lifeMin = 0.5f;
        fp.lifeMax = 1.0f;
        fp.sizeMin = 0.008f;
        fp.sizeMax = 0.042f;
        fp.velMin  = glm::vec3(-0.005f, 0.000f, 0.100f);
        fp.velMax  = glm::vec3( 0.005f, 0.010f, 0.200f);
        fp.gravity = glm::vec3(glm::linearRand(-0.005f, 0.005f), glm::linearRand(0.01f, 0.001f), glm::linearRand(0.005f, 0.009f));
        fp.emitterRadius = 0.03f;
        fp.emitterRadiusX = 0.03f;
        fp.emitterRadiusZ = 0.0f;
        fp.emitterYOffset = 0.24f;
        fp.spawnPerFrame = 0.6f;
        fp.stretch = 0.105f;
        fp.colorStart = glm::vec4(6.0f, 3.5f, 1.0f, 1.0f);
        fp.colorEnd = glm::vec4(7.0f, 4.5f, 1.5f, 0.0f);
        fp.texture = "fire.png";
        fire->setParams(fp);
        fire->setRenderMode(GPUParticle3D::RenderMode::Textured);

        auto sp = smoke->getParams();
        sp.lifeMax = 0.5f;
        sp.sizeMin = 0.08f;
        sp.sizeMax = 0.042f;
        sp.emitterRadius = 0.03f;
        sp.emitterRadiusX = 0.03f;
        sp.emitterRadiusZ = 0.0f;
        sp.emitterYOffset = 0.24f;
        sp.velMin  = glm::vec3(-0.005f, 0.000f, 0.100f);
        sp.velMax  = glm::vec3( 0.005f, 0.010f, 0.200f);
        sp.texture = "smoke.png";
        smoke->setParams(sp);
        smoke->setRenderMode(GPUParticle3D::RenderMode::Textured);

        auto rp = rain->getParams();
        rp.texture = "rain.png";
        rain->setParams(rp);
        rain->setRenderMode(GPUParticle3D::RenderMode::Textured);

        auto snowParams = snow->getParams();
        snowParams.texture = "snow.png";
        snow->setParams(snowParams);
        snow->setRenderMode(GPUParticle3D::RenderMode::Textured);

        auto explosionParams = explosion->getParams();
        explosionParams.texture = "explosion.png";
        explosion->setParams(explosionParams);
        explosion->setRenderMode(GPUParticle3D::RenderMode::Textured);

        explosionParams = explosion2->getParams();
        explosionParams.texture = "explosion.png";
        explosionParams.timeOffset = 0.2f;
        explosion2->setParams(explosionParams);
        explosion2->setRenderMode(GPUParticle3D::RenderMode::Textured);

        const auto torch = make_shared<ArrayMesh>(resourceManager->getShader("basicShader"));
        torch->fromMesh(resourceManager->getModel("torch"));
        const auto torchNode = make_shared<MeshNode3D>(torch, resourceManager);
        const auto torchNode2 = make_shared<MeshNode3D>(torch, resourceManager);
        const auto torchNode3 = make_shared<MeshNode3D>(torch, resourceManager);
        const auto torchNode4 = make_shared<MeshNode3D>(torch, resourceManager);
        torchNode->setRotationX(90);
        torchNode2->setRotationX(90);
        torchNode3->setRotationX(90);
        torchNode4->setRotationX(90);
        torchNode->setScale({0.2, 0.2, 0.2});
        torchNode2->setScale({0.2, 0.2, 0.2});
        torchNode3->setScale({0.2, 0.2, 0.2});
        torchNode4->setScale({0.2, 0.2, 0.2});
        torchNode->setPosition({-5.07928, -5.47677, -4.98698});
        torchNode2->setPosition({15.2239, -5.47677, -4.98698});
        torchNode3->setPosition({15.2753, 15.4487, -4.98698});
        torchNode4->setPosition({-5.07928, 15.4487, -4.98698});
        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});
        const auto torchAlbedo = resourceManager->getTexture("torch.png");
        const auto torchNormal = resourceManager->getTexture("torch_normal.png");
        const auto torchMaterial = make_shared<StandardMaterial>(resourceManager->getShader("basicShader"), resourceManager->getShader("shadowDepthShader"));
        torchMaterial->setAlbedo(torchAlbedo);
        torchMaterial->setNormal(torchNormal);
        torchMaterial->setNormalEnabled(true);
        torchMaterial->setDirectionalLight(directionalLight);
        torchMaterial->setBlending(Blending::Translucent);
        torch->setMaterial(torchMaterial);


        torchNode->addNode(smoke);
        torchNode2->addNode(smoke);
        torchNode3->addNode(smoke);
        torchNode4->addNode(smoke);
        torchNode->addNode(fire);
        torchNode2->addNode(fire);
        torchNode3->addNode(fire);
        torchNode4->addNode(fire);

        addMeshNode3D(torchNode, 1);
        addMeshNode3D(torchNode2, 1);
        addMeshNode3D(torchNode3, 1);
        addMeshNode3D(torchNode4, 1);
        addMeshNode3D(rain, 1);
        // addMeshNode3D(snow, 1);
        addMeshNode3D(explosion, 1);
        addMeshNode3D(explosion2, 1);
        addMeshNode2D(rainDrop2D, 1);

        positionHandler->addItem(torchNode4);
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
                rendererManager->toggleReflections();
                planeMaterial->setReflectionEnabled(rendererManager->isReflectionsEnabled());
                if (rendererManager->isReflectionsEnabled()) {
                    planeMaterialDirLight->setAmbient({0.7f, 0.7f, 0.7f});
                } else {
                    planeMaterialDirLight->setAmbient({0.07f, 0.07f, 0.07f});
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

    void MainScene::initSkybox() {
        const auto skybox = make_shared<Cube>();
        const auto skyboxRenderer = make_shared<SkyboxRenderer>(skybox, camera, projection, resourceManager);
        rendererManager->addRenderer(skyboxRenderer, 1000);
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

        planeMaterialDirLight = make_shared<DirectionalLight>();
        planeMaterialDirLight->setPosition({0.0f, 7.0f, 11.0f});
        planeMaterialDirLight->setDirection({1, 1.0, -3});
        planeMaterialDirLight->setAmbient({0.07f, 0.07f, 0.07f});
        planeMaterialDirLight->setDiffuse({0.0f, 0.0f, 0.0f});
        planeMaterialDirLight->setSpecular({.091f, .091f, .091f});

        planeMaterial->setDirectionalLight(planeMaterialDirLight);
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
        planarReflectionRenderer->setPlaneZ(-1.0f);

        addMeshNode3D(node3d, 101);
    }

    void MainScene::initPlayerScene() {
        playerScene = make_shared<PlayerScene>(rendererManager, camera, projection, resourceManager, width, height);
        playerScene->init(0);
        snakeMoveHandler = playerScene->getSnakeMoveHandler();
        snakeMoveHandler->setCollisionDetector(collisionDetector);
        addNode(playerScene);
    }

    void MainScene::initBarriersScene() {
        barriersScene = make_shared<BarriersScene>(rendererManager, camera, projection, resourceManager, width, height);
        barriersScene->init(1);
        levelManager = barriersScene->getLevelManager();
        addNode(barriersScene);
    }

    void MainScene::initCoinScene() {
        coinScene = make_shared<CoinScene>(rendererManager, camera, projection, resourceManager, width, height);
        coinScene->init(1);
        addNode(coinScene);

        collisionDetector->addStaticItem(coinScene->getCoin());
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
        radarMeshNode = make_shared<RadarMeshNode2D>(radarNode, resourceManager);
        radarMeshNode->setPosition({width - 240 + 100, 30.0 + 110, 0.0}); // + 100 kvuli tomu, ze stred neni 0,0 ale stred quadu
        radarMeshNode->addItem(playerScene->getSnake(), glm::vec3(0.0,1.0,0.0), "snake");
        radarMeshNode->addItem(coinScene->getCoin(), glm::vec3(1.0,1.0,0.0), "coin");
        radarMeshNode->addItem(barriersScene->getLevelBoxes(), glm::vec3(1.0,0.0,0.0), "barriers");
        radarMeshNode->hideItems();

        // meshNode2d.push_back(radarMeshNode);
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

        // meshNode2d.push_back(helpText);
        // meshNode2d.push_back(tilesCounterNode);
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
                coinScene->getCoin()->setVisible(false);
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
        if (planarReflectionRenderer) {
            planarReflectionRenderer->update(getAllMeshNodes3D());
            planarReflectionRenderer->updateRenderers(rendererManager->getRenderers());
        }
        Scene::update();

        if (radarMeshNode->isVisible() && radarFadeOutUniform->getAlpha() <= 0) {
            radarMeshNode->setVisible(false);
        }

        if (!helpText->isVisible()) {
            eatManager->run(EatManager::checkPlace);
        }
    }
} // Scenes
