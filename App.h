#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "ItemsDto/Snake.h"
#include "ItemsDto/ObjWall.h"
#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Renderer/Opengl/GameFieldRenderer.h"
#include "Renderer/Opengl/SkyboxRenderer.h"
#include "Renderer/Opengl/SnakeRenderer.h"
#include "Renderer/Opengl/RadarRenderer.h"
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
#include "Renderer/Opengl/RainRenderer.h"
#include "Renderer/Opengl/RainDropRenderer.h"
#include "Renderer/Opengl/AnimRenderer.h"
#include "Particle/SmokeParticleSystem.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/FireRenderer.h"
#include "Renderer/Opengl/TorchRenderer.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include <AL/al.h>
#include <nlohmann/json.hpp>
#include "Renderer/Opengl/Model/SpinnerModel.h"
#include "Scenes/MainScene.h"

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
    void Init() const;
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
    // Snake* InitSnake();
    // ObjWall* InitObjWall(); // outer wall
    //static Radar* CreateRadar();
    //void InitRadar();
    // [[nodiscard]] Eat *InitEat() const;
    void initTexts() const;
private:
    struct TextureEntry {
        std::string name;
        std::string path;
        std::string category;
    };
    unique_ptr<LevelManager> levelManager;
    shared_ptr<ResourceManager> resourceManager;
    shared_ptr<RenderManager> rendererManager;
    shared_ptr<GameFieldRenderer> gameFieldRenderer{};
    Snake* snake{};
    Eat* animateEat{};
    Eat* eat;
    // Radar* radar{};
    ObjWall* objWall{};
    Barriers* barriers = nullptr;
    shared_ptr<SnakeRenderer> snakeRenderer;
    shared_ptr<ObjWallRenderer> objWallRenderer{};
    shared_ptr<BarrierRenderer> barrierRenderer{};
    shared_ptr<EatRenderer> eatRenderer{};
    shared_ptr<RadarRenderer> radarRenderer{};
    shared_ptr<TextRenderer> textRenderer{};
    shared_ptr<EatRemoveAnimateRenderer> eatRemoveAnimateRenderer{};
    shared_ptr<RainRenderer> rainRenderer{};
    // shared_ptr<AnimRenderer> animRenderer{};
    shared_ptr<RainDropRenderer> rainDropRenderer{};
    shared_ptr<FireRenderer> fireRenderer{};
    shared_ptr<TorchRenderer> torchRenderer{};
    shared_ptr<BoltRenderer> boltRenderer{};
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
    shared_ptr<MainScene> mainScene;
};

#endif //SNAKE3_APP_H
