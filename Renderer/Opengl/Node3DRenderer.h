#ifndef SNAKE3_STANDARDMESHRENDERER_H
#define SNAKE3_STANDARDMESHRENDERER_H

#include <memory>
#include "BaseRenderer.h"
#include "Model/Standard/StandardMesh.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "Model/Standard/MeshNode3D.h"

using namespace Model;
using namespace std;

namespace Renderer {
    class Node3DRenderer final : public BaseRenderer {
    public:
        explicit Node3DRenderer(shared_ptr<Camera> camera,
                                      const glm::mat4 &projection,
                                      shared_ptr<MeshNode3D> rootNode);

        Node3DRenderer(shared_ptr<Camera> camera,
                                      const glm::mat4 &projection);

        ~Node3DRenderer() override;

        void render3D(float dt, uint64_t frameId) override;

        void beforeRender() override;

        void afterRender() override;

        void renderShadowMap() override;

        void setMesh(const shared_ptr<StandardMesh> &mesh);

        void setRootNode(const shared_ptr<MeshNode3D> &rootNode);

        shared_ptr<MeshNode3D> getRootNode();

    protected:
        void renderScene() const;

        shared_ptr<Camera> camera;
        shared_ptr<MeshNode3D> rootNode;
        glm::mat4 projection;
    };
} // Renderer

#endif //SNAKE3_STANDARDMESHRENDERER_H
