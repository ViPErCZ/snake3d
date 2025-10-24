#ifndef SNAKE3_SCENE_H
#define SNAKE3_SCENE_H

#include <memory>
#include <vector>
#include "SceneRenderer.h"
#include "../../../Manager/KeyboardManager.h"
#include "../../../Manager/RenderManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../Model/Standard/StandardMesh.h"

using namespace std;
using namespace Model;
using namespace Manager;

namespace Scenes {
    class Scene {
    public:
        virtual ~Scene();

        explicit Scene(const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        virtual void init();

        virtual void update();

        virtual void render();

        virtual void keyboardInput(GLFWwindow *window, int keyCode, int scancode, int action, int mods) const;

    protected:
        vector<shared_ptr<StandardMesh> > meshes;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<SceneRenderer> sceneRenderer;
        shared_ptr<RenderManager> rendererManager;
        unique_ptr<KeyboardManager> keyboardManager;
        shared_ptr<Camera> camera;
        glm::mat4 projection;
    };
} // Scene

#endif //SNAKE3_SCENE_H
