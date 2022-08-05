#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "ItemsDto/GameField.h"
#include "ItemsDto/Snake.h"
#include "ItemsDto/Wall.h"
#include "ItemsDto/ObjWall.h"
#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Renderer/Opengl/GameFieldRenderer.h"
#include "Renderer/Opengl/SnakeRenderer.h"
#include "Renderer/Opengl/WallRenderer.h"
#include "Renderer/Opengl/RadarRenderer.h"
#include "Handler/SnakeMoveHandler.h"
#include "Handler/RadarHandler.h"
#include "Handler/EatLocationHandler.h"
#include "stdafx.h"
#include "Renderer/Opengl/EatRenderer.h"
#include "Renderer/Opengl/TextRenderer.h"
#include "ItemsDto/Eat.h"
#include "Manager/EatManager.h"
#include "Renderer/Opengl/EatRemoveAnimateRenderer.h"
#include "ItemsDto/Barriers.h"
#include "Renderer/Opengl/BarrierRenderer.h"
#include "Manager/LevelManager.h"
#include "Renderer/Opengl/ObjWallRenderer.h"
#include <filesystem>

#define MAX_POINT 6
#define MAX_LIVES 4

namespace fs = std::filesystem;
using namespace ItemsDto;
using namespace Manager;
using namespace Renderer;
using namespace Handler;

class App {
public:
    App(SDL_Window *window, SDL_Renderer *renderer, SDL_Event* event, int width, int height);
    ~App();
    void run();

protected:
    void Init();
    void InitResourceManager();
    GameField* InitGameField();
    Snake* InitSnake();
    Wall* InitWall(); // outer wall
    ObjWall* InitObjWall(); // outer wall
    Barriers* InitBarriers(); // inter barriers
    Radar* InitRadar();
private:
    LevelManager* levelManager{};
    ResourceManager* resourceManager{};
    RenderManager* rendererManager;
    GameField* gameField{};
    GameFieldRenderer* gameFieldRenderer{};
    Snake* snake{};
    Eat* animateEat;
    Eat* eat;
    Wall* wall{};
    ObjWall* objWall{};
    Barriers* barriers{};
    Radar* radar{};
    SnakeRenderer* snakeRenderer{};
    WallRenderer* wallRenderer{};
    ObjWallRenderer* objWallRenderer{};
    BarrierRenderer* barrierRenderer{};
    EatRenderer* eatRenderer{};
    RadarRenderer* radarRenderer{};
    TextRenderer* textRenderer{};
    EatRemoveAnimateRenderer* eatRemoveAnimateRenderer{};
    KeyboardManager* keyboardManager;
    CollisionDetector* collisionDetector{};
    EatManager* eatManager;
    Text* startText;
    Text* tilesCounterText;
    SDL_Window* window;
    SDL_Renderer *renderer;
    int width;
    int height;
    SDL_Event* event;

    Eat *InitEat();
};


#endif //SNAKE3_APP_H
