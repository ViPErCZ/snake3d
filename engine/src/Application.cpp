#include <snake3d/Application.h>

#include <chrono>
#include <iostream>

#include <stdafx.h>

#include <snake3d/Renderer/Opengl/Material/Feature/FogFeature.h>
#include <snake3d/Renderer/Opengl/RenderStats.h>
#include <snake3d/Tools/BuildSettings.h>

using namespace snake3d;

namespace {
    // GLFW user pointer bridge - all callbacks recover the Application*
    // via glfwGetWindowUserPointer. Mirror of the standalone callbacks that
    // previously lived in main.cpp.
    inline Application* appFromWindow(GLFWwindow* window) {
        return static_cast<Application*>(glfwGetWindowUserPointer(window));
    }

    void glfwFramebufferSizeCb(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        if (auto* app = appFromWindow(window)) {
            app->resize(width, height);
        }
    }

    void glfwCursorPosCb(GLFWwindow* window, double x, double y) {
        if (auto* app = appFromWindow(window)) {
            app->mousePositionCallback(window, x, y);
        }
    }

    void glfwMouseButtonCb(GLFWwindow* window, int button, int action, int mods) {
        if (auto* app = appFromWindow(window)) {
            app->mouseButtonCallback(window, button, action, mods);
        }
    }

    void glfwScrollCb(GLFWwindow* /*window*/, double /*offsetX*/, double /*offsetY*/) {
        // Reserved for future use - currently game classes don't subscribe to scroll.
    }

    void glfwKeyCb(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* app = appFromWindow(window);
        if (!app) return;

        if (action == GLFW_PRESS) {
            app->setKeyState(key, true);
        } else if (action == GLFW_RELEASE) {
            app->setKeyState(key, false);
        }

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            app->processInput(window, key, scancode, action, mods);
        }

        app->cameraProcessKeyboard(window);
    }
}

Application::Application() {
    // Default camera - derived class can swap via setCamera() in onInit().
    camera = std::make_shared<Manager::Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    cursorModeBeforeSpectator = GLFW_CURSOR_HIDDEN;
}

Application::~Application() = default;

void Application::setCamera(std::shared_ptr<Manager::Camera> cam) {
    if (cam) {
        camera = std::move(cam);
        // Propagate to the RenderManager (if already built) so the per-frame
        // FrameData UBO uses this camera. Without this the manager keeps the
        // default camera it was constructed with in initManagers() and the scene
        // renders from the wrong viewpoint (black screen for a fixed camera).
        if (rendererManager) {
            rendererManager->setCamera(camera);
        }
    }
}

void Application::launch() {
    const WindowConfig cfg = getWindowConfig();
    width = cfg.width;
    height = cfg.height;

    if (!glfwInit()) {
        std::cerr << "[Application] glfwInit failed\n";
        return;
    }

    glfwSetErrorCallback([](int /*error*/, const char* description) {
        std::cerr << "Error: " << description << '\n';
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
        cfg.compatProfile ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);

    // Fullscreen (release default for some games) creates on the primary monitor at its
    // native video mode; windowed uses the configured size. Alt+Enter toggles at runtime.
    cursorAlwaysVisible_ = cfg.visibleCursor;
    windowedW_ = cfg.width;
    windowedH_ = cfg.height;
    GLFWmonitor* fsMonitor = cfg.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    int createW = cfg.width, createH = cfg.height;
    if (fsMonitor) {
        if (const GLFWvidmode* mode = glfwGetVideoMode(fsMonitor)) { createW = mode->width; createH = mode->height; }
    }
    GLFWwindow* window = glfwCreateWindow(createW, createH, cfg.title.c_str(), fsMonitor, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "[Application] glfwCreateWindow failed\n";
        return;
    }
    isFullscreen_ = fsMonitor != nullptr;
    window_ = window;
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCb);
    glfwSetCursorPosCallback(window, glfwCursorPosCb);
    glfwSetMouseButtonCallback(window, glfwMouseButtonCb);
    glfwSetScrollCallback(window, glfwScrollCb);
    glfwSetKeyCallback(window, glfwKeyCb);

    glewInit();

    glfwGetFramebufferSize(window, &width, &height);

    // Debug build defaultně ukáže systémový kurzor - umožní klikat na ImGui
    // overlay bez RMB hold. Release build ukáže prázdný kurzor (hra má vlastní
    // crosshair / nepoužívá GUI). Camera rotation v debug je gated na
    // Ctrl/RMB v mousePositionCallback.
    if (cfg.visibleCursor || Build::isDebug) {
        // Mouse-driven games (RTS) and debug builds keep the OS cursor visible so you can
        // see where you point / click and drive the menu.
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    // Build managers + projection now that GL is alive (some construct GL
    // resources eagerly). resize() applies the framebuffer dimensions to
    // dependents.
    initManagers();
    resize(width, height);

    // Derived game gets its chance to register shaders / kick async loads /
    // build the loading scene, with all managers already constructed.
    onInit();

    initDebugOverlay(window);

    lastFrameTime_ = glfwGetTime(); // seed so the first frame's dt is ~one frame, not the whole startup
    while (!glfwWindowShouldClose(window)) {
        runFrame();

        // glfw: swap buffers and poll IO events. Měřeno - dlouhý swap typicky
        // znamená vsync wait (GPU work nedoběhl). Long pollEvents je rare.
        const auto swapStart = std::chrono::steady_clock::now();
        glfwSwapBuffers(window);
        glfwPollEvents();
        Renderer::RenderStats::swapMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - swapStart).count();
    }

    // Teardown order: derived state via destructor, then GLFW.
    imguiOverlay.reset();
    rendererManager.reset();
    resourceManager.reset();
    shaderRegistry.reset();
    keyboardManager.reset();
    camera.reset();
    contextState.reset();
    environment.reset();

    glfwDestroyWindow(window);
    glfwTerminate();
    window_ = nullptr;
}

