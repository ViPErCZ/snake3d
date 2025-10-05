#include <AL/alc.h>
#include <AL/alut.h>
#include "App.h"

#include "Handler/Debug/PositionHandler.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/StandardMeshRenderer.h"
#include "Renderer/Opengl/TorchRenderer.h"
#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "Renderer/Opengl/Material/Uniform/TextureUniform.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "Renderer/Opengl/Model/Standard/CapsuleMesh.h"
#include "Resource/AnimLoader.h"
#include "Resource/ShaderLoader.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
App::App(Camera* camera, const int width, const int height) : camera(camera), width(width), height(height) {
    rendererManager = new RenderManager(width, height);
    keyboardManager = new KeyboardManager();
    startText = new Text("Press start I, K or L...");
    tilesCounterText = new Text("");
    eat = new Eat;
    skybox = new Cube();
    eatManager = nullptr;
}

App::~App() {
    delete rendererManager;
    delete resourceManager;
    delete keyboardManager;
    delete eatManager;
    delete collisionDetector;
    delete eat;
    delete skybox;
    delete levelManager;
    delete camera;
    alDeleteSources(1, &musicSource);
    alDeleteSources(1, &coinSource);
    alDeleteBuffers(1, &musicBuffer);
    alDeleteBuffers(1, &coinBuffer);
}

