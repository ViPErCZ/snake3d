#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "ItemsDto/GameField.h"
#include "ItemsDto/Snake.h"
#include "ItemsDto/ObjWall.h"
#include "Resource/ObjModelLoader.h"
#include "Resource/ShaderLoader.h"
#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Renderer/Opengl/GameFieldRenderer.h"
#include "Renderer/Opengl/SkyboxRenderer.h"
#include "Renderer/Opengl/SnakeRenderer.h"
#include "Renderer/Opengl/RadarRenderer.h"
#include "Renderer/Opengl/DepthMapRenderer.h"
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
#include "Manager/Camera.h"
#include <filesystem>

#define MAX_POINT 6
#define MAX_LIVES 4
#define START_LEVEL 1

namespace fs = std::filesystem;
using namespace ItemsDto;
using namespace Manager;
using namespace Renderer;
using namespace Handler;
using namespace Resource;

class App {
public:
    App(Camera* camera, int width, int height);
    ~App();
    void Init();
    void run();
    void processInput(int keyCode);
protected:
    void InitResourceManager();
    GameField* InitGameField();
    Snake* InitSnake();
    ObjWall* InitObjWall(); // outer wall
    Barriers* InitBarriers(); // inter barriers
    Radar* InitRadar();
    Eat *InitEat();
    void initTexts();
private:
    LevelManager* levelManager{};
    ResourceManager* resourceManager{};
    RenderManager* rendererManager;
    GameField* gameField{};
    GameFieldRenderer* gameFieldRenderer{};
    SkyboxRenderer* skyboxRenderer{};
    Snake* snake{};
    Eat* animateEat{};
    Eat* eat;
    ObjWall* objWall{};
    Barriers* barriers{};
    Radar* radar{};
    Cube* skybox{};
    SnakeRenderer* snakeRenderer{};
    ObjWallRenderer* objWallRenderer{};
    BarrierRenderer* barrierRenderer{};
    EatRenderer* eatRenderer{};
    RadarRenderer* radarRenderer{};
    TextRenderer* textRenderer{};
    DepthMapRenderer* depthMapRenderer{};
    EatRemoveAnimateRenderer* eatRemoveAnimateRenderer{};
    KeyboardManager* keyboardManager;
    CollisionDetector* collisionDetector{};
    EatManager* eatManager;
    Text* startText;
    Text* tilesCounterText;
    Camera* camera;
    int width;
    int height;
};


#endif //SNAKE3_APP_H
