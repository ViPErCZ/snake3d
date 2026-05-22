#include "Scene.h"
#include <algorithm>
#include <functional>

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
        renderPriority = priority;
    }

    Scene::~Scene() {
        meshNode3d.clear();
    }

    void Scene::update() {
        keyboardManager->runDefault();
        sceneRenderer->update(meshNode3d, meshNode2d);
        for (const auto &node: nodes) {
            node.second->update();
        }

        for (auto &node3D : meshNode3d) {
            node3D.node->computeWorldMatrix(glm::mat4(1.0f));
        }
    }

    void Scene::physics() {
        if (collisionSystem == nullptr) {
            return;
        }
        static double lastPhysicsTime = glfwGetTime();
        const double now = glfwGetTime();
        // Clamp dt to avoid huge integration jumps after a loading hitch or pause.
        const float dt = std::min(static_cast<float>(now - lastPhysicsTime), 0.05f);
        lastPhysicsTime = now;
        collisionSystem->step(dt);
    }

    void Scene::render() {
        static float lastFrame = 0.0f;
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        deltaTime = std::min(deltaTime, 0.05f);

        rendererManager->render(deltaTime);
    }

    void Scene::resize(const int width, const int height, const glm::mat4 &projection) {
        this->width = width;
        this->height = height;
        this->projection = projection;
        if (sceneRenderer) {
            sceneRenderer->resize(width, height, projection);
        }
        for (auto &[name, node] : nodes) {
            if (node) {
                node->resize(width, height, projection);
            }
        }
    }

    void Scene::addNode(const std::shared_ptr<Scene> &node) {
        const string name = node->getName();
        if (nodes.contains(name)) {
            throw std::runtime_error("Scene node with name '" + name + "' already exists.");
        }
        addNode(name, node);
    }

    void Scene::addNode(const std::string &name, const std::shared_ptr<Scene> &node) {
        if (nodes.contains(name)) {
            throw std::runtime_error("Scene node with name '" + name + "' already exists.");
        }

        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 10) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 10");
        }
        nodes[name] = node;
        node->attachRenderer();
    }

    bool Scene::hasNode(const std::string &name) const {
        return nodes.contains(name);
    }

    shared_ptr<Scene> Scene::getNode(const std::string &name) const {
        const auto it = nodes.find(name);
        if (it == nodes.end()) {
            return nullptr;
        }
        return it->second;
    }

    bool Scene::removeNode(const std::string &name) {
        const auto it = nodes.find(name);
        if (it == nodes.end()) {
            return false;
        }

        it->second->detachRenderer();
        nodes.erase(it);
        return true;
    }

    bool Scene::replaceNode(const std::string &name, const std::shared_ptr<Scene> &node) {
        const auto it = nodes.find(name);
        if (it == nodes.end()) {
            return false;
        }

        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 10) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 10");
        }

        it->second->detachRenderer();
        it->second = node;
        node->attachRenderer();
        return true;
    }

    void Scene::attachRenderer() {
        if (rendererAttached) {
            return;
        }
        rendererManager->addRenderer(sceneRenderer, renderPriority);
        rendererManager->updateDirectionalLight(directionalLight);
        rendererAttached = true;
    }

    void Scene::detachRenderer() {
        if (!rendererAttached) {
            return;
        }
        rendererManager->removeRenderer(sceneRenderer);
        rendererAttached = false;
    }

    bool Scene::isRendererAttached() const {
        return rendererAttached;
    }

    void Scene::keyboardInput(GLFWwindow *window, const int keyCode, const int scancode, const int action,
                              const int mods) {
        keyboardManager->onKeyPress(keyCode, scancode, action, mods, deltaTime);
        for (const auto &node: nodes) {
            node.second->keyboardInput(window, keyCode, scancode, action, mods);
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
            auto childNodes = node.second->getAllMeshNodes3D();
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

    namespace {
        // DFS přes MeshNode3D subtree. Hledá CollisionShape3D přes polymorphic
        // isCollisionShapeNode() v children + collisionShapes vectoru (snake
        // používá setCollisionShape který přidá jen do collisionShapes, ne
        // children - proto je nutné chodit přes oba).
        //
        // respectParentVisibility: pokud true, hidden parent meshNode (např.
        // remote snake v single-player se setActive(false)) přeskočí celý
        // subtree - jeho collision shapes nejsou enable-able. Pro vypnutí
        // (visible=false) projdeme všechno bez ohledu na visibility.
        void walkCollisionShapes(const shared_ptr<MeshNode3D>& node,
                                 const std::function<void(MeshNode3D*)>& fn,
                                 bool respectParentVisibility) {
            if (!node) return;
            if (respectParentVisibility && !node->isCollisionShapeNode() && !node->isVisible()) {
                return;
            }
            if (node->isCollisionShapeNode()) {
                fn(node.get());
            }
            for (const auto& cs : node->getCollisionShapes()) {
                walkCollisionShapes(cs, fn, respectParentVisibility);
            }
            for (const auto& child : node->getChildren()) {
                walkCollisionShapes(child, fn, respectParentVisibility);
            }
        }
    }

    void Scene::setCollisionShapesVisible(const bool visible) {
        // Při zapínání respektujeme visibility parent meshNodes - inactive
        // sub-scény (remote snake v single-player se setActive(false))
        // nesmí dostat své shapes zviditelněné. Při vypínání projdeme všechno
        // pro idempotentní reset stavu.
        const bool respectParents = visible;
        auto fn = [visible](MeshNode3D* n) { n->setVisible(visible); };
        for (const auto& entry : meshNode3d) {
            walkCollisionShapes(entry.node, fn, respectParents);
        }
        for (const auto& [_, sub] : nodes) {
            if (sub) sub->setCollisionShapesVisible(visible);
        }
    }

    void Scene::collectCollisionShapeCounts(int& total, int& visible) const {
        // Pro počítání bereme jen aktivní subtree (hidden parents skip) -
        // jinak by Inspector ukazoval "1/8 visible" když je 7 z 8 v hidden
        // remote scéně. respectParentVisibility=true.
        auto fn = [&](MeshNode3D* n) {
            ++total;
            if (n->isVisible()) ++visible;
        };
        for (const auto& entry : meshNode3d) {
            walkCollisionShapes(entry.node, fn, true);
        }
        for (const auto& [_, sub] : nodes) {
            if (sub) sub->collectCollisionShapeCounts(total, visible);
        }
    }

    bool Scene::isAllCollisionShapesVisible() const {
        int total = 0, visible = 0;
        collectCollisionShapeCounts(total, visible);
        return total > 0 && total == visible;
    }
} // Scene
