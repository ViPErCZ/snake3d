#include "MainScene.h"
#include "PlayerScene.h"
#include "../Renderer/Opengl/AnimRenderer.h"
#include "../Renderer/Opengl/BarrierRenderer.h"
#include "../Renderer/Opengl/EatRenderer.h"
#include "../Renderer/Opengl/RadarRenderer.h"
#include "../Renderer/Opengl/SkyboxRenderer.h"
#include "../Renderer/Opengl/SnakeRenderer.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "../Renderer/Opengl/Model/Standard/PlaneMesh.h"

namespace Scenes {
    MainScene::MainScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    }

    void MainScene::init() {
        Scene::init();
        initSkybox();
        initPlane();
        initSnake();
        initBarriers();
        initEat();
        initRadar();
        initEatManager();
        initLevelManager();
        initSnakeMoveHandler();

        // TODO: doplnit width, height
        const auto playerScene = make_shared<PlayerScene>(rendererManager, camera, projection, resourceManager, 0, 0);
        playerScene->init();
        addNode(playerScene);
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
        // planeShader.get()->printActiveUniforms();

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
        standardBaseItem->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
        standardBaseItem->setPosition(glm::vec3(1.0, 1.0, -1.0));
        auto planeMesh = make_shared<PlaneMesh>(standardBaseItem, basicShader, 4, 4);
        planeMesh->setMaterial(planeMaterial);
        const auto node3d = make_shared<MeshNode3D>(shared_ptr<StandardMesh>(std::move(planeMesh)), resourceManager);

        meshes.push_back(node3d);
    }

    void MainScene::initSnake() {
        snake = make_shared<Snake>();
        snake->init();
        //snake->getHeadTile()->setVisible(false);
        snakeRenderer = make_shared<SnakeRenderer>(snake, camera.get(), projection, resourceManager.get());

        auto headTile = *snake->getItems().begin();
        const auto animHead = resourceManager->getAnimationModel("pacman");
        animHead->setBaseItem(snake->getHeadTile());

        const auto animRenderer = make_shared<AnimRenderer>(headTile, animHead, camera.get(), projection, resourceManager.get());
        animRenderer->addPlay("KostraAction");
        animRenderer->setAcceleration(2.2f);

        // rendererManager->addRenderer(animRenderer);
        //rendererManager->addRenderer(snakeRenderer);
        camera->setStickyPoint(snake->getHeadTile().get());
    }

    void MainScene::initSnakeMoveHandler() {
        auto animHead = resourceManager->getAnimationModel("pacman");
        snakeMoveHandler = make_shared<SnakeMoveHandler>(snake, animHead);
        collisionDetector = make_shared<CollisionDetector>();
        // collisionDetector->setPerimeter(objWall.get());
        // collisionDetector->setBarriers(barriers.get());
        collisionDetector->addStaticItem(eat);
        snakeMoveHandler->setCollisionDetector(collisionDetector);

        buildStartMoveCallback(animHead);
        buildEatenUpCallback();
        buildCrashCallback();

        keyboardManager->addEventHandler(snakeMoveHandler);
    }

    void MainScene::initBarriers() {
        barriers = make_shared<Barriers>();
        const auto barrierRenderer = make_shared<BarrierRenderer>(snake, barriers, camera.get(), projection, resourceManager.get());
        rendererManager->addRenderer(barrierRenderer);

        objWall = make_shared<ObjWall>();
        objWall->init();
        const auto objWallRenderer = make_shared<ObjWallRenderer>(snake, objWall, camera.get(), projection, resourceManager.get());
        rendererManager->addRenderer(objWallRenderer);
    }

    void MainScene::initLevelManager() {
        levelManager = make_unique<LevelManager>(1, MAX_LIVES, barriers);
        levelManager->createLevel(START_LEVEL);
    }

    void MainScene::initEat() {
        eat = make_shared<Eat>();
        eat->setVirtualX((23 - -23) / 2 * 32 + 16);
        eat->setVirtualY((-3 - -23) / 2 * 32 + 16);
        eat->setPosition({-69.0, -69, -70.0f});
        eat->setZoom({0.013888889, 0.013888889, 0.013888889});
        eat->setRotate({1, 0, 0, 90}, {0, 1, 0, 0}, {0, 0, 1, 0});
        eat->setVisible(false);
        const auto eatRenderer = make_shared<EatRenderer>(eat.get(), camera.get(), projection, resourceManager.get());

        rendererManager->addRenderer(eatRenderer, 10);
    }

    void MainScene::initEatManager() {
        auto eatLocationHandler = make_shared<EatLocationHandler>(barriers, snake, eat, radar);
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
        radar->setZoom({100, 100, 1});
        radar->setWidth(176);
        radar->setHeight(176);

        if (resourceManager) {
            for (const auto& tile: snake->getItems()) {
                radar->addItem(tile->tile, {0.278,1.,0.});
            }
            for (const auto& block: barriers->getItems()) {
                radar->addItem(block, {0.694,0.078,0.016});
            }
            radar->addItem(eat, {1.,0.953,0.});
        }
    }

    void MainScene::buildEatenUpCallback() const {
        snakeMoveHandler->setEatenUpCallback([this]() {
            if (this->levelManager && this->snake) {
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

    void MainScene::buildStartMoveCallback(shared_ptr<AnimationModel> &animHead) const {
        snakeMoveHandler->setStartMoveCallback([this, animHead]() {
            if (this->levelManager) {
                animHead->setGlobalPause(false);
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
                eat->setVisible(false);
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
