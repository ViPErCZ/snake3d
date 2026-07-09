#ifndef SNAKE3D_APPLICATION_H
#define SNAKE3D_APPLICATION_H

#include <atomic>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <snake3d/Handler/Debug/ImGuiOverlay.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Manager/KeyboardManager.h>
#include <snake3d/Manager/RenderManager.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/ShaderRegistry.h>
#include <snake3d/Tools/ContextState.h>
#include <snake3d/Tools/Environment.h>

struct GLFWwindow;

namespace snake3d {

    // Window/GL hints used by Application::launch to create the GLFW window.
    // Derived game classes override Application::getWindowConfig() to
    // customize title/size/profile.
    struct WindowConfig {
        std::string title = "Snake3D Engine";
        int width = 1920;
        int height = 1080;
        int glMajor = 3;
        int glMinor = 3;
        bool compatProfile = true;
        bool fullscreen = false;    // create the window fullscreen on the primary monitor (Alt+Enter toggles)
        bool visibleCursor = false; // keep the OS cursor visible even in release (mouse-driven games: RTS)
    };

    enum class SceneState {
        LOADING,
        RUNNING
    };

    // Generic application/engine entry point. Owns GLFW + GL bootstrap, the
    // core managers, the main loop skeleton, and routing of GLFW callbacks.
    // Derived game classes override the virtual hooks to provide scene-specific
    // construction and per-frame logic.
    //
    // Template-method protocol:
    //  - getWindowConfig() — once, before window creation
    //  - onInit()          — once, after managers + GL + overlay are up
    //  - onSceneReady()    — once, on LOADING -> RUNNING transition
    //  - onFrame(dt)       — every RUNNING frame
    //  - onLoadingFrame()  — every LOADING frame
    //  - onKeyboardInput / onMouseButton / onCursorPos / onResize — input/resize routing
    class Application {
    public:
        Application();
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Runs the whole lifecycle: GLFW init, window/GL create, callback
        // registration, manager construction, onInit, main loop, teardown.
        void launch();

        // Public so the static GLFW callback trampolines can forward into them.
        // (They are not virtual hooks themselves - derived classes override
        // the onXxx variants below.)
        void resize(int width, int height);
        void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
        void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void mousePositionCallback(GLFWwindow* window, double x, double y);
        void setKeyState(int key, bool pressed);
        void cameraProcessKeyboard(GLFWwindow* window);

    protected:
        // Override to customize window title / size / GL version.
        virtual WindowConfig getWindowConfig() const { return {}; }

        // Once after managers + GL + debug overlay are constructed. Derived
        // game registers shaders, kicks off async loads, builds the loading
        // scene, configures the camera, etc.
        virtual void onInit() {}

        // Once on LOADING -> RUNNING transition. Derived game calls
        // mainScene->init() / attachRenderer() and wires the debug overlay.
        virtual void onSceneReady() {}

        // Per-frame in RUNNING state. Derived game runs scene update + physics
        // + render.
        virtual void onFrame(float dt) { (void)dt; }

        // Per-frame in LOADING state. Derived game runs the preloader scene.
        virtual void onLoadingFrame() {}

        // GLFW input / resize hook overrides. Default impls do nothing; the
        // base also calls into camera / cursor mode where it owns the policy.
        virtual void onKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
            (void)window; (void)key; (void)scancode; (void)action; (void)mods;
        }
        virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
            (void)window; (void)button; (void)action; (void)mods;
        }
        virtual void onCursorPos(GLFWwindow* window, double x, double y) {
            (void)window; (void)x; (void)y;
        }
        virtual void onResize(int width, int height, const glm::mat4& projection) {
            (void)width; (void)height; (void)projection;
        }

        // Gates the per-frame camera-keyboard feed. Derived game can suppress
        // when e.g. menu is visible. Default: feed.
        virtual bool shouldProcessCameraKeyboard() const { return true; }

        // Gates the per-frame camera-mouse feed (after debug Ctrl/RMB gate).
        // Derived game can suppress when menu is visible / paused. Default: feed.
        virtual bool shouldProcessCameraMouse() const { return true; }

        // Replace the default camera (constructed by ctor with position (0,0,3)).
        // Call from onInit() if the game needs PITCH/offset/etc. Updates
        // dependents (projection-derived state stays untouched - resize() picks
        // up the new zoom on next resize).
        void setCamera(std::shared_ptr<Manager::Camera> cam);

        // --- Managers / state shared with derived class ---
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Manager::ShaderRegistry>  shaderRegistry;
        std::shared_ptr<Manager::RenderManager>   rendererManager;
        std::unique_ptr<Manager::KeyboardManager> keyboardManager;
        std::shared_ptr<Manager::Camera>          camera;
        std::shared_ptr<Tools::ContextState>      contextState;
        std::shared_ptr<Tools::Environment>       environment;
        std::unique_ptr<Handler::Debug::ImGuiOverlay> imguiOverlay;

        glm::mat4 projection{1.0f};
        int width = 1920;
        int height = 1080;
        std::atomic<bool> scanning{false};
        SceneState state = SceneState::LOADING;
        double lastFrameTime_ = 0.0;            // glfwGetTime() of the previous frame; seeds per-frame dt
        int  cursorModeBeforeSpectator = 0;     // GLFW_CURSOR_HIDDEN sentinel; init in ctor
        bool lastMouseRotationSkipped = true;

        // Fullscreen state + saved windowed geometry, for the Alt+Enter toggle.
        bool cursorAlwaysVisible_ = false;      // from WindowConfig.visibleCursor
        bool isFullscreen_ = false;
        int windowedX_ = 0, windowedY_ = 0, windowedW_ = 1600, windowedH_ = 900;
        void toggleFullscreen();

        GLFWwindow* window_ = nullptr;

    private:
        // Per-frame loop body invoked from launch()'s main loop.
        void runFrame();

        // Manager construction (called by launch() after GLFW + glew are up).
        void initManagers();

        // Debug overlay setup, called by launch() after managers + GL.
        void initDebugOverlay(GLFWwindow* window);
    };

} // namespace snake3d

#endif // SNAKE3D_APPLICATION_H
