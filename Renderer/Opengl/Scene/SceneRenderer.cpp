#include "SceneRenderer.h"

namespace Scenes {
    SceneRenderer::SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection)
        : camera(camera), projection(projection) {
        meshRenderer = make_unique<StandardMeshRenderer>(camera, projection);
    }

    SceneRenderer::~SceneRenderer() = default;

    void SceneRenderer::update(const vector<shared_ptr<MeshNode3D> > &nodes) {
        this->nodes = nodes;
    }

    void SceneRenderer::render(const float dt) {
        for (auto &node : nodes) {
            this->meshRenderer->setRootNode(node);
            this->meshRenderer->render(dt);
        }
    }

    void SceneRenderer::renderShadowMap() {
        for (auto &node : nodes) {
            this->meshRenderer->setRootNode(node);
            this->meshRenderer->renderShadowMap();
        }
    }

    void SceneRenderer::beforeRender() {
    }

    void SceneRenderer::afterRender() {
    }

    void SceneRenderer::setShadow(const bool shadow) {
        BaseRenderer::setShadow(shadow);
        this->meshRenderer->setShadow(shadow);
    }
} // Scene
