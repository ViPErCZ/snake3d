#include <AL/alc.h>
#include <AL/alut.h>
#include "App.h"
#include "Handler/Debug/PositionHandler.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/StandardMeshRenderer.h"
#include "Renderer/Opengl/TorchRenderer.h"
#include "Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "Renderer/Opengl/Model/SpinnerModel.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "Renderer/Opengl/Model/Standard/CapsuleMesh.h"
#include "Resource/AnimLoader.h"
#include "Resource/ShaderLoader.h"
#include "Resource/TextureLoader.h"

App::App(const shared_ptr<Camera> &camera, const int width, const int height) : camera(camera), width(width), height(height) {
    resourceManager = make_shared<ResourceManager>();
    keyboardManager = make_unique<KeyboardManager>();
    startText = new Text("Press start I, K or L...");
    tilesCounterText = new Text("");
    eat = new Eat;
    torchRenderer = nullptr;

    projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );

    rendererManager = make_shared<RenderManager>(camera, resourceManager, projection, width, height);
    rendererManager->setWidth(width);
    rendererManager->setHeight(height);
    mainScene = make_unique<MainScene>(rendererManager, camera, projection, resourceManager, width, height);
}

App::~App() {
    delete eat;
    alDeleteSources(1, &musicSource);
    alDeleteSources(1, &coinSource);
    alDeleteBuffers(1, &musicBuffer);
    alDeleteBuffers(1, &coinBuffer);
}

