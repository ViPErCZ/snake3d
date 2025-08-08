#include "StandardMeshRenderer.h"

namespace Renderer {
    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera,
                                               shared_ptr<ShaderManager> baseShader,
                                               const glm::mat4 &projection,
                                               shared_ptr<StandardMesh> standardMesh)
        : camera(std::move(camera)),
          baseShader(std::move(baseShader)),
          mesh(std::move(standardMesh)),
          projection(projection) {
    }

    StandardMeshRenderer::~StandardMeshRenderer() = default;

    void StandardMeshRenderer::render(float dt) {
        renderScene(baseShader);
    }

    void StandardMeshRenderer::beforeRender() {
    }

    void StandardMeshRenderer::afterRender() {
    }

    void StandardMeshRenderer::renderShadowMap() {
    }

    void StandardMeshRenderer::setMaterial(const shared_ptr<BaseMaterial> &material) {
        this->material = material;
    }

    void StandardMeshRenderer::renderScene(const shared_ptr<ShaderManager> &shader) const {
        if (mesh->getBaseItem()->isVisible()) {
            glLoadIdentity();
            shader->use();
            shader->setMat4("view", camera->getViewMatrix());
            shader->setMat4("projection", this->projection);
            shader->setMat4("model", mesh->getBaseItem()->getWorldMatrix());
            shader->setBool("useMaterial", true);
            shader->setBool("useBones", false);
            mesh->getMesh()->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getMesh()->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Renderer
