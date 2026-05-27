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

namespace Scenes {
    struct RendererEntry3D {
        std::shared_ptr<Model::MeshNode3D> node;
        int priority;
    };
    struct RendererEntry2D {
        std::shared_ptr<Model::MeshNode2D> node;
        int priority;
    };
    class SceneRenderer final : public Renderer::BaseRenderer {
    public:
        SceneRenderer(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, int width, int height);

        ~SceneRenderer() override;

        void update(const std::vector<RendererEntry3D> &nodes, const std::vector<RendererEntry2D> &nodes2d);

        void render3D(float dt, uint64_t frameId) override;

        void render2D(float dt, uint64_t frameId) override;

        void renderShadowMap() override;

        void beforeRender(Renderer::MODE mode) override;

        void afterRender() override;

        void setShadow(bool shadow) override;
        void resize(int width, int height, const glm::mat4 &projection) override;

    private:
        std::shared_ptr<Manager::Camera> camera;
        glm::mat4 projection;
        std::vector<RendererEntry3D> nodes3d;
        std::vector<RendererEntry2D> nodes2d;
        std::unique_ptr<Renderer::Node3DRenderer> meshNode3DRenderer;
        std::unique_ptr<Renderer::Node2DRenderer> meshNode2DRenderer;
    };
} // Scene

#endif //SNAKE3_SCENERENDERER_H
