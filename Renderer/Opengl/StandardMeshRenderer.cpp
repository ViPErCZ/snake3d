#include "StandardMeshRenderer.h"

namespace Renderer {
    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera,
                                               const glm::mat4 &projection,
                                               shared_ptr<StandardMesh> standardMesh)
        : camera(std::move(camera)),
          mesh(std::move(standardMesh)),
          projection(projection) {
        item = mesh->getBaseItem().get();
    }

    StandardMeshRenderer::~StandardMeshRenderer() = default;

    void StandardMeshRenderer::render(float dt) {
        renderScene(nullptr);
    }

    void StandardMeshRenderer::beforeRender() {
    }

    void StandardMeshRenderer::afterRender() {
    }

    void StandardMeshRenderer::renderShadowMap() {
        mesh->renderShadowMap(camera, projection, 1);
    }

    void StandardMeshRenderer::renderScene(const shared_ptr<ShaderManager> &shader) const {
        mesh->render(camera, projection, 1);
    }

    shared_ptr<Mesh> StandardMeshRenderer::getMesh() {
        return mesh->getMesh();
    }
} // Renderer
