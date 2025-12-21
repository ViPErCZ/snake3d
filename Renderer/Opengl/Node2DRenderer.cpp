#include "Node2DRenderer.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Renderer {
    Node2DRenderer::Node2DRenderer(const shared_ptr<Camera> &camera, const int width, const int height)
        : camera(camera) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);
    }

    void Node2DRenderer::render3D(float dt, uint64_t frameId) {
        throw std::runtime_error("Not implemented. Use render2D() instead.");
    }

    void Node2DRenderer::render2D(const float dt) {
        rootNode->update(dt);
        this->beforeRender();
        renderScene();
        this->afterRender();
    }

    void Node2DRenderer::beforeRender() {
    }

    void Node2DRenderer::afterRender() {
    }

    void Node2DRenderer::setRootNode(const shared_ptr<MeshNode2D> &rootNode) {
        this->rootNode = rootNode;
    }

    shared_ptr<MeshNode2D> Node2DRenderer::getRootNode() {
        return rootNode;
    }

    void Node2DRenderer::renderScene() const {
        rootNode->render(camera, ortho, 1, glm::mat4(1));
    }
} // Renderer