void App::initScene() {
    mainScene->init();
    //const glm::mat4 ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);

    // InitSnake();
    // animRenderer = make_shared<AnimRenderer>((*snake->getItems().begin()), resourceManager->getAnimationModel("pacman"), camera.get(), projection, resourceManager.get());
    // animRenderer->addPlay("KostraAction");
    // animRenderer->setAcceleration(2.2f);
    // snake->getHeadTile()->setVisible(false);
    //    animRenderer->addPlay("Armature|Take 001|BaseLayer");
    //    animRenderer->addPlay("Kostra2Action.002");
    //    animRenderer->addPlay("Kostra3Action");
    // bloomRenderer = new BloomRenderer(resourceManager.get(), width, height);
    // depthMapRenderer = new DepthMapRenderer(camera.get(), projection, resourceManager.get());
    //gameFieldRenderer = new GameFieldRenderer(InitGameField(), camera.get(), projection, resourceManager.get());
    //eat = InitEat();
    // ObjWall *objWall = InitObjWall();
    //barriers = new Barriers();
    //radar = CreateRadar();
    //InitRadar();

    // TORCH
    // ===============================
    // const auto torch = new Cube();
    // torch->setPosition(glm::vec3(0.33, 0.3, -8.2));
    // torch->setRotate(
    //     glm::vec4(1.0, 0.0, 0.0, 90.0f),
    //     glm::vec4(0.0, 1.0, 0.0, 0.0f),
    //     glm::vec4(0.0, 0.0, 1.0, 0.0f));
    // torch->setZoom({0.12, 0.12, 0.12});

    //levelManager = make_unique<LevelManager>(1, MAX_LIVES, barriers);
    //levelManager->createLevel(START_LEVEL);

    //auto *eatLocationHandler = new EatLocationHandler(barriers, snake, eat, radar);
    //eatManager = make_unique<EatManager>(eatLocationHandler);

    auto basicShader = resourceManager->getShader("basicShader");
    auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
    const auto planeMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    const auto coinMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));

    auto gamefieldAlbedo = resourceManager->getTexture("gamefield.bmp");
    auto gamefieldNormal = resourceManager->getTexture("gamefield_normal.jpg");
    auto gamefieldSpecular = resourceManager->getTexture("gamefield_specular.jpg");
    auto skeletonAlbedo = resourceManager->getTexture("Skeleton_Body.png");
    auto skeletonORM = resourceManager->getTexture("Skeleton_Body_ORM.png");
    auto shadowMap = resourceManager->getTexture("depth");
    auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
    auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
    auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
    auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");
    auto rustedAlbedo = resourceManager->getTexture("rusted_albedo.png");
    auto rustedNormal = resourceManager->getTexture("rusted_normal.png");
    auto rustedRoughness = resourceManager->getTexture("rusted_roughness.png");
    auto aoMap = resourceManager->getTexture("ao.png");
    auto rustedMetallic = resourceManager->getTexture("rusted_metallic.png");
    auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
    auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
    auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");
    auto environmentMap = resourceManager->getTexture("skybox");

    const auto directionalLight = make_shared<DirectionalLight>();
    directionalLight->setPosition({0.0f, 7.0f, 11.0f});
    directionalLight->setDirection({1, 1.0, -3});
    directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
    directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
    directionalLight->setSpecular({.091f, .091f, .091f});

    const auto spotLight = make_shared<SpotLight>();
    spotLight->setPosition({0.0f, 0.5f, 0.2f});
    spotLight->setDirection({0.0f, 0.0f, 0.0f});
    spotLight->setAmbient({1.0f, 1.0f, 1.0f});
    spotLight->setDiffuse({0.0f, 0.0f, 0.0f});
    spotLight->setSpecular({1.0f, 1.0f, 1.0f});
    spotLight->setCutOff(12.5);
    spotLight->setOuterCutOff(17.5);

    const auto pointLight = make_shared<PointLight>();
    pointLight->setPosition({-0.9f, 1.1f, 0.2f});
    pointLight->setAmbient(glm::vec3(0.6f));
    // pointLight->setDiffuse({0.198f, 0.459f, 0.94f});
    pointLight->setDiffuse(glm::vec3(0.0f));
    pointLight->setSpecular(glm::vec3(0.0f));
    pointLight->setConstant(0.0005f);
    pointLight->setLinear(0.8f);

    planeMaterial->setColor({0.88, 0.05, 0.05});
    // planeMaterial->setColor({1, 1, 1});
    //planeMaterial->setAlbedo(rustedAlbedo);
    //planeMaterial->setNormal(rustedNormal);
    // planeMaterial->setRoughness(rustedRoughness);
    // planeMaterial->setMetalness(rustedMetallic);
    //planeMaterial->setAoMap(aoMap);
    // planeMaterial->setAlbedo(skeletonAlbedo);
    // planeMaterial->setMetalness(skeletonORM);
    // planeMaterial->setRoughness(skeletonORM);
    // planeMaterial->setAoMap(skeletonORM);
    boxMaterial->setAlbedo(brickWall);
    boxMaterial->setNormal(brickWallNormal);
    boxMaterial->setSpecular(brickWallSpecular);
    // planeMaterial->setAlbedo(gamefieldAlbedo);
    coinMaterial->setAlbedo(coinAlbedo);
    coinMaterial->setNormal(coinNormal);
    coinMaterial->setSpecular(coinMetalness);
    // coinMaterial->setAoMap(aoMap);
    // planeMaterial->setAlbedo(brickWall);
    // planeMaterial->setNormal(brickWallNormal);
    // planeMaterial->setMetalness(coinMetalness);
    // planeMaterial->setRoughness(coinRoughness);
    // planeMaterial->setNormal(gamefieldNormal);
    planeMaterial->setShadow(shadowMap);
    coinMaterial->setShadow(shadowMap);
    boxMaterial->setShadow(shadowMap);
    // planeMaterial->setNormalEnabled(true);
    coinMaterial->setNormalEnabled(true);
    boxMaterial->setNormalEnabled(true);
    // planeMaterial->setSpecular(gamefieldSpecular);
    planeMaterial->setDirectionalLight(directionalLight);
    coinMaterial->setDirectionalLight(directionalLight);
    boxMaterial->setDirectionalLight(directionalLight);
    // planeMaterial->addSpotLight(spotLight);
    // planeMaterial->addPointLight(pointLight);
    // coinMaterial->addPointLight(pointLight);
    // boxMaterial->addPointLight(pointLight);
    // boxMaterial->addPointLight(pointLight);
    // planeMaterial->setShadow(true);
    // coinMaterial->setShadow(true);
    // boxMaterial->setShadow(true);
    // planeMaterial->setEnvironmentMap(environmentMap);
    // coinMaterial->setEnvironmentMap(environmentMap);

    // const std::shared_ptr<AnimationModel> pacmanModel(
    //     resourceManager->getAnimationModel("pacman"), [](AnimationModel *) {
    //     });
    // const std::shared_ptr<AnimationModel> skeletonModel(
    //     resourceManager->getAnimationModel("skeleton"), [](AnimationModel *) {
    //     });

    const auto standardBaseItem = make_shared<BaseItem>(BaseItem());
    const auto standardBaseItem2 = make_shared<BaseItem>(BaseItem());
    const auto standardBaseItem3 = make_shared<BaseItem>(BaseItem());
    const auto standardPlaneMesh = make_shared<PlaneMesh>(PlaneMesh(standardBaseItem, basicShader, 1, 1));
    const auto standardBoxMesh = make_shared<BoxMesh>(BoxMesh(standardBaseItem3, basicShader, 1, 1, 1));
    const auto sphereMesh = make_shared<SphereMesh>(SphereMesh(standardBaseItem, basicShader));
    const auto capsuleMesh = make_shared<CapsuleMesh>(CapsuleMesh(standardBaseItem, basicShader));
    const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(standardBaseItem2, basicShader));
    // const auto pacmanMesh = make_shared<AnimationArrayMesh>(AnimationArrayMesh(pacmanModel, basicShader));
    // const auto skeletonMesh = make_shared<AnimationArrayMesh>(AnimationArrayMesh(skeletonModel, basicShader));

    const std::shared_ptr<ObjItem> coinObjItem(
        resourceManager->getModel("coin"), [](ObjItem *) {
        });
    coinMesh->fromObj(coinObjItem);
    standardPlaneMesh->setMaterial(planeMaterial);
    standardBoxMesh->setMaterial(boxMaterial);
    sphereMesh->setMaterial(planeMaterial);
    capsuleMesh->setMaterial(planeMaterial);
    coinMesh->setMaterial(coinMaterial);
    // pacmanMesh->setMaterial(planeMaterial);
    // skeletonMesh->setMaterial(planeMaterial);
    //sphereMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    //coinMesh->getBaseItem()->setPosition({5, 0, 0});
    //coinMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    // pacmanMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    // skeletonMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    sphereMesh->getBaseItem()->setScale({0.2, 0.2, 0.2});
    coinMesh->getBaseItem()->setScale({0.2, 0.2, 0.2});
    // pacmanMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    standardBoxMesh->getBaseItem()->setScale({0.2, 0.2, 0.2});
    // skeletonMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    // standardBaseItem->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    // const auto standardRenderer = new StandardMeshRenderer(camera, projection, sphereMesh);
    // const auto standardRenderer2 = new StandardMeshRenderer(camera, projection, coinMesh);
    // const auto standardRenderer3 = new StandardMeshRenderer(camera, projection, standardBoxMesh);
    // const auto standardRenderer4 = new StandardMeshRenderer(camera, projection, sphereMesh);

    // snakeRenderer = make_shared<SnakeRenderer>(snake, camera.get(), projection, resourceManager.get());
    // objWallRenderer = make_shared<ObjWallRenderer>(snake, objWall, camera.get(), projection, resourceManager.get());
    // barrierRenderer = make_shared<BarrierRenderer>(snake, barriers, camera.get(), projection, resourceManager.get());
    // eatRenderer = make_shared<EatRenderer>(eat, camera.get(), projection, resourceManager.get());
    // radarRenderer = make_shared<RadarRenderer>(radar, camera.get(), ortho, resourceManager.get());
    // textRenderer = make_shared<TextRenderer>(width, height);
    // rainRenderer = make_shared<RainRenderer>(new BaseItem(), camera.get(), projection, resourceManager.get());
    // rainDropRenderer = make_shared<RainDropRenderer>(new BaseItem(), camera.get(), projection, resourceManager.get());
    // fireRenderer = make_shared<FireRenderer>(camera.get(), projection, resourceManager.get());
    // torchRenderer = make_shared<TorchRenderer>(torch, camera.get(), projection, resourceManager.get());
    // boltRenderer = make_shared<BoltRenderer>(camera.get(), projection, resourceManager.get());
    // const auto storm = new BaseItem();
    // storm->setVisible(false);

    initTexts();

    animateEat = new Eat;
    animateEat->setVisible(false);
    animateEat->setPosition(eat->getPosition());

    eatRemoveAnimateRenderer = make_shared<EatRemoveAnimateRenderer>(animateEat, camera.get(), projection, resourceManager.get());

    // shared_ptr<PlaneMesh> planeMesh = initPlane();

    // rendererManager->setWidth(width);
    // rendererManager->setHeight(height);
     //rendererManager->addRenderer(new StandardMeshRenderer(camera, projection, planeMesh));
     // rendererManager->addRenderer(standardRenderer4);
     //rendererManager->addRenderer(standardRenderer2);
     //rendererManager->addRenderer(standardRenderer3);
     // rendererManager->addRenderer(gameFieldRenderer);
     // rendererManager->addRenderer(eatRenderer);
     // rendererManager->addRenderer(eatRemoveAnimateRenderer);
     // rendererManager->addRenderer(animRenderer);
     // rendererManager->addRenderer(snakeRenderer);
     // rendererManager->addRenderer(rainDropRenderer);
     //rendererManager->addRenderer(objWallRenderer);
     //rendererManager->addRenderer(barrierRenderer);
     // rendererManager->addRenderer(radarRenderer);
     // rendererManager->addRenderer(rainRenderer);
     // rendererManager->addRenderer(torchRenderer);
     // rendererManager->addRenderer(fireRenderer);
     // rendererManager->addRenderer(boltRenderer);
     // rendererManager->addRenderer(textRenderer);
     // rendererManager->setDepthMapRenderer(depthMapRenderer);
     // rendererManager->setBloomRenderer(bloomRenderer);
    // rendererManager->enableShadows();

     //auto animHead = resourceManager->getAnimationModel("pacman");
     // animHead->setBaseItem(snake->getHeadTile());
     // auto *snakeMoveHandler = new SnakeMoveHandler(snake, animHead);
     // auto *radarHandler = new RadarHandler(radar);
     //
     // collisionDetector = make_shared<CollisionDetector>();
     // collisionDetector->setPerimeter(objWall);
     // collisionDetector->setBarriers(barriers);
     // collisionDetector->addStaticItem(eat);
     // snakeMoveHandler->setCollisionDetector(collisionDetector.get());
     // snakeMoveHandler->setStartMoveCallback([this, animHead]() {
     //     if (this->levelManager) {
     //         animHead->setGlobalPause(false);
     //         this->eatManager->run(Manager::EatManager::firstPlace);
     //         this->startText->fadeOut();
     //         char buff[100];
     //         snprintf(buff, sizeof(buff),
     //                  "%s %d, %s %d, %s %d",
     //                  "Level:",
     //                  this->levelManager->getLevel(),
     //                  "Lives:",
     //                  this->levelManager->getLive(),
     //                  "Points left:",
     //                  MAX_POINT - this->levelManager->getEatCounter()
     //         );
     //         const std::string buffAsStdStr = buff;
     //         this->tilesCounterText->setText(buffAsStdStr);
     //         if (this->tilesCounterText->getAlpha() == 1.0f) {
     //             this->tilesCounterText->setAlpha(0.0f);
     //             this->tilesCounterText->fadeIn();
     //         }
     //     }
     // });
     // snakeMoveHandler->setCrashCallback([this]() {
     //     if (this->levelManager && this->barrierRenderer) {
     //         snake->reset();
     //         InitRadar();
     //         this->levelManager->setLive(this->levelManager->getLive() - 1);
     //         this->levelManager->setEatCounter(0);
     //         char buff[100];
     //         snprintf(buff, sizeof(buff),
     //                  "%s %d, %s %d, %s %d",
     //                  "Level:",
     //                  this->levelManager->getLevel(),
     //                  "Lives:",
     //                  this->levelManager->getLive(),
     //                  "Points left:",
     //                  MAX_POINT - this->levelManager->getEatCounter()
     //         );
     //         std::string buffAsStdStr = buff;
     //         this->tilesCounterText->setText(buffAsStdStr);
     //         eat->setVisible(false);
     //         if (this->levelManager->getLive() == 0) { // Game Over
     //             this->levelManager->createLevel(1);
     //             this->startText->setVisible(true);
     //             this->levelManager->setLive(3);
     //             cout << "crash callback call" << endl;
     //         }
     //     }
     // });
     // snakeMoveHandler->setEatenUpCallback([this]() {
     //     if (this->levelManager && this->snake && this->barrierRenderer) {
     //         alSourcePlay (coinSource);
     //
     //         if (const ALCenum error = alGetError(); error != AL_NO_ERROR) {
     //             cout << "Sound error" << endl;
     //         }
     //
     //         if (this->eatRemoveAnimateRenderer && this->animateEat) {
     //             this->animateEat->setPosition(eat->getPosition());
     //             this->animateEat->setVisible(true);
     //             this->animateEat->fadeOut();
     //         }
     //
     //         this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);
     //
     //         if (this->levelManager->getEatCounter() == MAX_POINT) {
     //             this->startText->setVisible(true);
     //             this->snake->reset();
     //             this->eat->setVisible(false);
     //             this->levelManager->createLevel(this->levelManager->getLevel() + 1);
     //             this->eatManager->run(Manager::EatManager::clean);
     //             InitRadar();
     //         } else {
     //             this->eatManager->run(Manager::EatManager::eatenUp);
     //         }
     //
     //         char buff[100];
     //         snprintf(buff, sizeof(buff),
     //                  "%s %d, %s %d, %s %d",
     //                  "Level:",
     //                  this->levelManager->getLevel(),
     //                  "Lives:",
     //                  this->levelManager->getLive(),
     //                  "Points left:",
     //                  MAX_POINT - this->levelManager->getEatCounter()
     //         );
     //         std::string buffAsStdStr = buff;
     //         this->tilesCounterText->setText(buffAsStdStr);
     //
     //     }
     // });
     // const auto positionHandler = new PositionHandler(camera);
     // positionHandler->addItem(pointLight.get());
     // positionHandler->addItem(eat);
     // positionHandler->addItem(directionalLight.get());
     // positionHandler->addItem(standardBaseItem2.get());
     // positionHandler->addItem(standardBaseItem.get());
     // positionHandler->addItem(standardBoxMesh->getBaseItem().get());
     // positionHandler->addItem(pacmanMesh->getBaseItem().get());
     // positionHandler->addItem(skeletonMesh->getBaseItem().get());
     // positionHandler->addItem(planeMesh->getBaseItem().get());
     // positionHandler->addItem(sphereMesh->getBaseItem().get());
     // positionHandler->addItem(torch);

     // keyboardManager->addEventHandler(snakeMoveHandler);
     // keyboardManager->addEventHandler(radarHandler);
     // keyboardManager->addEventHandler(positionHandler);

     musicBuffer = alutCreateBufferFromFile("Assets/Sounds/snake.wav");
     coinBuffer = alutCreateBufferFromFile("Assets/Sounds/coin.wav");
     alGenSources (1, &musicSource);
     alGenSources (1, &coinSource);
     alSourcei (musicSource, AL_BUFFER, static_cast<ALint>(musicBuffer));
     alSourcei (coinSource, AL_BUFFER, static_cast<ALint>(coinBuffer));
     alSourcei (musicSource, AL_LOOPING, true);
     //alSourcePlay (musicSource);
     ALCenum error;

     error = alGetError();
     if (error != AL_NO_ERROR) {
         cout << "Sound error" << endl;
     }
}

