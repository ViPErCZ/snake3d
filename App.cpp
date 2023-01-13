#include "App.h"

App::App(Camera* camera, int width, int height) : width(width), height(height), camera(camera) {
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

    glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), (float) width / (float) height, 1.5f,
                                            2600.0f);
    glm::mat4 ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    glm::mat4 view = camera->getViewMatrix();

    resourceManager->addShader("textShader", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/text.vs", "Assets/Shaders/text.fs")));
    resourceManager->addShader("basicShader", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/basic.vs", "Assets/Shaders/basic.fs")));
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
            ShaderLoader::loadShader("Assets/Shaders/bloom/bloom.vs", "Assets/Shaders/bloom/light.fs")));

    bloomRenderer = new BloomRenderer(resourceManager, width, height);
    depthMapRenderer = new DepthMapRenderer(camera, projection, resourceManager);
    gameFieldRenderer = new GameFieldRenderer(InitGameField(), camera, projection, resourceManager);
    Snake *snake = InitSnake();
    eat = InitEat();
    ObjWall *objWall = InitObjWall();
    Barriers *barriers = InitBarriers();
    Radar *radar = InitRadar();

    levelManager = new LevelManager(1, MAX_LIVES, barriers);
    levelManager->createLevel(START_LEVEL);

    auto *eatLocationHandler = new EatLocationHandler(barriers, snake, eat);
    eatManager = new EatManager(eatLocationHandler);

    snakeRenderer = new SnakeRenderer(snake, camera, projection, resourceManager);
    objWallRenderer = new ObjWallRenderer(objWall, camera, projection, resourceManager);
    barrierRenderer = new BarrierRenderer(barriers, camera, projection, resourceManager);
    eatRenderer = new EatRenderer(eat, camera, projection, resourceManager);
    radarRenderer = new RadarRenderer(radar, camera, ortho, resourceManager);
    textRenderer = new TextRenderer(width, height);
    skyboxRenderer = new SkyboxRenderer(skybox, camera, projection, resourceManager);

    initTexts();

    animateEat = new Eat;
    animateEat->setVisible(false);
    animateEat->setPosition(eat->getPosition());

    eatRemoveAnimateRenderer = new EatRemoveAnimateRenderer(animateEat, camera, projection,
                                                            resourceManager);

    rendererManager->setWidth(width);
    rendererManager->setHeight(height);
    rendererManager->addRenderer(gameFieldRenderer);
    rendererManager->addRenderer(objWallRenderer);
    rendererManager->addRenderer(barrierRenderer);
    rendererManager->addRenderer(eatRenderer);
    rendererManager->addRenderer(eatRemoveAnimateRenderer);
    rendererManager->addRenderer(snakeRenderer);
    rendererManager->addRenderer(radarRenderer);
    rendererManager->addRenderer(skyboxRenderer);
    rendererManager->addRenderer(textRenderer);
    rendererManager->setDepthMapRenderer(depthMapRenderer);
    rendererManager->setBloomRenderer(bloomRenderer);
    //rendererManager->enableShadows();
    camera->setStickyPoint(snake->getHeadTile());

    auto *snakeMoveHandler = new SnakeMoveHandler(snake);
    auto *radarHandler = new RadarHandler(radar);

    collisionDetector = new CollisionDetector();
    collisionDetector->setPerimeter(objWall);
    collisionDetector->setBarriers(barriers);
    collisionDetector->addStaticItem(eat);
    snakeMoveHandler->setCollisionDetector(collisionDetector);
    snakeMoveHandler->setStartMoveCallback([this]() {
        if (this->levelManager) {
            this->eatManager->run(Manager::EatManager::firstPlace);
            this->startText->setVisible(false);
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
    snakeMoveHandler->setCrashCallback([this]() {
        if (this->levelManager && this->barrierRenderer) {
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
            if (this->levelManager->getLive() == 0) { // Game Over
                this->levelManager->createLevel(1);
                this->startText->setVisible(true);
                this->levelManager->setLive(3);
                eat->setVisible(false);
                cout << "crash callback call" << endl;
            }
        }
    });
    snakeMoveHandler->setEatenUpCallback([this]() {
        if (this->levelManager && this->snake && this->barrierRenderer) {
            alSourcePlay (coinSource);
            ALCenum error;

            error = alGetError();
            if (error != AL_NO_ERROR) {
                cout << "Sound error" << endl;
            }

            if (this->eatRemoveAnimateRenderer && this->animateEat) {
                this->animateEat->setPosition(eat->getPosition());
                this->animateEat->setVisible(true);
                this->eatRemoveAnimateRenderer->setCompleted(false);
            }

            this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

            if (this->levelManager->getEatCounter() == MAX_POINT) {
                this->startText->setVisible(true);
                this->snake->reset();
                this->eat->setVisible(false);
                this->levelManager->createLevel(this->levelManager->getLevel() + 1);
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
    keyboardManager->addEventHandler(snakeMoveHandler);
    keyboardManager->addEventHandler(radarHandler);

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

void App::initTexts() {
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

Eat *App::InitEat() {
    eat->setVirtualX((((int) (23 - (-23)) / 2) * 32) + 16);
    eat->setVirtualY((((int) (-3 - (-23)) / 2) * 32) + 16);
    eat->setPosition({-69.0, -69, -70.0f}); // velikost mince je cca 6x6

    eat->setRotate({90, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1});
    eat->setVisible(true);

    return eat;
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

Barriers *App::InitBarriers() {
    barriers = new Barriers();
    barriers->init();

    return barriers;
}

Radar *App::InitRadar() {
    radar = new Radar();
    radar->setVisible(true);
    radar->setPosition({125.0, 160.0, 0.0});
    radar->setZoom({100, 100, 1});
    radar->setWidth(176);
    radar->setHeight(176);

    if (resourceManager) {
        if (snake) {
            radar->addItem(snake->getHeadTile(), resourceManager->getTexture("radar_snake.bmp"));
        }
        radar->addItem(eat, resourceManager->getTexture("eat.bmp"));
    }

    return radar;
}

ObjWall *App::InitObjWall() {
    objWall = new ObjWall();
    objWall->init();

    return objWall;
}

void App::InitResourceManager() {
    resourceManager = new ResourceManager();

    std::string path = "Assets/Textures/";
    for (fs::recursive_directory_iterator i(path), end; i != end; ++i) {
        if (!is_directory(i->path())) {
            std::cout << i->path().filename() << std::endl;
            auto texture = std::make_shared<TextureManager>();
            texture->addTexture(TextureLoader::loadTexture(i->path()));
            resourceManager->addTexture(i->path().filename(), texture);
        }
    }

    const fs::path assets_dir{"Assets/Objects"};
    resourceManager->addModel("cube", ObjModelLoader::loadObj(assets_dir / "Cube.obj"));
    resourceManager->addModel("coin", ObjModelLoader::loadObj(assets_dir / "Coin.obj"));

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

void App::run() {
    camera->upsideDownRotate();
    camera->updateStickyPoint();
    rendererManager->render();
    keyboardManager->runDefault();
    if (!startText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
        eatManager->run(Manager::EatManager::checkPlace);
    }
}

void App::processInput(int keyCode) {
    keyboardManager->onKeyPress(keyCode);

    switch (keyCode) {
        case GLFW_KEY_V:
            rendererManager->toggleShadows();
            break;
        case GLFW_KEY_P:
            if (objWallRenderer) {
                objWallRenderer->toggleParallax();
            }
            break;
        case GLFW_KEY_RIGHT:
            if (objWallRenderer) {
                objWallRenderer->upScale();
            }
            break;
        case GLFW_KEY_LEFT:
            if (objWallRenderer) {
                objWallRenderer->downScale();
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
        case GLFW_KEY_U:
            camera->startUpsideDownRotate();
            break;
        case GLFW_KEY_M:
            ALint source_state;
            alGetSourcei(musicSource, AL_SOURCE_STATE, &source_state);

            if (source_state == AL_PLAYING) {
                alSourceStop(musicSource);
            } else {
                alSourcePlay(musicSource);
            }
        default:
            break;
    }
}
