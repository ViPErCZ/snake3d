#ifndef SNAKE3_SCENE_H
#define SNAKE3_SCENE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "SceneRenderer.h"
#include "../../../Handler/Debug/ManipulatorHandler.h"
#include "../../../Manager/KeyboardManager.h"
#include "../../../Manager/RenderManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../../Physic/CollisionSystem3D.h"
#include "../../../Tools/BuildSettings.h"

using namespace std;
using namespace Model;
using namespace Manager;
using namespace Handler::Debug;
using namespace Build;

namespace Scenes {
    class Scene : public enable_shared_from_this<Scene>, public Named {
    public:
        virtual ~Scene();

        explicit Scene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        virtual void init(int priority);

        virtual void update();

        virtual void physics();

        virtual void render();

        virtual void resize(int width, int height, const glm::mat4 &projection);

        void addNode(const std::shared_ptr<Scene>& node);
        void addNode(const std::string &name, const std::shared_ptr<Scene> &node);
        [[nodiscard]] bool hasNode(const std::string &name) const;
        [[nodiscard]] shared_ptr<Scene> getNode(const std::string &name) const;
        bool removeNode(const std::string &name);
        bool replaceNode(const std::string &name, const std::shared_ptr<Scene> &node);
        void attachRenderer();
        void detachRenderer();
        [[nodiscard]] bool isRendererAttached() const;

        virtual void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods);

        void addMeshNode3D(shared_ptr<MeshNode3D> node, int priority = 0);
        void addMeshNode2D(shared_ptr<MeshNode2D> node, int priority = 0);
        
        vector<RendererEntry3D> getAllMeshNodes3D() const;

        void setEnvironment(const shared_ptr<Environment> &environment) {
            this->environment = environment;
        }

        void setCollisionSystem(const shared_ptr<CollisionSystem3D> &collisionSystem);

        void setManipulatorHandler(const shared_ptr<ManipulatorHandler> &manipulatorHandler);

        SoundManager &getSoundManager() const;

    protected:
        vector<RendererEntry3D> meshNode3d;
        vector<RendererEntry2D> meshNode2d;
        shared_ptr<Environment> environment;
        shared_ptr<DirectionalLight> directionalLight;
        vector<shared_ptr<SpotLight> > spotLights;
        vector<shared_ptr<PointLight> > pointLights;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<SceneRenderer> sceneRenderer;
        shared_ptr<RenderManager> rendererManager;
        unique_ptr<KeyboardManager> keyboardManager;
        shared_ptr<Camera> camera;
        shared_ptr<ContextState> contextState;
        shared_ptr<ManipulatorHandler> manipulatorHandler;
        shared_ptr<CollisionSystem3D> collisionSystem;
        unique_ptr<SoundManager> soundManager;
        glm::mat4 projection;
        unordered_map<string, shared_ptr<Scene>> nodes;
        weak_ptr<Scene> parent;
        int depth = 0;
        int width;
        int height;
        float deltaTime = 1;
        int renderPriority = 0;
        bool rendererAttached = false;
    };
} // Scene

#endif //SNAKE3_SCENE_H
