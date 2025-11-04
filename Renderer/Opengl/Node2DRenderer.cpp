#include "Node2DRenderer.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Renderer {
    Node2DRenderer::Node2DRenderer(const std::shared_ptr<Camera> &camera, const int width, const int height)
        : camera(camera) {
        ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    }

    void Node2DRenderer::render(float dt) {
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
} // Renderer