void Application::initManagers() {
    resourceManager = std::make_shared<Manager::ResourceManager>();
    shaderRegistry  = std::make_shared<Manager::ShaderRegistry>();
    resourceManager->setShaderRegistry(shaderRegistry);
    resourceManager->setFogFeature(std::make_shared<Feature::FogFeature>(false));
    keyboardManager = std::make_unique<Manager::KeyboardManager>();

    projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );
    contextState = std::make_shared<Tools::ContextState>();

    rendererManager = std::make_shared<Manager::RenderManager>(
        contextState, camera, resourceManager, projection, width, height);
    rendererManager->setWidth(width);
    rendererManager->setHeight(height);

    environment = std::make_shared<Tools::Environment>();
}

void Application::initDebugOverlay(GLFWwindow* window) {
    if constexpr (Build::isDebug) {
        imguiOverlay = std::make_unique<Handler::Debug::ImGuiOverlay>(window, rendererManager);
    }
}

void Application::runFrame() {
    // Real wall-clock delta since the previous frame, advanced every iteration
    // (loading frames included) so the first RUNNING frame gets a one-frame dt.
    const double now = glfwGetTime();
    const float dt = static_cast<float>(now - lastFrameTime_);
    lastFrameTime_ = now;

    if (state == SceneState::LOADING) {
        resourceManager->processPending();

        if (!scanning.load() && resourceManager->isAllLoaded()) {
            std::cout << "\rLoading DONE!      " << std::endl;
            state = SceneState::RUNNING;

            rendererManager->reset();
            onSceneReady();
        }
    }

    if constexpr (Build::isDebug) {
        if (imguiOverlay) imguiOverlay->beginFrame();
    }

    if (state == SceneState::RUNNING) {
        onFrame(dt);
    } else {
        onLoadingFrame();
    }

    if constexpr (Build::isDebug) {
        if (imguiOverlay) {
            imguiOverlay->renderPanels();
            imguiOverlay->endFrame();
        }
    }
}

void Application::processInput(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
    // Alt+Enter: toggle fullscreen <-> windowed (engine-wide, all games get it).
    if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && (mods & GLFW_MOD_ALT)) {
        toggleFullscreen();
        return;
    }
    onKeyboardInput(window, key, scancode, action, mods);
}

void Application::toggleFullscreen() {
    if (!window_) return;
    if (isFullscreen_) {
        // Back to windowed at the saved geometry.
        glfwSetWindowMonitor(window_, nullptr, windowedX_, windowedY_, windowedW_, windowedH_, 0);
        isFullscreen_ = false;
    } else {
        // Save the current windowed geometry, then go fullscreen on the primary monitor.
        glfwGetWindowPos(window_, &windowedX_, &windowedY_);
        glfwGetWindowSize(window_, &windowedW_, &windowedH_);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr) {
            glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            isFullscreen_ = true;
        }
    }
    // A monitor switch can reset the cursor input mode; re-assert the game's choice.
    glfwSetInputMode(window_, GLFW_CURSOR,
                     (cursorAlwaysVisible_ || Build::isDebug) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void Application::mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods) {
    // Game-specific routing first (so e.g. main scene can claim the click).
    onMouseButton(window, button, action, mods);

    if (camera) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // Disable the cursor while holding RMB so the spectator camera gets
            // unbounded mouse deltas - otherwise the cursor hits the screen edge
            // and yaw stops accumulating after ~half a turn.
            if (action == GLFW_PRESS) {
                cursorModeBeforeSpectator = glfwGetInputMode(window, GLFW_CURSOR);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, cursorModeBeforeSpectator);
            }
            camera->onMouseDown(button, action, mods);
        }
    }
}

void Application::mousePositionCallback(GLFWwindow* window, const double x, const double y) {
    // Always notify derived game first - it may need cursor position even
    // when paused / menu visible / debug rotation gating is off.
    onCursorPos(window, x, y);

    if (state != SceneState::RUNNING || camera == nullptr || !shouldProcessCameraMouse()) {
        return;
    }

    if constexpr (Build::isDebug) {
        const bool ctrlHeld = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
                           || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        const bool rmbHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if (!ctrlHeld && !rmbHeld) {
            lastMouseRotationSkipped = true;
            return;
        }

        if (lastMouseRotationSkipped) {
            camera->resetMouseDelta();
            lastMouseRotationSkipped = false;
        }
    }

    camera->processMouseMovement(x, y);
}

void Application::setKeyState(const int key, const bool pressed) {
    if (camera) camera->setKeyState(key, pressed);
}

void Application::cameraProcessKeyboard(GLFWwindow* window) {
    if (!shouldProcessCameraKeyboard()) return;
    if (camera) camera->processKeyboard(window, 1);
}

void Application::resize(const int newWidth, const int newHeight) {
    if (newWidth <= 0 || newHeight <= 0) {
        return;
    }

    width = newWidth;
    height = newHeight;
    projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );

    if (rendererManager) {
        rendererManager->resize(width, height, projection);
    }

    onResize(width, height, projection);
}
