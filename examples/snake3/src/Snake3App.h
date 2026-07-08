#ifndef SNAKE3_SNAKE3APP_H
#define SNAKE3_SNAKE3APP_H

#include <memory>

#include <snake3d/Application.h>

namespace Scenes {
    class MainScene;
    class PreloaderScene;
}

// Snake3-specific game entry point. Derives the generic engine
// Application and provides Snake3 scene construction + per-frame logic
// via the virtual hook protocol.
class Snake3App : public snake3d::Application {
public:
    Snake3App();
    ~Snake3App() override;

protected:
    snake3d::WindowConfig getWindowConfig() const override;

    void onInit() override;
    void onSceneReady() override;
    void onFrame(float dt) override;
    void onLoadingFrame() override;

    void onKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void onMouseButton(GLFWwindow* window, int button, int action, int mods) override;
    void onCursorPos(GLFWwindow* window, double x, double y) override;
    void onResize(int width, int height, const glm::mat4& projection) override;

    bool shouldProcessCameraKeyboard() const override;
    bool shouldProcessCameraMouse() const override;

private:
    std::shared_ptr<Scenes::MainScene>      mainScene;
    std::shared_ptr<Scenes::PreloaderScene> preloaderScene;

    // Splits of the previous monolithic App init for readability.
    void registerShaders();        // shader registry masters + eager addShader entries
    void preflightShaders();       // warmup + abort on failure
    void initResourceManifest();   // texture manifest + async shader loads (game-specific)
    void buildPreloaderScene();    // construct + init the loading scene
    void wireDebugOverlay();       // wire scene/manipulator/collision into ImGuiOverlay
};

#endif //SNAKE3_SNAKE3APP_H
