#include "BaseRenderer.h"
#include "Model/Standard/PlaneMesh.h"

Renderer::BaseRenderer::BaseRenderer() : shadows(false), fog(false) {
}

Renderer::BaseRenderer::~BaseRenderer() = default;

void Renderer::BaseRenderer::setShadow(const bool shadow) {
    shadows = shadow;
}

bool Renderer::BaseRenderer::isShadow() const {
    return shadows;
}

void Renderer::BaseRenderer::setFog(bool fog) {
    BaseRenderer::fog = fog;
}

bool Renderer::BaseRenderer::isFog() const {
    return fog;
}

glm::vec3 Renderer::BaseRenderer::compareSceneMin(const glm::vec3 sceneMin) {
    return sceneMin;
}

glm::vec3 Renderer::BaseRenderer::compareSceneMax(const glm::vec3 sceneMax) {
    return sceneMax;
}
