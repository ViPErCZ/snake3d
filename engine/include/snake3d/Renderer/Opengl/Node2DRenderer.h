#ifndef SNAKE3_NODE2DRENDERER_H
#define SNAKE3_NODE2DRENDERER_H

#include <memory>

#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/MeshNode2D.h>

namespace Renderer {
    class Node2DRenderer final : public BaseRenderer { // BaseRenderer2D
    public:
        Node2DRenderer(const std::shared_ptr<Manager::Camera> &camera, int width, int height);
        ~Node2DRenderer() override = default;
        void render3D(float dt, uint64_t frameId) override;
        void render2D(float dt, uint64_t frameId) override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void resize(int width, int height, const glm::mat4 &projection) override;
        void renderShadowMap() override {};
        void setRootNode(const std::shared_ptr<Model::MeshNode2D> &rootNode);
        std::shared_ptr<Model::MeshNode2D> getRootNode();

    protected:
        void renderScene() const;

        std::shared_ptr<Manager::Camera> camera;
        std::shared_ptr<Model::MeshNode2D> rootNode;
        glm::mat4 ortho{};
    };
} // Renderer

#endif //SNAKE3_NODE2DRENDERER_H
