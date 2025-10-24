#include "SceneRenderer.h"

namespace Scenes {
    SceneRenderer::SceneRenderer(const shared_ptr<Camera> &camera, const glm::mat4 &projection)
        : camera(camera), projection(projection) {
        meshRenderer = make_unique<StandardMeshRenderer>(camera, projection);
    }

    SceneRenderer::~SceneRenderer() = default;

    void SceneRenderer::update(const vector<shared_ptr<StandardMesh> > &meshes) {
        this->meshes = meshes;
    }

    void SceneRenderer::render(const float dt) {
        for (auto &mesh : meshes) {
            this->meshRenderer->setMesh(mesh);
            this->meshRenderer->render(dt);
        }
    }

    void SceneRenderer::renderShadowMap() {
        for (auto &mesh : meshes) {
            this->meshRenderer->setMesh(mesh);
            this->meshRenderer->renderShadowMap();
        }
    }

    void SceneRenderer::beforeRender() {
    }

    void SceneRenderer::afterRender() {
    }
} // Scene