void App::Init() const {
    InitResourceManager();

    resourceManager->addShader(
        "bloom",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/bloom.vs",
                "Assets/Shaders/bloom/bloom.fs"
                ))
    );

    resourceManager->addShader("blur",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/blur.vs",
                "Assets/Shaders/bloom/blur.fs"
                ))
    );

    resourceManager->addShader(
        "bloomFinal",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/bloom_final.vs",
                "Assets/Shaders/bloom/bloom_final.fs"
                ))
    );

    resourceManager->addShader(
        "shadowShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map.vs",
                "Assets/Shaders/shadow_map.fs"
            ))
    );
    resourceManager->addShader(
        "shadowDepthShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map_depth.vs",
                "Assets/Shaders/shadow_map_depth.fs"
            ))
    );
    resourceManager->addShader(
        "basicShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/basic.vs",
                "Assets/Shaders/basic.fs"
            ))
    );
    resourceManager->addShader(
        "preloadShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/preloader/preloader.vs",
                "Assets/Shaders/preloader/preloader.fs"
            ))
    );

    rendererManager->initBloom();
    rendererManager->initShadowMapping();

    const auto preLoader = initPreloader();
    rendererManager->addRenderer(make_shared<StandardMeshRenderer>(camera, projection, preLoader));
    camera->setStickyPoint(preLoader);

    const fs::path assets_dir{"Assets/Objects"};
    resourceManager->loadAsyncModel<AnimationModel>(assets_dir / "pacman.glb", "pacman", []() {
        std::cout << "Model pacman ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<AnimationModel>(assets_dir / "skeleton.glb", "skeleton", []() {
        std::cout << "Model skeleton ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<ObjItem>(assets_dir / "Cube.obj", "cube", []() {
        std::cout << "Model cube ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<ObjItem>(assets_dir / "Coin.obj", "coin", []() {
        std::cout << "Model coin ready!" << std::endl;
    });
    // resourceManager->loadAsyncModel<ObjItem>(assets_dir / "Tile.obj", "tile", []() {
    //     std::cout << "Model tile ready!" << std::endl;
    // });
    resourceManager->loadAsyncModel<ObjItem>(assets_dir / "torch.obj", "torch", []() {
        std::cout << "Model torch ready!" << std::endl;
    });
}

