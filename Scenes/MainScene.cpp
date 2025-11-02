#include "MainScene.h"
#include "PlayerScene.h"
#include "../Renderer/Opengl/AnimRenderer.h"
#include "../Renderer/Opengl/BarrierRenderer.h"
#include "../Renderer/Opengl/RadarRenderer.h"
#include "../Renderer/Opengl/SkyboxRenderer.h"
#include "../Renderer/Opengl/SnakeRenderer.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"

namespace Scenes {
    MainScene::MainScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    }

    void MainScene::init() {
        Scene::init();
        initPlayerScene();
        initSkybox();
        initPlane();
        initSnake();
        initBarriers();
        initEat();
        initRadar();
        initEatManager();
        initLevelManager();

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
        auto planeShader = resourceManager->getShader("shadowShader");
        auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
        auto shadowMap = resourceManager->getTexture("depth");
        auto gamefieldAlbedo = resourceManager->getTexture("tile.png");
        auto gamefieldNormal = resourceManager->getTexture("gamefield_normal.jpg");
        auto gamefieldSpecular = resourceManager->getTexture("gamefield_specular.jpg");
        const auto planeMaterial = make_shared<StandardMaterial>(basicShader, shadowDepthShader);
        const auto shaderMaterial = make_shared<ShaderMaterial>(planeShader, shadowDepthShader);

        const auto albedo = make_shared<TextureUniform>(0, gamefieldAlbedo);
        const auto normalMap = make_shared<TextureUniform>(2, gamefieldNormal);
        const auto specularMap = make_shared<TextureUniform>(3, gamefieldSpecular);
        const auto shadow = make_shared<TextureArrayUniform>(4, shadowMap);
        shaderMaterial->addUniform("diffuseMap", albedo);
        shaderMaterial->addUniform("normalMap", normalMap);
        shaderMaterial->addUniform("specularMap", specularMap);
        shaderMaterial->addUniform("shadowMap", shadow);
        shaderMaterial->addUniform("material.diffuse", 0);
        shaderMaterial->addUniform("shadowsEnable", true);

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

        const auto standardBaseItem = make_shared<BaseItem>();
        auto planeMesh = make_shared<PlaneMesh>(standardBaseItem, basicShader, 4, 4);
        planeMesh->setMaterial(planeMaterial);
        const auto node3d = make_shared<MeshNode3D>(shared_ptr<StandardMesh>(std::move(planeMesh)), resourceManager);
        node3d->setRotationX(90);
        node3d->setPosition({1.0, 1.0, -1.0});

        meshes.push_back(node3d);
    }

    void MainScene::initSnake() { // TODO: jakmile predelam renderery, kde se pracuje se Snake, tak toto smazat + celou tridu Snake
        snake = make_shared<Snake>();
        snake->init();
        //snake->getHeadTile()->setVisible(false);
        // snakeRenderer = make_shared<SnakeRenderer>(snake, camera.get(), projection, resourceManager.get());
        //
        // auto headTile = *snake->getItems().begin();
        // const auto animHead = resourceManager->getAnimationModel("pacman");
        // animHead->setBaseItem(snake->getHeadTile());

        // const auto animRenderer = make_shared<AnimRenderer>(headTile, animHead, camera.get(), projection, resourceManager.get());
        // animRenderer->addPlay("KostraAction");
        // animRenderer->setAcceleration(2.2f);

        // rendererManager->addRenderer(animRenderer);
        //rendererManager->addRenderer(snakeRenderer);
        camera->setStickyPoint(snake->getHeadTile().get());
    }

    void MainScene::initBarriers() {
        const auto geometry = make_shared<BaseItem>(BaseItem());
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto boxMesh = make_shared<BoxMesh>(geometry, shader, 2.0, 2.0, 2.0);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 110.0f});
        directionalLight->setDirection({0, 1.0, -3});
        directionalLight->setAmbient({0.6f, 0.6f, 0.6f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.001f, .001f, .001f});

        const auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
        const auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
        const auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");
        const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        boxMaterial->setShadow(resourceManager->getTexture("depth"));
        boxMaterial->setNormalEnabled(true);
        boxMaterial->setAlbedo(brickWall);
        boxMaterial->setNormal(brickWallNormal);
        boxMaterial->setSpecular(brickWallSpecular);
        boxMaterial->setDirectionalLight(directionalLight);

        boxMesh->setMaterial(boxMaterial);
        geometry->setScale({0.041666667f, 0.041666667f, 0.041666667f});

        const auto boxNode3D = make_shared<MeshNode3D>(boxMesh, resourceManager);
        boxNode3D->setPosition({-25.0, -25.0, -23.0});

        for (int x = 2; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition({x, 0.0, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int x = 0; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition({x, 98.0, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition({0, y, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition({98, y, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        meshes.push_back(boxNode3D);
    }

    void MainScene::initLevelManager() {
        levelManager = make_unique<LevelManager>(1, MAX_LIVES, resourceManager);
        levelManager->createLevel(START_LEVEL);
        levelBoxes = levelManager->createLevel(START_LEVEL);
        meshes.push_back(levelBoxes);
    }

    void MainScene::initEat() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const std::shared_ptr<ObjItem> coinModel(
            resourceManager->getModel("coin"), [](ObjItem *) {
        });
        const auto geometry = make_shared<BaseItem>(BaseItem());
        const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(geometry, shader));
        coinMesh->fromObj(coinModel);

        coinMeshNode3D = make_shared<CoinMeshNode3D>(coinMesh, resourceManager);
        coinMeshNode3D->setPosition({-69.0, -69, -70.0f});
        coinMeshNode3D->setScale({0.013888889, 0.013888889, 0.013888889});
        geometry->setRotationX(90);
        geometry->setVisible(false);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto shadowMap = resourceManager->getTexture("depth");
        const auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
        const auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
        const auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
        auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");
        const auto coinMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        coinMaterial->setAlbedo(coinAlbedo);
        coinMaterial->setNormal(coinNormal);
        coinMaterial->setSpecular(coinMetalness);
        coinMaterial->setShadow(shadowMap);
        coinMaterial->setNormalEnabled(true);
        coinMaterial->setDirectionalLight(directionalLight);

        coinMesh->setMaterial(coinMaterial);

        collisionDetector->addStaticItem(coinMeshNode3D);

        meshes.push_back(coinMeshNode3D);
    }

    void MainScene::initEatManager() {
        auto eatLocationHandler = make_shared<EatLocationHandler>(nullptr, playerScene->getSnake(), coinMeshNode3D, radar);
        eatManager = make_unique<EatManager>(eatLocationHandler);
    }

    void MainScene::initRadar() {
        radar = make_shared<Radar>();
        resetRadar();

        const auto radarRenderer = make_shared<RadarRenderer>(radar, camera, resourceManager, ortho);
        rendererManager->addRenderer(radarRenderer);
    }

    void MainScene::resetRadar() const {
        radar->reset();
        radar->setVisible(true);
        radar->setPosition({1.25, 1.4, 0.0});
        radar->setScale({100, 100, 1});
        radar->setWidth(176);
        radar->setHeight(176);

        if (resourceManager) {
            for (const auto& tile: snake->getItems()) {
                radar->addItem(tile->tile, {0.278,1.,0.});
            }
            // for (const auto& block: barriers->getItems()) {
            //     radar->addItem(block, {0.694,0.078,0.016});
            // }
            // radar->addItem(eat, {1.,0.953,0.});
        }
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
                    //     this->snake->reset();
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
                std::string buffAsStdStr = buff;
                // this->tilesCounterText->setText(buffAsStdStr);
            }
        });
    }

    void MainScene::buildStartMoveCallback() const {
        snakeMoveHandler->addStartMoveCallback([this]() {
            if (this->levelManager) {
                this->eatManager->run(Manager::EatManager::firstPlace);
                // this->startText->fadeOut();
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
                // this->tilesCounterText->setText(buffAsStdStr);
                // if (this->tilesCounterText->getAlpha() == 1.0f) {
                //     this->tilesCounterText->setAlpha(0.0f);
                //     this->tilesCounterText->fadeIn();
                // }
            }
        });
    }

    void MainScene::buildCrashCallback() const {
        snakeMoveHandler->setCrashCallback([this]() {
            if (this->levelManager) {
                snake->reset();
                resetRadar();
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
                std::string buffAsStdStr = buff;
                // this->tilesCounterText->setText(buffAsStdStr);
                coinMeshNode3D->getBaseItem()->setVisible(false);
                if (this->levelManager->getLive() == 0) {
                    // Game Over
                    this->levelManager->createLevel(1);
                    // this->startText->setVisible(true);
                    this->levelManager->setLive(3);
                    cout << "crash callback call" << endl;
                }
            }
        });
    }
} // Scenes
