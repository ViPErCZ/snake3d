#include "Scene.h"

namespace Scenes {
    Scene::Scene(const shared_ptr<RenderManager> &rendererManager,
                 const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                 const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : resourceManager(rm), rendererManager(rendererManager), camera(camera), projection(projection),
          width(width), height(height) {
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
        for (const auto &node: nodes) {
            node->update();
        }
    }

    void Scene::render() {
        static float lastFrame = 0.0f;
        const auto currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        deltaTime = std::min(deltaTime, 0.05f);

        rendererManager->render(deltaTime);
    }

    void Scene::addNode(const std::shared_ptr<Scene> &node) {
        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 10) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 10");
        }
        nodes.push_back(node);
    }

    void Scene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action,
                              const int mods) const {
        keyboardManager->onKeyPress(keyCode, scancode, action, mods);
    }
} // Scene