void App::run() {
    if (state == SceneState::LOADING) {
        resourceManager->processPending();

        if (!scanning && resourceManager->isAllLoaded()) {
            std::cout << "\rLoading DONE!      " << std::endl;
            state = SceneState::RUNNING;

            rendererManager->reset();
            initScene();
        }
    }

    static float lastFrame = 0.0f;
    const auto currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    deltaTime = std::min(deltaTime, 0.05f);

    if (state == SceneState::RUNNING) {
        //keyboardManager->runDefault();
        if (!startText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
            eatManager->run(Manager::EatManager::checkPlace);
        }
        mainScene->update();
        mainScene->render();
    } else {
        rendererManager->render(deltaTime);
    }
}

void App::processInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
    mainScene->keyboardInput(window, keyCode, scancode, action, mods);
    //keyboardManager->onKeyPress(keyCode, scancode, action, mods);

    switch (keyCode) {
        case GLFW_KEY_P:
            // if (objWallRenderer) {
            //     objWallRenderer->toggleParallax();
            // }
            break;
        case GLFW_KEY_F:
            rendererManager->toggleFog();
            break;
        case GLFW_KEY_W:
            if (rainRenderer && rainDropRenderer) {
                rainRenderer->toggle();
                rainDropRenderer->setEnable(rainRenderer->isEnable());
            }
            break;
        //        case GLFW_KEY_U:
        //            camera->startUpsideDownRotate();
        //            break;
        case GLFW_KEY_M:
            ALint source_state;
            alGetSourcei(musicSource, AL_SOURCE_STATE, &source_state);

            if (source_state == AL_PLAYING) {
                alSourceStop(musicSource);
            } else {
                alSourcePlay(musicSource);
            }
            break;
        // case GLFW_KEY_1: // show classic red head
            // snake->getHeadTile()->setVisible(true);
            // animRenderer->setShow(false);
            //snakeRenderer->toggleStyle(1);
            // break;
        // case GLFW_KEY_2: // show animated pacman head
            // snake->getHeadTile()->setVisible(false);
            // animRenderer->setShow(true);
            //snakeRenderer->toggleStyle(2);
            // break;
        case GLFW_KEY_T:
            if (boltRenderer) {
                boltRenderer->triggerBolt();
            }
            break;
        case GLFW_KEY_ESCAPE:
            if (state == SceneState::RUNNING) {
                glfwSetWindowShouldClose(window, true);
            }
        default:
            break;
    }
}

