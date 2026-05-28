#ifndef SNAKE3_APP_H
#define SNAKE3_APP_H

#include "Manager/ResourceManager.h"
#include "Manager/RenderManager.h"
#include "Manager/KeyboardManager.h"
#include "Manager/EatManager.h"
#include "Manager/LevelManager.h"
#include "Manager/Camera.h"
#include "Manager/ShaderRegistry.h"
#include <snake3d/Tools/Environment.h>
#include <snake3d/Tools/BuildSettings.h>
#include "Scenes/MainScene.h"
#include "Scenes/PreloaderScene.h"
#include <snake3d/Handler/Debug/ImGuiOverlay.h>

namespace fs = std::filesystem;
using namespace Manager;
using namespace Renderer;
using namespace Handler;
using namespace Resource;
using namespace Model;
using namespace Material;
using namespace Scenes;

class App {
    enum class SceneState {
        LOADING,
        RUNNING
    };
public:
    App(const shared_ptr<Camera> &camera, int width, int height);

    void Init();
    // C4: GLFWwindow je nutný pro ImGui backend init. Voláno z main.cpp po
    // glewInit. Vlastní debug overlay (F1 plán) lifecycle.
    void initDebugOverlay(GLFWwindow* window);
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
    // B1: paralelní registry vedle ResourceManager::addShader. V dalších PR
    // (B2+) převezme roli single source of truth pro shader programy.
    shared_ptr<ShaderRegistry> shaderRegistry;
    shared_ptr<RenderManager> rendererManager;
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
    mutable int cursorModeBeforeSpectator = GLFW_CURSOR_HIDDEN;

    // Sleduje gating mouse callback - edge transition (rotation off → on)
    // resetuje firstMouse v kameře aby přechod nezpůsobil skok orientace.
    mutable bool lastMouseRotationSkipped = true;

    // F1 ImGui debug overlay - žije jen v debug buildu (z if constexpr).
    // Lifecycle: created in initDebugOverlay() po GLFW + GL contextu;
    // destruktor v App destructor obejde s shutdown ImGui.
    std::unique_ptr<Handler::Debug::ImGuiOverlay> imguiOverlay;
};

#endif //SNAKE3_APP_H
