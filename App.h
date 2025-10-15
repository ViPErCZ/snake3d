#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "ItemsDto/GameField.h"
#include "ItemsDto/Snake.h"
#include "ItemsDto/ObjWall.h"
#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Renderer/Opengl/GameFieldRenderer.h"
#include "Renderer/Opengl/SkyboxRenderer.h"
#include "Renderer/Opengl/SnakeRenderer.h"
#include "Renderer/Opengl/RadarRenderer.h"
#include "Renderer/Opengl/DepthMapRenderer.h"
#include "Handler/RadarHandler.h"
#include "Handler/EatLocationHandler.h"
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
#include "Renderer/Opengl/BloomRenderer.h"
#include "Renderer/Opengl/RainRenderer.h"
#include "Renderer/Opengl/RainDropRenderer.h"
#include "Renderer/Opengl/AnimRenderer.h"
#include "Particle/SmokeParticleSystem.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/FireRenderer.h"
#include "Renderer/Opengl/TorchRenderer.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include "Renderer/Opengl/Model/Standard/PlaneMesh.h"
#include <AL/al.h>
#include <nlohmann/json.hpp>
#include "Renderer/Opengl/Model/SpinnerModel.h"

#define MAX_POINT 6
#define MAX_LIVES 4
#define START_LEVEL 2

namespace fs = std::filesystem;
using namespace ItemsDto;
using namespace Manager;
using namespace Renderer;
using namespace Handler;
using namespace Resource;
using namespace Particle;
using namespace Model;
using namespace Material;

class App {
    enum class SceneState {
        LOADING,
        RUNNING
    };
public:
    App(const shared_ptr<Camera> &camera, int width, int height);
    ~App();
    void Init();
    void run();
    void processInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const;
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) const;
    void mousePositionCallback(GLFWwindow* window, double x, double y) const;
    void setKeyState(int key, bool pressed) const;
    void cameraProcessKeyboard(GLFWwindow *window) const;
protected:
    void initScene();
    [[nodiscard]] shared_ptr<SpinnerModel> initPreloader() const;
    void InitResourceManager() const;
    GameField* InitGameField();
    Snake* InitSnake();
    ObjWall* InitObjWall(); // outer wall
    static Radar* CreateRadar();
    void InitRadar();
    [[nodiscard]] Eat *InitEat() const;
    void initTexts() const;
    [[nodiscard]] shared_ptr<PlaneMesh> initPlane() const;
private:
    struct TextureEntry {
        std::string name;
        std::string path;
        std::string category;
    };
    unique_ptr<LevelManager> levelManager;
    unique_ptr<ResourceManager> resourceManager;
    unique_ptr<RenderManager> rendererManager;
    GameField* gameField{};
    GameFieldRenderer* gameFieldRenderer{};
    SkyboxRenderer* skyboxRenderer{};
    Snake* snake{};
    Eat* animateEat{};
    Eat* eat;
    Radar* radar{};
    ObjWall* objWall{};
    Barriers* barriers = nullptr;
    shared_ptr<Cube> skybox;
    SnakeRenderer* snakeRenderer{};
    ObjWallRenderer* objWallRenderer{};
    BarrierRenderer* barrierRenderer{};
    EatRenderer* eatRenderer{};
    RadarRenderer* radarRenderer{};
    TextRenderer* textRenderer{};
    DepthMapRenderer* depthMapRenderer{};
    BloomRenderer* bloomRenderer{};
    EatRemoveAnimateRenderer* eatRemoveAnimateRenderer{};
    RainRenderer* rainRenderer{};
    AnimRenderer* animRenderer{};
    RainDropRenderer* rainDropRenderer{};
    FireRenderer* fireRenderer{};
    TorchRenderer* torchRenderer{};
    BoltRenderer* boltRenderer{};
    unique_ptr<KeyboardManager> keyboardManager;
    shared_ptr<CollisionDetector> collisionDetector;
    unique_ptr<EatManager> eatManager;
    Text* startText;
    Text* tilesCounterText;
    shared_ptr<Camera> camera;
    glm::mat4 projection{};
    int width;
    int height;
    ALuint musicSource{}, coinSource{};
    ALuint coinBuffer{}, musicBuffer{};
    SceneState state = SceneState::LOADING;
    std::atomic<bool> scanning = false;
};

#endif //SNAKE3_APP_H
