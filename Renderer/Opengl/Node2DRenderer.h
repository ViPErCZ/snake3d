#ifndef SNAKE3_NODE2DRENDERER_H
#define SNAKE3_NODE2DRENDERER_H

#include <memory>

#include "BaseRenderer.h"
#include "../../Manager/Camera.h"
#include "Model/Standard/2D/MeshNode2D.h"

using namespace Manager;
using namespace Model;
using namespace std;

namespace Renderer {
    class Node2DRenderer final : public BaseRenderer { // BaseRenderer2D
    public:
        Node2DRenderer(const shared_ptr<Camera> &camera, int width, int height);
        ~Node2DRenderer() override = default;
        void render3D(float dt, uint64_t frameId) override;
        void render2D(float dt, uint64_t frameId) override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void renderShadowMap() override {};
        void setRootNode(const shared_ptr<MeshNode2D> &rootNode);
        shared_ptr<MeshNode2D> getRootNode();

    protected:
        void renderScene() const;

        shared_ptr<Camera> camera;
        shared_ptr<MeshNode2D> rootNode;
        glm::mat4 ortho{};
    };
} // Renderer

#endif //SNAKE3_NODE2DRENDERER_H