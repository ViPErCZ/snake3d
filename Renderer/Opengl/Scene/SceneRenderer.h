#ifndef SNAKE3_SCENERENDERER_H
#define SNAKE3_SCENERENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../Model/Standard/StandardMesh.h"
#include "../../../Manager/Camera.h"
#include "../BaseRenderer.h"
#include "../StandardMeshRenderer.h"

using namespace std;
using namespace Renderer;
using namespace Model;
using namespace Manager;

namespace Scenes {
    class SceneRenderer final : public BaseRenderer {
    public:
        SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection);

        ~SceneRenderer() override;

        void update(const vector<shared_ptr<StandardMesh> > &meshes);

        void render(float dt) override;

        void renderShadowMap() override;

        void beforeRender() override;

        void afterRender() override;

    private:
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        vector<shared_ptr<StandardMesh> > meshes;
        unique_ptr<StandardMeshRenderer> meshRenderer;
    };
} // Scene

#endif //SNAKE3_SCENERENDERER_H
