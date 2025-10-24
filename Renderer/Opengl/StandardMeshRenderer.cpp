#include "StandardMeshRenderer.h"

namespace Renderer {
    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera,
                                               const glm::mat4 &projection,
                                               shared_ptr<StandardMesh> standardMesh)
        : camera(std::move(camera)),
          mesh(std::move(standardMesh)),
          projection(projection) {
        item = mesh->getBaseItem();
    }

    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera, const glm::mat4 &projection)
    : camera(std::move(camera)),
      projection(projection) {
    }

    StandardMeshRenderer::~StandardMeshRenderer() = default;

    void StandardMeshRenderer::render(const float dt) {
        mesh->update(dt);
        renderScene(nullptr);
    }

    void StandardMeshRenderer::beforeRender() {
    }

    void StandardMeshRenderer::afterRender() {
    }

    void StandardMeshRenderer::renderShadowMap() {
        mesh->renderShadowMap(camera, projection, 1);
    }

    void StandardMeshRenderer::setMesh(const shared_ptr<StandardMesh> &mesh) {
        this->mesh = mesh;
        this->item = mesh->getBaseItem();
    }

    void StandardMeshRenderer::renderScene(const shared_ptr<ShaderManager> &shader) const {
        mesh->render(camera, projection, 1);
    }

    shared_ptr<Mesh> StandardMeshRenderer::getMesh() {
        return mesh->getMesh();
    }
} // Renderer
