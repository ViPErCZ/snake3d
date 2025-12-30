#ifndef SNAKE3_SCENE_H
#define SNAKE3_SCENE_H

#include <memory>
#include <vector>
#include "SceneRenderer.h"
#include "../../../Manager/KeyboardManager.h"
#include "../../../Manager/RenderManager.h"
#include "../../../Manager/ResourceManager.h"

using namespace std;
using namespace Model;
using namespace Manager;

namespace Scenes {
    class Scene : public enable_shared_from_this<Scene> {
    public:
        virtual ~Scene();

        explicit Scene(const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        virtual void init(int priority);

        virtual void update();

        virtual void render();

        void addNode(const std::shared_ptr<Scene>& node);

        virtual void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const;

        void addMeshNode3D(shared_ptr<MeshNode3D> node, int priority = 0);
        void addMeshNode2D(shared_ptr<MeshNode2D> node, int priority = 0);
        
        vector<RendererEntry3D> getAllMeshNodes3D() const;

    protected:
        vector<RendererEntry3D> meshNode3d;
        vector<RendererEntry2D> meshNode2d;
        shared_ptr<DirectionalLight> directionalLight;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<SceneRenderer> sceneRenderer;
        shared_ptr<RenderManager> rendererManager;
        unique_ptr<KeyboardManager> keyboardManager;
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        vector<shared_ptr<Scene>> nodes;
        weak_ptr<Scene> parent;
        int depth = 0;
        int width;
        int height;
    };
} // Scene

#endif //SNAKE3_SCENE_H