void App::mouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods) const {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 cursor(static_cast<float>(xpos), static_cast<float>(ypos));

    if (torchRenderer != nullptr) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                torchRenderer->onMouseDown(cursor, width, height);
            } else if (action == GLFW_RELEASE) {
                torchRenderer->onMouseUp();
            }
        }
    }
    if (camera) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            camera->onMouseDown(button, action, mods);
        }
    }
}

void App::mousePositionCallback(GLFWwindow *window, const double x, const double y) const {
    const glm::vec2 cursor(static_cast<float>(x), static_cast<float>(y));
    if (torchRenderer != nullptr) {
        torchRenderer->onMouseMove(cursor, width, height);
    }
    if (state == SceneState::RUNNING && camera != nullptr) {
        camera->processMouseMovement(x, y);
    }
}

void App::setKeyState(const int key, const bool pressed) const {
    camera->setKeyState(key, pressed);
}

void App::cameraProcessKeyboard(GLFWwindow *window) const {
    camera->processKeyboard(window, 1);
}

shared_ptr<MeshNode3D> App::initPreloader() const {
    auto shader = resourceManager->getShader("preloadShader");
    auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
    const auto standardBaseItem = make_shared<BaseItem>();

    return make_shared<MeshNode3D>(make_shared<SpinnerModel>(standardBaseItem, shader), resourceManager);
}

