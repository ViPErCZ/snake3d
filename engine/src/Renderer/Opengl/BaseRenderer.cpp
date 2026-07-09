#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Renderer/Opengl/Model/Standard/PlaneMesh.h>

Renderer::BaseRenderer::BaseRenderer() : shadows(false) {
}

Renderer::BaseRenderer::~BaseRenderer() = default;

void Renderer::BaseRenderer::setShadow(const bool shadow) {
    shadows = shadow;
}

bool Renderer::BaseRenderer::isShadow() const {
    return shadows;
}

glm::vec3 Renderer::BaseRenderer::compareSceneMin(const glm::vec3 sceneMin) {
    return sceneMin;
}

glm::vec3 Renderer::BaseRenderer::compareSceneMax(const glm::vec3 sceneMax) {
    return sceneMax;
}
