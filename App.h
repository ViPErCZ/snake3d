#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Renderer/Opengl/SkyboxRenderer.h"
#include "Handler/EatLocationHandler.h"
#include "Manager/EatManager.h"
#include "Manager/LevelManager.h"
#include "Manager/Camera.h"
#include "Renderer/Opengl/RainDropRenderer.h"
#include "Particle/SmokeParticleSystem.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/FireRenderer.h"
#include <AL/al.h>
#include <nlohmann/json.hpp>
#include "Scenes/MainScene.h"
#include "Scenes/PreloaderScene.h"

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
    void InitResourceManager() const;
private:
    struct TextureEntry {
        std::string name;
        std::string path;
        std::string category;
    };
    shared_ptr<Environment> environment;
    shared_ptr<ResourceManager> resourceManager;
    shared_ptr<RenderManager> rendererManager;
    shared_ptr<RainDropRenderer> rainDropRenderer{};
    shared_ptr<FireRenderer> fireRenderer{};
    shared_ptr<BoltRenderer> boltRenderer{};
    unique_ptr<KeyboardManager> keyboardManager;
    shared_ptr<CollisionDetector> collisionDetector;
    unique_ptr<EatManager> eatManager;
    shared_ptr<Camera> camera;
    glm::mat4 projection{};
    int width;
    int height;
    ALuint musicSource{}, coinSource{};
    ALuint coinBuffer{}, musicBuffer{};
    SceneState state = SceneState::LOADING;
    std::atomic<bool> scanning = false;
    shared_ptr<MainScene> mainScene;
    shared_ptr<PreloaderScene> preloaderScene;
    shared_ptr<ContextState> contextState;
};

#endif //SNAKE3_APP_H
