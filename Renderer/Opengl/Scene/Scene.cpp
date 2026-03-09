#include "Scene.h"

namespace Scenes {
    Scene::Scene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager,
        const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : resourceManager(rm), rendererManager(rendererManager), camera(camera),
          projection(projection), width(width), height(height) {
        this->directionalLight = directionalLight;
        if (directionalLight == nullptr) {
            this->directionalLight = make_shared<DirectionalLight>();
            this->directionalLight->setPosition({0.0f, 0.0f, 0.0f});
            this->directionalLight->setDirection({-0.410001, -0.82, 0.84});
            this->directionalLight->setAmbient({0.07f, 0.07f, 0.07f});
            //this->directionalLight->setDiffuse({0.0f, 0.0f, 0.0f});
            this->directionalLight->setDiffuse({0.01f, 0.01f, 0.01f});
            this->directionalLight->setSpecular({.051f, .051f, .051f});
        }

        this->spotLights = spotLights;
        this->pointLights = pointLights;

        keyboardManager = make_unique<KeyboardManager>();
        soundManager = make_unique<SoundManager>();
        if (!soundManager->initialize()) {
            std::cout << "Sound system init failed." << std::endl;
        }
        sceneRenderer = make_shared<SceneRenderer>(camera, projection, width, height);
        contextState = rendererManager->getContextState();
    }

    void Scene::init(const int priority) {
        rendererManager->addRenderer(sceneRenderer, priority);
        rendererManager->updateDirectionalLight(directionalLight);
    }

    Scene::~Scene() {
        meshNode3d.clear();
    }

    void Scene::update() {
        keyboardManager->runDefault();
        sceneRenderer->update(meshNode3d, meshNode2d);
        for (const auto &node: nodes) {
            node->update();
        }

        for (auto &node3D : meshNode3d) {
            node3D.node->computeWorldMatrix(glm::mat4(1.0f));
        }
    }

    void Scene::physics() {
        if (collisionSystem != nullptr) {
            collisionSystem->update();
        }
    }

    void Scene::render() {
        static float lastFrame = 0.0f;
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
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
        keyboardManager->onKeyPress(keyCode, scancode, action, mods, deltaTime);
        for (const auto &node: nodes) {
            node->keyboardInput(window, keyCode, scancode, action, mods);
        }
    }

    void Scene::addMeshNode3D(shared_ptr<MeshNode3D> node, const int priority) {
        meshNode3d.push_back({std::move(node), priority});
        ranges::stable_sort(meshNode3d,
                            [](auto &a, auto &b) { return a.priority > b.priority; });
    }

    void Scene::addMeshNode2D(shared_ptr<MeshNode2D> node, const int priority) {
        meshNode2d.push_back({std::move(node), priority});
        ranges::stable_sort(meshNode2d,
                            [](auto &a, auto &b) { return a.priority > b.priority; });
    }

    vector<RendererEntry3D> Scene::getAllMeshNodes3D() const {
        vector<RendererEntry3D> allNodes = meshNode3d;
        for (const auto &node : nodes) {
            auto childNodes = node->getAllMeshNodes3D();
            allNodes.insert(allNodes.end(), childNodes.begin(), childNodes.end());
        }
        return allNodes;
    }

    void Scene::setCollisionSystem(const shared_ptr<CollisionSystem3D> &collisionSystem) {
        this->collisionSystem = collisionSystem;
    }

    void Scene::setManipulatorHandler(const shared_ptr<ManipulatorHandler> &manipulatorHandler) {
        this->manipulatorHandler = manipulatorHandler;
    }

    SoundManager &Scene::getSoundManager() const {
        return *soundManager;
    }
} // Scene
