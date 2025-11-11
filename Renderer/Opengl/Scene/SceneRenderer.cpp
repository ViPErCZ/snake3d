#include "SceneRenderer.h"

namespace Scenes {
    SceneRenderer::SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection, int width, int height)
        : camera(camera), projection(projection) {
        meshNode3DRenderer = make_unique<Node3DRenderer>(camera, projection);
        meshNode2DRenderer = make_unique<Node2DRenderer>(camera, width, height);
    }

    SceneRenderer::~SceneRenderer() = default;

    void SceneRenderer::update(const vector<shared_ptr<MeshNode3D> > &nodes, const vector<shared_ptr<MeshNode2D> > &nodes2d) {
        this->nodes2d = nodes2d;
        this->nodes3d = nodes;
    }

    void SceneRenderer::render3D(const float dt) {
        for (auto &node : nodes3d) {
            this->meshNode3DRenderer->setRootNode(node);
            this->meshNode3DRenderer->render3D(dt);
        }
    }

    void SceneRenderer::render2D(const float dt) {
        BaseRenderer::render2D(dt);

        for (auto &node : nodes2d) {
            this->meshNode2DRenderer->setRootNode(node);
            this->meshNode2DRenderer->render2D(dt);
        }
    }

    void SceneRenderer::renderShadowMap() {
        for (auto &node : nodes3d) {
            this->meshNode3DRenderer->setRootNode(node);
            this->meshNode3DRenderer->renderShadowMap();
        }
    }

    void SceneRenderer::beforeRender() {
    }

    void SceneRenderer::afterRender() {
    }

    void SceneRenderer::setShadow(const bool shadow) {
        BaseRenderer::setShadow(shadow);
        this->meshNode3DRenderer->setShadow(shadow);
    }
} // Scene
