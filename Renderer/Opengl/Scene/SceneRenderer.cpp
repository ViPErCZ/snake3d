#include "SceneRenderer.h"

namespace Scenes {
    SceneRenderer::SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection, int width, int height)
        : camera(camera), projection(projection) {
        meshNode3DRenderer = make_unique<Node3DRenderer>(camera, projection);
        meshNode2DRenderer = make_unique<Node2DRenderer>(camera, width, height);
    }

    SceneRenderer::~SceneRenderer() = default;

    void SceneRenderer::update(const vector<RendererEntry3D> &nodes, const vector<RendererEntry2D> &nodes2d) {
        this->nodes2d = nodes2d;
        this->nodes3d = nodes;
    }

    void SceneRenderer::render3D(const float dt, const uint64_t frameId) {
        for (auto &node : nodes3d) {
            this->meshNode3DRenderer->beforeRender(this->mode);
            this->meshNode3DRenderer->setRootNode(node.node);
            this->meshNode3DRenderer->render3D(dt, frameId);
        }
    }

    void SceneRenderer::render2D(const float dt, const uint64_t frameId) {
        BaseRenderer::render2D(dt, frameId);

        for (auto &node : nodes2d) {
            this->meshNode2DRenderer->beforeRender(this->mode);
            this->meshNode2DRenderer->setRootNode(node.node);
            this->meshNode2DRenderer->render2D(dt, frameId);
        }
    }

    void SceneRenderer::renderShadowMap() {
        for (auto &node : nodes3d) {
            this->meshNode3DRenderer->setRootNode(node.node);
            this->meshNode3DRenderer->renderShadowMap();
        }
    }

    void SceneRenderer::beforeRender(const MODE mode) {
        this->mode = mode;
    }

    void SceneRenderer::afterRender() {
    }

    void SceneRenderer::setShadow(const bool shadow) {
        BaseRenderer::setShadow(shadow);
        this->meshNode3DRenderer->setShadow(shadow);
    }
} // Scene
