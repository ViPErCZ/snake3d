#ifndef SNAKE3_STANDARDMESHRENDERER_H
#define SNAKE3_STANDARDMESHRENDERER_H

#include <memory>
#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Renderer {
    class Node3DRenderer final : public BaseRenderer {
    public:
        explicit Node3DRenderer(std::shared_ptr<Manager::Camera> camera,
                                      const glm::mat4 &projection,
                                      std::shared_ptr<Model::MeshNode3D> rootNode);

        Node3DRenderer(std::shared_ptr<Manager::Camera> camera,
                                      const glm::mat4 &projection);

        ~Node3DRenderer() override;

        void render3D(float dt, uint64_t frameId) override;

        void beforeRender(MODE mode) override;

        void afterRender() override;
        void resize(int width, int height, const glm::mat4 &projection) override;

        void renderShadowMap() override;

        void setMesh(const std::shared_ptr<Model::StandardMesh> &mesh);

        void setRootNode(const std::shared_ptr<Model::MeshNode3D> &rootNode);

        std::shared_ptr<Model::MeshNode3D> getRootNode();

    protected:
        void renderScene() const;

        std::shared_ptr<Manager::Camera> camera;
        std::shared_ptr<Model::MeshNode3D> rootNode;
        glm::mat4 projection;
    };
} // Renderer

#endif //SNAKE3_STANDARDMESHRENDERER_H
