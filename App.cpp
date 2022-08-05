#include "App.h"

App::App(SDL_Window *window,
         SDL_Renderer *renderer,
         SDL_Event *event,
         int width,
         int height) : window(window), renderer(renderer), event(event), width(width), height(height) {
    rendererManager = new RenderManager(width, height);
    keyboardManager = new KeyboardManager(event);
    startText = new Text("Press start I, K or R...");
    tilesCounterText = new Text("");
    eat = new Eat;
    eatManager = nullptr;

    Init();
}

App::~App() {
    delete rendererManager;
    delete objWallRenderer;
    delete resourceManager;
    delete keyboardManager;
    delete eatManager;
    delete collisionDetector;
    delete eat;
    delete levelManager;
}

void App::Init() {
    InitResourceManager();

    gameFieldRenderer = new GameFieldRenderer(InitGameField());
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

    snakeRenderer = new SnakeRenderer(snake);
    wallRenderer = new WallRenderer(wall);
    objWallRenderer = new ObjWallRenderer(objWall);
    barrierRenderer = new BarrierRenderer(barriers);
    eatRenderer = new EatRenderer(eat);
    radarRenderer = new RadarRenderer(radar);
    textRenderer = new TextRenderer();

    animateEat = new Eat;
    animateEat->load("Assets/Objects/Coin.obj");
    animateEat->setVisible(false);
    animateEat->setPosition(eat->getPosition());
    animateEat->addTexture(12);
    animateEat->setZoom({9, 9, 9});
    animateEat->setRotate(eat->getRotate()[0], eat->getRotate()[1], eat->getRotate()[2]);

    eatRemoveAnimateRenderer = new EatRemoveAnimateRenderer(animateEat);

    startText->setVisible(true);
    startText->setColor({0.8, 0.8, 0.8});
    startText->setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    textRenderer->addText(startText);

    tilesCounterText->setVisible(true);
    tilesCounterText->setColor({0.8, 0.8, 0.8});
    tilesCounterText->setFont(GLUT_BITMAP_8_BY_13);
    textRenderer->addText(tilesCounterText);

    rendererManager->addRenderer(gameFieldRenderer);
    rendererManager->addRenderer(snakeRenderer);
    rendererManager->addRenderer(wallRenderer);
//    rendererManager->addRenderer(objWallRenderer);
    rendererManager->addRenderer(barrierRenderer);
    rendererManager->addRenderer(eatRenderer);
    rendererManager->addRenderer(eatRemoveAnimateRenderer);
    rendererManager->addRenderer(radarRenderer);
    rendererManager->addRenderer(textRenderer);
    rendererManager->setStickyPoint(snake->getHeadTile());

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

            if (this->eatRemoveAnimateRenderer) {
                animateEat->setZoom({9, 9, 9});
                animateEat->setPosition(eat->getPosition());
                animateEat->setVisible(true);
                this->eatRemoveAnimateRenderer->setCompleted(false);
            }

            this->levelManager->setEatCounter(this->levelManager->getEatCounter() + 1);

            if (this->levelManager->getEatCounter() == MAX_POINT) {
                this->startText->setVisible(true);
                this->snake->reset();
                eat->setVisible(false);
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
    eat->addTexture(12);
    eat->setZoom({9, 9, 9});
    eat->setPosition({150, 150, 15});
    eat->setRotate({90, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1});
    eat->setVisible(false);

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
            resourceManager->createTexture((char *) i->path().c_str());
        }
    }
}

void App::run() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    rendererManager->render();
    keyboardManager->run();
    if (!startText->isVisible()) { // pokud hra bezi, tak checkneme zda je videt jidlo, pokud ne zkusime znova umisti
        eatManager->run(Manager::EatManager::checkPlace);
    }

    switch (event->type) {
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    width = event->window.data1;
                    height = event->window.data2;
                    glViewport(0, 0, width, height);
                    startText->setPosition({1920 / 2 - 90, 1080 / 2, 0});
                    tilesCounterText->setPosition({10, 15, 0});

                    // Clear window content
                    glClear(GL_COLOR_BUFFER_BIT);
                    glFlush();

                    break;
            }
            break;
    }

    SDL_GL_SwapWindow(window);
}
