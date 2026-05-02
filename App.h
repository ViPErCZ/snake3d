#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Manager/EatManager.h"
#include "Manager/LevelManager.h"
#include "Manager/Camera.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Scenes/MainScene.h"
#include "Scenes/PreloaderScene.h"

namespace fs = std::filesystem;
using namespace ItemsDto;
using namespace Manager;
using namespace Renderer;
using namespace Handler;
using namespace Resource;
using namespace Model;
using namespace Material;

class App {
    enum class SceneState {
        LOADING,
        RUNNING
    };
public:
    App(const shared_ptr<Camera> &camera, int width, int height);

    void Init();
    void run();
    void processInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const;
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) const;
    void mousePositionCallback(GLFWwindow* window, double x, double y) const;
    void setKeyState(int key, bool pressed) const;
    void cameraProcessKeyboard(GLFWwindow *window) const;
    void resize(int width, int height);
protected:
    void initScene() const;
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
    shared_ptr<BoltRenderer> boltRenderer{};
    unique_ptr<KeyboardManager> keyboardManager;
    unique_ptr<EatManager> eatManager;
    shared_ptr<Camera> camera;
    glm::mat4 projection{};
    int width;
    int height;
    SceneState state = SceneState::LOADING;
    std::atomic<bool> scanning = false;
    shared_ptr<MainScene> mainScene;
    shared_ptr<PreloaderScene> preloaderScene;
    shared_ptr<ContextState> contextState;
};

#endif //SNAKE3_APP_H