void App::Init() {
    InitResourceManager();

    const glm::mat4 projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );
    const glm::mat4 ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);

    resourceManager->addShader("textShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/text.vs", "Assets/Shaders/text.fs")));
    resourceManager->addShader("basicShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/basic.vs", "Assets/Shaders/basic.fs")));
    resourceManager->addShader("colorShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/color.vs", "Assets/Shaders/color.fs")));
    resourceManager->addShader("respawnShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/basic.vs", "Assets/Shaders/respawn/respawn.fs")));
    resourceManager->addShader("normalShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/normal_map.vs", "Assets/Shaders/normal_map.fs")));
    resourceManager->addShader("radarShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/radar.vs", "Assets/Shaders/radar.fs")));
    resourceManager->addShader("skyboxShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/skybox.vs", "Assets/Shaders/skybox.fs")));
    resourceManager->addShader("shadowShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/shadow_map.vs", "Assets/Shaders/shadow_map.fs")));
    resourceManager->addShader("shadowDepthShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/shadow_map_depth.vs", "Assets/Shaders/shadow_map_depth.fs")));
    resourceManager->addShader("debugQuadShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/debug_quad.vs", "Assets/Shaders/debug_quad.fs")));
    resourceManager->addShader("bloomLight", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/bloom/bloom.vs", "Assets/Shaders/bloom/bloom.fs")));
    resourceManager->addShader("rain", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/rain/rain.vs", "Assets/Shaders/rain/rain.fs")));
    resourceManager->addShader("rainDrop", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/basic.vs", "Assets/Shaders/rain/raindrop.fs")));
    resourceManager->addShader("fire", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/fire/fire.vs", "Assets/Shaders/fire/fire.fs")));
    resourceManager->addShader("smoke", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/fire/smoke.vs", "Assets/Shaders/fire/smoke.fs")));
    resourceManager->addShader("boltShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/bolt/bolt.vs", "Assets/Shaders/bolt/bolt.fs")));
    resourceManager->addShader("flash", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader("Assets/Shaders/bolt/flash.vs", "Assets/Shaders/bolt/flash.fs")));
    resourceManager->addShader("gizmoShader", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader(
                                       "Assets/Shaders/gizmo/gizmo.vs",
                                       "Assets/Shaders/gizmo/gizmo.geom",
                                       "Assets/Shaders/gizmo/gizmo.fs"
                                   )));
    resourceManager->addShader("explosion", std::make_shared<ShaderManager>(
                                   ShaderLoader::loadShader(
                                       "Assets/Shaders/explosion/explosion.vs",
                                       "Assets/Shaders/explosion/explosion.geom",
                                       "Assets/Shaders/explosion/explosion.fs"
                                   )));

    InitSnake();
    animRenderer = new AnimRenderer((*snake->getItems().begin()), resourceManager->getAnimationModel("pacman"), camera, projection, resourceManager);
    animRenderer->addPlay("KostraAction");
    animRenderer->setAcceleration(2.2f);
    snake->getHeadTile()->setVisible(false);
    //    animRenderer->addPlay("Armature|Take 001|BaseLayer");
    //    animRenderer->addPlay("Kostra2Action.002");
    //    animRenderer->addPlay("Kostra3Action");
    bloomRenderer = new BloomRenderer(resourceManager, width, height);
    depthMapRenderer = new DepthMapRenderer(camera, projection, resourceManager);
    gameFieldRenderer = new GameFieldRenderer(InitGameField(), camera, projection, resourceManager);
    eat = InitEat();
    ObjWall *objWall = InitObjWall();
    barriers = new Barriers();
    radar = CreateRadar();
    InitRadar();
    const auto torch = new Cube();
    torch->setPosition(glm::vec3(0.33, 0, -8.2));
    torch->setRotate(
        glm::vec4(1.0, 0.0, 0.0, 90.0f),
        glm::vec4(0.0, 1.0, 0.0, 0.0f),
        glm::vec4(0.0, 0.0, 1.0, 0.0f));
    torch->setZoom({0.12, 0.12, 0.12});

    levelManager = new LevelManager(1, MAX_LIVES, barriers);
    levelManager->createLevel(START_LEVEL);

    auto *eatLocationHandler = new EatLocationHandler(barriers, snake, eat, radar);
    eatManager = new EatManager(eatLocationHandler);

    const std::shared_ptr<ShaderManager> basicShader(
        resourceManager->getShader("basicShader"), [](ShaderManager *) {
        });
    const std::shared_ptr<ShaderManager> shadowDepthShader(
        resourceManager->getShader("shadowDepthShader"), [](ShaderManager *) {
        });
    const auto planeMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    const auto coinMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    const std::shared_ptr<TextureManager> gamefieldAlbedo(
        resourceManager->getTexture("gamefield.bmp"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> gamefieldNormal(
        resourceManager->getTexture("gamefield_normal.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> gamefieldSpecular(
        resourceManager->getTexture("gamefield_specular.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> skeletonAlbedo(
        resourceManager->getTexture("Skeleton_Body.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> skeletonORM(
        resourceManager->getTexture("Skeleton_Body_ORM.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> shadowMap(
        resourceManager->getTexture("depth"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> coinAlbedo(
        resourceManager->getTexture("Coin_Gold_albedo.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> coinNormal(
        resourceManager->getTexture("Coin_Gold_nm.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> coinMetalness(
        resourceManager->getTexture("Coin_Gold_metalness.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> coinRoughness(
        resourceManager->getTexture("Coin_Gold_rough.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> rustedAlbedo(
        resourceManager->getTexture("rusted_albedo.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> rustedNormal(
        resourceManager->getTexture("rusted_normal.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> rustedRoughness(
        resourceManager->getTexture("rusted_roughness.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> aoMap(
        resourceManager->getTexture("ao.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> rustedMetallic(
        resourceManager->getTexture("rusted_metallic.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> brickWall(
        resourceManager->getTexture("brickwork-texture.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> brickWallNormal(
        resourceManager->getTexture("brickwork_normal-map.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> brickWallSpecular(
        resourceManager->getTexture("brickwork-bump-map.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> environmentMap(
        resourceManager->getTexture("skybox"), [](TextureManager*) {
        });

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
    planeMaterial->setShadow(true);
    coinMaterial->setShadow(true);
    boxMaterial->setShadow(true);
    // planeMaterial->setEnvironmentMap(environmentMap);
    // coinMaterial->setEnvironmentMap(environmentMap);

    const std::shared_ptr<AnimationModel> pacmanModel(
        resourceManager->getAnimationModel("pacman"), [](AnimationModel *) {
        });
    const std::shared_ptr<AnimationModel> skeletonModel(
        resourceManager->getAnimationModel("skeleton"), [](AnimationModel *) {
        });

    const auto standardBaseItem = make_shared<BaseItem>(BaseItem());
    const auto standardBaseItem2 = make_shared<BaseItem>(BaseItem());
    const auto standardBaseItem3 = make_shared<BaseItem>(BaseItem());
    const auto standardPlaneMesh = make_shared<PlaneMesh>(PlaneMesh(standardBaseItem, basicShader, 1, 1));
    const auto standardBoxMesh = make_shared<BoxMesh>(BoxMesh(standardBaseItem3, basicShader, 1, 1, 1));
    const auto sphereMesh = make_shared<SphereMesh>(SphereMesh(standardBaseItem, basicShader));
    const auto capsuleMesh = make_shared<CapsuleMesh>(CapsuleMesh(standardBaseItem, basicShader));
    const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(standardBaseItem2, basicShader));
    const auto pacmanMesh = make_shared<AnimationArrayMesh>(AnimationArrayMesh(pacmanModel, basicShader));
    const auto skeletonMesh = make_shared<AnimationArrayMesh>(AnimationArrayMesh(skeletonModel, basicShader));

    const std::shared_ptr<ObjItem> coinObjItem(
        resourceManager->getModel("coin"), [](ObjItem *) {
        });
    coinMesh->fromObj(coinObjItem);
    standardPlaneMesh->setMaterial(planeMaterial);
    standardBoxMesh->setMaterial(boxMaterial);
    sphereMesh->setMaterial(planeMaterial);
    capsuleMesh->setMaterial(planeMaterial);
    coinMesh->setMaterial(coinMaterial);
    pacmanMesh->setMaterial(planeMaterial);
    skeletonMesh->setMaterial(planeMaterial);
    sphereMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    coinMesh->getBaseItem()->setPosition({5, 0, 0});
    coinMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    pacmanMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    skeletonMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    sphereMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    coinMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    pacmanMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    standardBoxMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    skeletonMesh->getBaseItem()->setZoom({0.2, 0.2, 0.2});
    // standardBaseItem->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    const std::shared_ptr<Camera> sharedCamera(camera, [](Camera*) {});
    const auto standardRenderer = new StandardMeshRenderer(sharedCamera, projection, sphereMesh);
    const auto standardRenderer2 = new StandardMeshRenderer(sharedCamera, projection, coinMesh);
    const auto standardRenderer3 = new StandardMeshRenderer(sharedCamera, projection, standardBoxMesh);
    const auto standardRenderer4 = new StandardMeshRenderer(sharedCamera, projection, sphereMesh);

    snakeRenderer = new SnakeRenderer(snake, camera, projection, resourceManager);
    objWallRenderer = new ObjWallRenderer(snake, objWall, camera, projection, resourceManager);
    barrierRenderer = new BarrierRenderer(snake, barriers, camera, projection, resourceManager);
    eatRenderer = new EatRenderer(eat, camera, projection, resourceManager);
    radarRenderer = new RadarRenderer(radar, camera, ortho, resourceManager);
    textRenderer = new TextRenderer(width, height);
    skyboxRenderer = new SkyboxRenderer(skybox, camera, projection, resourceManager);
    rainRenderer = new RainRenderer(new BaseItem(), camera, projection, resourceManager);
    rainDropRenderer = new RainDropRenderer(new BaseItem(), camera, projection, resourceManager);
    fireRenderer = new FireRenderer(camera, projection, resourceManager);
    torchRenderer = new TorchRenderer(torch, camera, projection, resourceManager);
    boltRenderer = new BoltRenderer(camera, projection, resourceManager);
    const auto storm = new BaseItem();
    storm->setVisible(false);

    initTexts();

    animateEat = new Eat;
    animateEat->setVisible(false);
    animateEat->setPosition(eat->getPosition());

    eatRemoveAnimateRenderer = new EatRemoveAnimateRenderer(animateEat, camera, projection, resourceManager);

    shared_ptr<PlaneMesh> planeMesh = initPlane();

    rendererManager->setWidth(width);
    rendererManager->setHeight(height);
    rendererManager->addRenderer(skyboxRenderer);
    rendererManager->addRenderer(new StandardMeshRenderer(sharedCamera, projection, planeMesh));
    rendererManager->addRenderer(standardRenderer4);
    //rendererManager->addRenderer(standardRenderer2);
    //rendererManager->addRenderer(standardRenderer3);
    // rendererManager->addRenderer(gameFieldRenderer);
    rendererManager->addRenderer(eatRenderer);
    rendererManager->addRenderer(eatRemoveAnimateRenderer);
    rendererManager->addRenderer(animRenderer);
    rendererManager->addRenderer(snakeRenderer);
    // rendererManager->addRenderer(rainDropRenderer);
    rendererManager->addRenderer(objWallRenderer);
    rendererManager->addRenderer(barrierRenderer);
    rendererManager->addRenderer(radarRenderer);
    // rendererManager->addRenderer(rainRenderer);
    rendererManager->addRenderer(torchRenderer);
    rendererManager->addRenderer(fireRenderer);
    rendererManager->addRenderer(boltRenderer);
    // rendererManager->addRenderer(textRenderer);
    rendererManager->setDepthMapRenderer(depthMapRenderer);
    rendererManager->setBloomRenderer(bloomRenderer);
    //rendererManager->enableShadows();
    camera->setStickyPoint(snake->getHeadTile());

    // pruhlednost se objevuje nepresne
    // idealni aby nebyla kosticka cela pruhledna, spodek kostky by mohl byt nepruhledny, ted to kvuli tomu divne mrka

    auto animHead = resourceManager->getAnimationModel("pacman");
    animHead->setBaseItem(snake->getHeadTile());
    auto *snakeMoveHandler = new SnakeMoveHandler(snake, animHead);
    auto *radarHandler = new RadarHandler(radar);

    collisionDetector = new CollisionDetector();
    collisionDetector->setPerimeter(objWall);
    collisionDetector->setBarriers(barriers);
    collisionDetector->addStaticItem(eat);
    snakeMoveHandler->setCollisionDetector(collisionDetector);
    snakeMoveHandler->setStartMoveCallback([this, animHead]() {
        if (this->levelManager) {
            animHead->setGlobalPause(false);
            this->eatManager->run(Manager::EatManager::firstPlace);
            this->startText->fadeOut();
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
            this->tilesCounterText->setText(buffAsStdStr);
            if (this->tilesCounterText->getAlpha() == 1.0f) {
                this->tilesCounterText->setAlpha(0.0f);
                this->tilesCounterText->fadeIn();
            }
        }
    });
    snakeMoveHandler->setCrashCallback([this]() {
        if (this->levelManager && this->barrierRenderer) {
            snake->reset();
            InitRadar();
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
            this->tilesCounterText->setText(buffAsStdStr);
            eat->setVisible(false);
            if (this->levelManager->getLive() == 0) { // Game Over
                this->levelManager->createLevel(1);
                this->startText->setVisible(true);
                this->levelManager->setLive(3);
                cout << "crash callback call" << endl;
            }
        }
    });
    snakeMoveHandler->setEatenUpCallback([this]() {
        if (this->levelManager && this->snake && this->barrierRenderer) {
            alSourcePlay (coinSource);

            if (const ALCenum error = alGetError(); error != AL_NO_ERROR) {
                cout << "Sound error" << endl;
            }

            if (this->eatRemoveAnimateRenderer && this->animateEat) {
                this->animateEat->setPosition(eat->getPosition());
                this->animateEat->setVisible(true);
                this->animateEat->fadeOut();
            }

            this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

            if (this->levelManager->getEatCounter() == MAX_POINT) {
                this->startText->setVisible(true);
                this->snake->reset();
                this->eat->setVisible(false);
                this->levelManager->createLevel(this->levelManager->getLevel() + 1);
                this->eatManager->run(Manager::EatManager::clean);
                InitRadar();
            } else {
                this->eatManager->run(Manager::EatManager::eatenUp);
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
            this->tilesCounterText->setText(buffAsStdStr);

        }
    });
    const auto positionHandler = new PositionHandler(camera);
    //positionHandler->addItem(pointLight.get());
    // positionHandler->addItem(eat);
    // positionHandler->addItem(directionalLight.get());
    // positionHandler->addItem(standardBaseItem2.get());
    // positionHandler->addItem(standardBaseItem.get());
    // positionHandler->addItem(standardBoxMesh->getBaseItem().get());
    // positionHandler->addItem(pacmanMesh->getBaseItem().get());
    // positionHandler->addItem(skeletonMesh->getBaseItem().get());
    positionHandler->addItem(planeMesh->getBaseItem().get());
    positionHandler->addItem(sphereMesh->getBaseItem().get());

    keyboardManager->addEventHandler(snakeMoveHandler);
    keyboardManager->addEventHandler(radarHandler);
    keyboardManager->addEventHandler(positionHandler);

    musicBuffer = (ALint)alutCreateBufferFromFile("Assets/Sounds/snake.wav");
    coinBuffer = (ALint)alutCreateBufferFromFile("Assets/Sounds/coin.wav");
    alGenSources (1, &musicSource);
    alGenSources (1, &coinSource);
    alSourcei (musicSource, AL_BUFFER, (ALint)musicBuffer);
    alSourcei (coinSource, AL_BUFFER, (ALint)coinBuffer);
    alSourcei (musicSource, AL_LOOPING, true);
    //alSourcePlay (musicSource);
    ALCenum error;

    error = alGetError();
    if (error != AL_NO_ERROR) {
        cout << "Sound error" << endl;
    }
}

void App::run() const {
    static float lastFrame = 0.0f;
    const auto currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    deltaTime = std::min(deltaTime, 0.05f);

    rendererManager->render(deltaTime);
    keyboardManager->runDefault();
    if (!startText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
        eatManager->run(Manager::EatManager::checkPlace);
    }
}

void App::processInput(GLFWwindow *window, const int keyCode, int scancode, const int action, int mods) const {
    keyboardManager->onKeyPress(keyCode, scancode, action, mods);

    switch (keyCode) {
        case GLFW_KEY_V:
            rendererManager->toggleShadows();
            break;
        case GLFW_KEY_P:
            if (objWallRenderer) {
                objWallRenderer->toggleParallax();
            }
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
        case GLFW_KEY_W:
            rainRenderer->toggle();
            rainDropRenderer->setEnable(rainRenderer->isEnable());
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
        case GLFW_KEY_1: // show classic red head
            snake->getHeadTile()->setVisible(true);
            animRenderer->setShow(false);
            snakeRenderer->toggleStyle(1);
            break;
        case GLFW_KEY_2: // show animated pacman head
            snake->getHeadTile()->setVisible(false);
            animRenderer->setShow(true);
            snakeRenderer->toggleStyle(2);
            break;
        case GLFW_KEY_T: // nebo jiná klávesa
            if (boltRenderer) {
                boltRenderer->triggerBolt();
            }
            break;
        default:
            break;
    }
}

void App::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 cursor(static_cast<float>(xpos), static_cast<float>(ypos));

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            torchRenderer->onMouseDown(cursor, width, height);
        } else if (action == GLFW_RELEASE) {
            torchRenderer->onMouseUp();
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        camera->onMouseDown(button, action, mods);
    }
}

void App::mousePositionCallback(GLFWwindow *window, const double x, const double y) const {
    const glm::vec2 cursor(static_cast<float>(x), static_cast<float>(y));
    torchRenderer->onMouseMove(cursor, width, height);
    camera->processMouseMovement(x, y);
}

void App::setKeyState(const int key, const bool pressed) const {
    camera->setKeyState(key, pressed);
}

void App::cameraProcessKeyboard(GLFWwindow *window) const {
    camera->processKeyboard(window, 1);
}

void App::InitResourceManager() {
    resourceManager = new ResourceManager();

    std::string path = "Assets/Textures/Albedo/";
    for (fs::recursive_directory_iterator i(path), end; i != end; ++i) {
        if (!is_directory(i->path())) {
            std::cout << i->path().filename() << std::endl;
            auto texture = std::make_shared<TextureManager>();
            texture->addTexture(TextureLoader::loadTexture(i->path()));
            resourceManager->addTexture(i->path().filename(), texture);
        }
    }

    path = "Assets/Textures/Others/";
    for (fs::recursive_directory_iterator i(path), end; i != end; ++i) {
        if (!is_directory(i->path())) {
            std::cout << i->path().filename() << std::endl;
            auto texture = std::make_shared<TextureManager>();
            texture->addTexture(TextureLoader::loadTexture(i->path(), false));
            resourceManager->addTexture(i->path().filename(), texture);
        }
    }

    const fs::path assets_dir{"Assets/Objects"};

    resourceManager->addModel("cube", ObjModelLoader::loadObj(assets_dir / "Cube.obj"));
    resourceManager->addModel("coin", ObjModelLoader::loadObj(assets_dir / "Coin.obj"));
    resourceManager->addModel("tile", AnimLoader::loadObj(assets_dir / "Tile.obj"));
    resourceManager->addModel("pacman", AnimLoader::loadObj(assets_dir / "pacman.glb")); //pac-man-ghosts-blue.glb
    resourceManager->addModel("skeleton", AnimLoader::loadObj(assets_dir / "skeleton.glb")); //pac-man-ghosts-blue.glb
    resourceManager->addModel("torch", ObjModelLoader::loadObj(assets_dir / "torch.obj"));

    vector<string> faces;
    faces.emplace_back("Assets/Skybox/cloud/right.jpg");
    faces.emplace_back("Assets/Skybox/cloud/left.jpg");
    faces.emplace_back("Assets/Skybox/cloud/top.jpg");
    faces.emplace_back("Assets/Skybox/cloud/bottom.jpg");
    faces.emplace_back("Assets/Skybox/cloud/front.jpg");
    faces.emplace_back("Assets/Skybox/cloud/back.jpg");
    unsigned int cubeMapTexture = TextureLoader::loadSkyboxTexture(faces);
    auto texture = std::make_shared<TextureManager>();
    texture->addTexture(cubeMapTexture);
    resourceManager->addTexture("skybox", texture);
}

GameField *App::InitGameField() {
    gameField = new GameField();
    gameField->Init();

    return gameField;
}

Snake *App::InitSnake() {
    snake = new Snake();
    snake->init();

    return snake;
}

ObjWall *App::InitObjWall() {
    objWall = new ObjWall();
    objWall->init();

    return objWall;
}

Radar *App::CreateRadar() {
    auto radar = new Radar();

    return radar;
}

void App::InitRadar() {
    radar->reset();
    radar->setVisible(true);
    radar->setPosition({125.0, 160.0, 0.0});
    radar->setZoom({100, 100, 1});
    radar->setWidth(176);
    radar->setHeight(176);

    if (resourceManager) {
        for (auto tile: snake->getItems()) {
            radar->addItem(tile->tile, {0.278,1.,0.});
        }
        for (auto block: barriers->getItems()) {
            radar->addItem(block, {0.694,0.078,0.016});
        }
        radar->addItem(eat, {1.,0.953,0.});
    }
}

Eat *App::InitEat() const {
    eat->setVirtualX((((int) (23 - (-23)) / 2) * 32) + 16);
    eat->setVirtualY((((int) (-3 - (-23)) / 2) * 32) + 16);
    eat->setPosition({-69.0, -69, -70.0f}); // velikost mince je cca 6x6
    eat->setZoom({0.013888889, 0.013888889, 0.013888889});
    eat->setRotate({1, 0, 0, 90}, {0, 1, 0, 0}, {0, 0, 1, 0});
    eat->setVisible(false);

    return eat;
}

void App::initTexts() const {
    if (textRenderer && resourceManager) {
        startText->setVisible(true);
        startText->setColor({0.8, 0.8, 0.8});
        startText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
        startText->setFontSize(22);
        startText->setPosition({(width - 360) / 2, height / 2 + 15, 0.0});
        startText->setZoom({1.0f, 0, 0});
        textRenderer->addText(startText, resourceManager->getShader("textShader"));

        tilesCounterText->setVisible(true);
        tilesCounterText->setColor({0.8, 0.8, 0.8});
        tilesCounterText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
        tilesCounterText->setFontSize(22);
        tilesCounterText->setPosition({25.0f, 25.0f, 0.0});
        tilesCounterText->setZoom({1.0f, 0, 0});
        textRenderer->addText(tilesCounterText, resourceManager->getShader("textShader"));
    }
}

shared_ptr<PlaneMesh> App::initPlane() const {
    const std::shared_ptr<ShaderManager> basicShader(
        resourceManager->getShader("basicShader"), [](ShaderManager *) {
        });
    const std::shared_ptr<ShaderManager> planeShader(
        resourceManager->getShader("shadowShader"), [](ShaderManager *) {
        });
    const std::shared_ptr<ShaderManager> shadowDepthShader(
        resourceManager->getShader("shadowDepthShader"), [](ShaderManager *) {
        });
    const std::shared_ptr<TextureManager> shadowMap(
        resourceManager->getTexture("depth"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> gamefieldAlbedo(
        resourceManager->getTexture("tile.png"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> gamefieldNormal(
        resourceManager->getTexture("gamefield_normal.jpg"), [](TextureManager*) {
        });
    const std::shared_ptr<TextureManager> gamefieldSpecular(
        resourceManager->getTexture("gamefield_specular.jpg"), [](TextureManager*) {
        });
    const auto planeMaterial = make_shared<StandardMaterial>(basicShader, shadowDepthShader);
    const auto shaderMaterial = make_shared<ShaderMaterial>(planeShader, shadowDepthShader);

    const auto albedo = make_shared<Uniform::TextureUniform>(0, gamefieldAlbedo);
    const auto normalMap = make_shared<Uniform::TextureUniform>(2, gamefieldNormal);
    const auto specularMap = make_shared<Uniform::TextureUniform>(3, gamefieldSpecular);
    const auto shadow = make_shared<Uniform::TextureArrayUniform>(4, shadowMap);
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
    planeMaterial->setShadow(true);
    planeMaterial->setNormalEnabled(true);
    planeMaterial->setAlbedo(gamefieldAlbedo);
    planeMaterial->setNormal(gamefieldNormal);
    planeMaterial->setSpecular(gamefieldSpecular);
    planeMaterial->set_uv_scale(glm::vec2(48.0f, 48.0f));

    const std::shared_ptr<Camera> sharedCamera(camera, [](Camera*) {});
    const auto standardBaseItem = make_shared<BaseItem>();
    standardBaseItem->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));
    standardBaseItem->setPosition(glm::vec3(1.0, 1.0, -1.0));
    const auto planeMesh = make_shared<PlaneMesh>(standardBaseItem, basicShader, 4, 4);
    planeMesh->setMaterial(planeMaterial);

    return planeMesh;
}