void App::InitResourceManager() const {
    vector<string> faces;
    faces.emplace_back("Assets/Skybox/cloud/right.jpg");
    faces.emplace_back("Assets/Skybox/cloud/left.jpg");
    faces.emplace_back("Assets/Skybox/cloud/top.jpg");
    faces.emplace_back("Assets/Skybox/cloud/bottom.jpg");
    faces.emplace_back("Assets/Skybox/cloud/front.jpg");
    faces.emplace_back("Assets/Skybox/cloud/back.jpg");
    const unsigned int cubeMapTexture = TextureLoader::loadSkyboxTexture(faces);
    const auto texture = std::make_shared<TextureManager>();
    texture->addTexture(cubeMapTexture);
    resourceManager->addTexture("skybox", texture);

    std::ifstream manifestFile("Assets/texture_manifest.json");
    if (!manifestFile.is_open()) {
        throw std::runtime_error("Nemohu otevřít manifest file.");
    }

    nlohmann::json j;
    manifestFile >> j;

    std::vector<TextureEntry> textures;

    for (auto &item : j) {
        textures.push_back(TextureEntry{
            item["name"].get<std::string>(),
            item["path"].get<std::string>(),
            item["category"].get<std::string>()
        });
    }

    for (const auto &[name, path, category] : textures) {
        bool isAlbedo = (category == "Albedo");

        resourceManager->loadAsyncTexture("Assets/Textures/" + path, name, isAlbedo, [name=name]() {
            std::cout << "Texture ready: " << name << std::endl;
        });
    }

    resourceManager->loadAsyncShader("textShader", "Assets/Shaders/text.vs", "", "Assets/Shaders/text.fs", []() {
        std::cout << "Shader textShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("gizmoShader",
                                       "Assets/Shaders/gizmo/gizmo.vs",
                                       "Assets/Shaders/gizmo/gizmo.geom",
                                       "Assets/Shaders/gizmo/gizmo.fs",
                                       []() {
        std::cout << "Shader gizmo ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("colorShader", "Assets/Shaders/color.vs", "", "Assets/Shaders/color.fs", []() {
        std::cout << "Shader color ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("respawnShader", "Assets/Shaders/basic.vs", "", "Assets/Shaders/respawn/respawn.fs", []() {;
        std::cout << "Shader respawnShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("normalShader", "Assets/Shaders/normal_map.vs", "", "Assets/Shaders/normal_map.fs", []() {
        std::cout << "Shader normalShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("radarShader", "Assets/Shaders/radar.vs", "", "Assets/Shaders/radar.fs", []() {;
        std::cout << "Shader radarShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("skyboxShader", "Assets/Shaders/skybox.vs", "", "Assets/Shaders/skybox.fs", []() {
        std::cout << "Shader skyboxShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("debugQuadShader", "Assets/Shaders/debug_quad.vs", "", "Assets/Shaders/debug_quad.fs", []() {
        std::cout << "Shader debugQuadShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("rain", "Assets/Shaders/rain/rain.vs", "", "Assets/Shaders/rain/rain.fs", []() {
        std::cout << "Shader rain ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("rainDrop", "Assets/Shaders/basic.vs", "", "Assets/Shaders/rain/raindrop.fs", []() {
        std::cout << "Shader rainDrop ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("fire", "Assets/Shaders/fire/fire.vs", "", "Assets/Shaders/fire/fire.fs", []() {
        std::cout << "Shader fire ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("smoke", "Assets/Shaders/fire/smoke.vs", "", "Assets/Shaders/fire/smoke.fs", []() {
        std::cout << "Shader smoke ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("boltShader", "Assets/Shaders/bolt/bolt.vs", "", "Assets/Shaders/bolt/bolt.fs", []() {
        std::cout << "Shader boltShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("flash", "Assets/Shaders/bolt/flash.vs", "", "Assets/Shaders/bolt/flash.fs", []() {
        std::cout << "Shader flash ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("explosion",
                                       "Assets/Shaders/explosion/explosion.vs",
                                       "Assets/Shaders/explosion/explosion.geom",
                                       "Assets/Shaders/explosion/explosion.fs", []() {
        std::cout << "Shader explosion ready!" << std::endl;
    });
}

// GameField *App::InitGameField() {
//     gameField = new GameField();
//     gameField->Init();
//
//     return gameField;
// }

// Snake *App::InitSnake() {
//     snake = new Snake();
//     snake->init();
//
//     return snake;
// }
//
// ObjWall *App::InitObjWall() {
//     objWall = new ObjWall();
//     objWall->init();
//
//     return objWall;
// }

// Radar *App::CreateRadar() {
//     auto radar = new Radar();
//
//     return radar;
// }

// void App::InitRadar() {
//     radar->reset();
//     radar->setVisible(true);
//     radar->setPosition({125.0, 160.0, 0.0});
//     radar->setZoom({100, 100, 1});
//     radar->setWidth(176);
//     radar->setHeight(176);
//
//     if (resourceManager) {
//         for (auto tile: snake->getItems()) {
//             // radar->addItem(tile->tile, {0.278,1.,0.});
//         }
//         for (auto block: barriers->getItems()) {
//             // radar->addItem(block, {0.694,0.078,0.016});
//         }
//         radar->addItem(eat, {1.,0.953,0.});
//     }
// }

// Eat *App::InitEat() const {
//     eat->setVirtualX((23 - -23) / 2 * 32 + 16);
//     eat->setVirtualY((-3 - -23) / 2 * 32 + 16);
//     eat->setPosition({-69.0, -69, -70.0f});
//     eat->setZoom({0.013888889, 0.013888889, 0.013888889});
//     eat->setRotate({1, 0, 0, 90}, {0, 1, 0, 0}, {0, 0, 1, 0});
//     eat->setVisible(false);
//
//     return eat;
// }

void App::initTexts() const {
    if (textRenderer && resourceManager) {
        startText->setVisible(true);
        startText->setColor({0.8, 0.8, 0.8});
        startText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
        startText->setFontSize(22);
        startText->setPosition({(width - 360) / 2, height / 2 + 15, 0.0});
        startText->setScale({1.0f, 0, 0});
        textRenderer->addText(startText, resourceManager->getShader("textShader").get());

        tilesCounterText->setVisible(true);
        tilesCounterText->setColor({0.8, 0.8, 0.8});
        tilesCounterText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
        tilesCounterText->setFontSize(22);
        tilesCounterText->setPosition({25.0f, 25.0f, 0.0});
        tilesCounterText->setScale({1.0f, 0, 0});
        textRenderer->addText(tilesCounterText, resourceManager->getShader("textShader").get());
    }
}
