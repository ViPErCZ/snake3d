#ifndef SNAKE3_SCENE_H
#define SNAKE3_SCENE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <snake3d/Renderer/Opengl/Scene/SceneRenderer.h>
#include <snake3d/Handler/Debug/ManipulatorHandler.h>
#include <snake3d/Lights/DirectionalLight.h>
#include <snake3d/Lights/PointLight.h>
#include <snake3d/Lights/SpotLight.h>
#include <snake3d/Manager/KeyboardManager.h>
#include <snake3d/Manager/RenderManager.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/SoundManager.h>
#include <snake3d/Physic/CollisionSystem3D.h>
#include <snake3d/Tools/BuildSettings.h>
#include <snake3d/Tools/Environment.h>

namespace Scenes {
    class Scene : public std::enable_shared_from_this<Scene>, public Node3D::Named {
    public:
        virtual ~Scene();

        explicit Scene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager,
            const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection,
            const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        virtual void init(int priority);

        virtual void update();

        virtual void physics();

        virtual void render();

        virtual void resize(int width, int height, const glm::mat4 &projection);

        void addNode(const std::shared_ptr<Scene>& node);
        void addNode(const std::string &name, const std::shared_ptr<Scene> &node);
        [[nodiscard]] bool hasNode(const std::string &name) const;
        [[nodiscard]] std::shared_ptr<Scene> getNode(const std::string &name) const;
        bool removeNode(const std::string &name);
        bool replaceNode(const std::string &name, const std::shared_ptr<Scene> &node);
        void attachRenderer();
        void detachRenderer();
        [[nodiscard]] bool isRendererAttached() const;

        virtual void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods);

        void addMeshNode3D(std::shared_ptr<Model::MeshNode3D> node, int priority = 0);
        void addMeshNode2D(std::shared_ptr<Model::MeshNode2D> node, int priority = 0);

        std::vector<RendererEntry3D> getAllMeshNodes3D() const;

        void setEnvironment(const std::shared_ptr<Tools::Environment> &environment) {
            this->environment = environment;
        }

        void setCollisionSystem(const std::shared_ptr<Physic::CollisionSystem3D> &collisionSystem);

        void setManipulatorHandler(const std::shared_ptr<Handler::Debug::ManipulatorHandler> &manipulatorHandler);
        [[nodiscard]] std::shared_ptr<Handler::Debug::ManipulatorHandler> getManipulatorHandler() const { return manipulatorHandler; }

        // C/D ImGui debug: bulk toggle všech CollisionShape3D ve scéně i v
        // sub-scénách (rekurzivně přes nodes mapu + MeshNode3D children).
        // Chytí i dynamicky přidané shapes (snake body) i ty, které nejsou
        // registrované v CollisionShapeHandler::items (per-game registrace
        // byla manuální v 3 scénách, ostatní chyběly).
        void setCollisionShapesVisible(bool visible);
        [[nodiscard]] bool isAllCollisionShapesVisible() const;

        // Interní rekurzivní counter pro isAllCollisionShapesVisible.
        // public, aby parent mohl agregovat sub-scenes; podtřídy by ho
        // přepisovat neměly.
        void collectCollisionShapeCounts(int& total, int& visible) const;

        Manager::SoundManager &getSoundManager() const;

    protected:
        std::vector<RendererEntry3D> meshNode3d;
        std::vector<RendererEntry2D> meshNode2d;
        std::shared_ptr<Tools::Environment> environment;
        std::shared_ptr<Lights::DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<Lights::SpotLight> > spotLights;
        std::vector<std::shared_ptr<Lights::PointLight> > pointLights;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<SceneRenderer> sceneRenderer;
        std::shared_ptr<Manager::RenderManager> rendererManager;
        std::unique_ptr<Manager::KeyboardManager> keyboardManager;
        std::shared_ptr<Manager::Camera> camera;
        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<Handler::Debug::ManipulatorHandler> manipulatorHandler;
        std::shared_ptr<Physic::CollisionSystem3D> collisionSystem;
        std::unique_ptr<Manager::SoundManager> soundManager;
        glm::mat4 projection;
        std::unordered_map<std::string, std::shared_ptr<Scene>> nodes;
        std::weak_ptr<Scene> parent;
        int depth = 0;
        int width;
        int height;
        float deltaTime = 1;
        int renderPriority = 0;
        bool rendererAttached = false;
    };
} // Scene

#endif //SNAKE3_SCENE_H
