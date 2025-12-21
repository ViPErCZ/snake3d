#ifndef SNAKE3_SCENERENDERER_H
#define SNAKE3_SCENERENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../Model/Standard/StandardMesh.h"
#include "../../../Manager/Camera.h"
#include "../BaseRenderer.h"
#include "../Node2DRenderer.h"
#include "../Node3DRenderer.h"
#include "../Model/Standard/2D/MeshNode2D.h"

using namespace std;
using namespace Renderer;
using namespace Model;
using namespace Manager;

namespace Scenes {
    struct RendererEntry3D {
        shared_ptr<MeshNode3D> node;
        int priority;
    };
    struct RendererEntry2D {
        shared_ptr<MeshNode2D> node;
        int priority;
    };
    class SceneRenderer final : public BaseRenderer {
    public:
        SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection, int width, int height);

        ~SceneRenderer() override;

        void update(const vector<RendererEntry3D> &nodes, const vector<RendererEntry2D> &nodes2d);

        void render3D(float dt, uint64_t frameId) override;

        void render2D(float dt) override;

        void renderShadowMap() override;

        void beforeRender() override;

        void afterRender() override;

        void setShadow(bool shadow) override;

    private:
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        vector<RendererEntry3D> nodes3d;
        vector<RendererEntry2D> nodes2d;
        unique_ptr<Node3DRenderer> meshNode3DRenderer;
        unique_ptr<Node2DRenderer> meshNode2DRenderer;
    };
} // Scene

#endif //SNAKE3_SCENERENDERER_H
