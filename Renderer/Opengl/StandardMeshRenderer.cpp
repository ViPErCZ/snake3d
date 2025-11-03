#include "StandardMeshRenderer.h"

namespace Renderer {
    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera,
                                      const glm::mat4 &projection,
                                      shared_ptr<MeshNode3D> rootNode)
        : camera(std::move(camera)),
          rootNode(std::move(rootNode)),
          projection(projection) {
    }

    StandardMeshRenderer::StandardMeshRenderer(shared_ptr<Camera> camera, const glm::mat4 &projection)
    : camera(std::move(camera)),
      projection(projection) {
    }

    StandardMeshRenderer::~StandardMeshRenderer() = default;

    void StandardMeshRenderer::render(const float dt) {
        rootNode->update(dt);
        renderScene(nullptr);
    }

    void StandardMeshRenderer::beforeRender() {
    }

    void StandardMeshRenderer::afterRender() {
    }

    void StandardMeshRenderer::renderShadowMap() {
        if (shadows) {
            rootNode->renderShadows(camera, projection, 1, glm::mat4(1));
        }
    }

    void StandardMeshRenderer::setRootNode(const shared_ptr<MeshNode3D> &rootNode) {
        this->rootNode = rootNode;
    }

    shared_ptr<MeshNode3D> StandardMeshRenderer::getRootNode() {
        return rootNode;
    }

    void StandardMeshRenderer::renderScene(const shared_ptr<ShaderManager> &shader) const {
        rootNode->render(camera, projection, 1, glm::mat4(1), shadows);
    }

    shared_ptr<Mesh> StandardMeshRenderer::getMesh() {
        throw std::runtime_error("StandardMeshRenderer::getMesh() not implemented. Deprecated.");
    }
} // Renderer
