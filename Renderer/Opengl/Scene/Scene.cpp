#include "Scene.h"

namespace Scenes {

    Scene::Scene(const shared_ptr<RenderManager> &rendererManager,
        const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : resourceManager(rm), rendererManager(rendererManager), camera(camera), projection(projection) {
        keyboardManager = make_unique<KeyboardManager>();
        sceneRenderer = make_shared<SceneRenderer>(camera, projection);
    }

    void Scene::init() {
        rendererManager->addRenderer(sceneRenderer);
    }

    Scene::~Scene() {
        meshes.clear();
    }

    void Scene::update() {
        keyboardManager->runDefault();
        sceneRenderer->update(meshes);
    }

    void Scene::render() {
        static float lastFrame = 0.0f;
        const auto currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        deltaTime = std::min(deltaTime, 0.05f);

        rendererManager->render(deltaTime);
    }

    void Scene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
        keyboardManager->onKeyPress(keyCode, scancode, action, mods);
    }
} // Scene
