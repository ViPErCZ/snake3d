#include "App.h"

App::App(int width, int height) : width(width), height(height) {
    rendererManager = new RenderManager(width, height);
    keyboardManager = new KeyboardManager();
    startText = new Text("Press start I, K or L...");
    tilesCounterText = new Text("");
    eat = new Eat;
    eatManager = nullptr;
    textShader = new ShaderManager;
    baseShader = new ShaderManager;
    normalShader = new ShaderManager;
    camera = new Camera(glm::vec3(0.0f, -1.0f, 2.0f));
}

App::~App() {
    delete rendererManager;
//    delete objWallRenderer;
    delete resourceManager;
    delete keyboardManager;
    delete eatManager;
    delete collisionDetector;
    delete eat;
    delete levelManager;
    delete textShader;
    delete baseShader;
    delete normalShader;
    delete camera;
}

void App::Init() {
    InitResourceManager();

    glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), (float)width / (float)height, 1.5f, 2600.0f);
    glm::mat4 view = camera->getViewMatrix();

    gameFieldRenderer = new GameFieldRenderer(InitGameField(), normalShader, camera, projection, resourceManager);
    Snake *snake = InitSnake();
    eat = InitEat();
    Wall *wall = InitWall();
    ObjWall *objWall = InitObjWall();
    Barriers *barriers = InitBarriers();
    Radar *radar = InitRadar();

    levelManager = new LevelManager(1, MAX_LIVES, barriers);
    levelManager->createLevel(1);

    auto *eatLocationHandler = new EatLocationHandler(barriers, snake, eat);
    eatManager = new EatManager(eatLocationHandler);

    snakeRenderer = new SnakeRenderer(snake, baseShader, camera, projection, resourceManager);
    wallRenderer = new WallRenderer(wall);
    objWallRenderer = new ObjWallRenderer(objWall, normalShader, camera, projection, resourceManager);
    barrierRenderer = new BarrierRenderer(barriers);
    eatRenderer = new EatRenderer(eat, normalShader, camera, projection, resourceManager);
    radarRenderer = new RadarRenderer(radar);
    textRenderer = new TextRenderer(width, height);

    animateEat = new Eat;
    animateEat->load("Assets/Objects/Coin.obj");
    animateEat->setVisible(false);
    animateEat->setPosition(eat->getPosition());

    eatRemoveAnimateRenderer = new EatRemoveAnimateRenderer(animateEat, normalShader, camera, projection, resourceManager);

    textShader->loadShader("Assets/Shaders/text.vs", "Assets/Shaders/text.fs");
    baseShader->loadShader("Assets/Shaders/basic.vs", "Assets/Shaders/basic.fs");
    normalShader->loadShader("Assets/Shaders/normal_map.vs", "Assets/Shaders/normal_map.fs");

    startText->setVisible(true);
    startText->setColor({0.8, 0.8, 0.8});
    startText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
    startText->setFontSize(22);
    startText->setPosition({(width - 360) / 2, height / 2, 0.0});
    startText->setZoom({1.0f, 0, 0});
    textRenderer->addText(startText, textShader);

    tilesCounterText->setVisible(true);
    tilesCounterText->setColor({0.8, 0.8, 0.8});
    tilesCounterText->setFontPath("Assets/Fonts/OCRAEXT.TTF");
    tilesCounterText->setFontSize(22);
    tilesCounterText->setPosition({25.0f, 25.0f, 0.0});
    tilesCounterText->setZoom({1.0f, 0, 0});
    textRenderer->addText(tilesCounterText, textShader);

    rendererManager->addRenderer(gameFieldRenderer);
    rendererManager->addRenderer(snakeRenderer);
//    rendererManager->addRenderer(wallRenderer);
    rendererManager->addRenderer(objWallRenderer);
//    rendererManager->addRenderer(barrierRenderer);
    rendererManager->addRenderer(eatRenderer);
    rendererManager->addRenderer(eatRemoveAnimateRenderer);
//    rendererManager->addRenderer(radarRenderer);
    rendererManager->addRenderer(textRenderer);
    camera->setStickyPoint(snake->getHeadTile());

    auto *snakeMoveHandler = new SnakeMoveHandler(snake);
    auto *radarHandler = new RadarHandler(radar);

    collisionDetector = new CollisionDetector();
    collisionDetector->setPerimeter(wall);
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
        if (this->levelManager) {
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
        if (this->levelManager && this->snake) {

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
}

Eat *App::InitEat() {
    eat->load("Assets/Objects/Coin.obj");
    eat->setVirtualX((((int)(23 - (-23)) / 2) * 32) + 16);
    eat->setVirtualY((((int)(-3 - (-23)) / 2) * 32) + 16);
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

Wall *App::InitWall() {
    wall = new Wall();
    wall->init();

    return wall;
}

Barriers *App::InitBarriers() {
    barriers = new Barriers();
    barriers->init();

    return barriers;
}

Radar *App::InitRadar() {
    radar = new Radar();
    radar->setVisible(true);
    radar->setPosition({5.0, 28.0, 0.0});
    radar->setZoom({2, 2, 2});
    radar->addTexture(11);
    radar->setWidth(164);
    radar->setHeight(164);

    if (snake) {
        radar->addItem(snake->getHeadTile(), 13);
    }
    radar->addItem(eat, 12);

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
            if (i->path().filename() == "gamefield.bmp") {
                resourceManager->createTexture(i->path().c_str(), i->path().filename(), GL_LINEAR);
            }
            resourceManager->createTexture(i->path().c_str(), i->path().filename());
        }
    }
}

void App::run() {
    camera->updateStickyPoint();
    rendererManager->render();
    keyboardManager->runDefault();
    if (!startText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
        eatManager->run(Manager::EatManager::checkPlace);
    }
}

void App::processInput(int keyCode) {
    keyboardManager->onKeyPress(keyCode);
}
