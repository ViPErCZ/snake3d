#include "BaseRenderer.h"

#include "Model/Standard/PlaneMesh.h"

Renderer::BaseRenderer::BaseRenderer() : item(nullptr), shadow(false), fog(false) {
}

Renderer::BaseRenderer::BaseRenderer(BaseItem *item) : item(item), shadow(false), fog(false) {
}

Renderer::BaseRenderer::~BaseRenderer() = default;

void Renderer::BaseRenderer::setShadow(const bool shadow) {
    BaseRenderer::shadow = shadow;
}

bool Renderer::BaseRenderer::isShadow() const {
    return shadow;
}

void Renderer::BaseRenderer::setFog(bool fog) {
    BaseRenderer::fog = fog;
}

bool Renderer::BaseRenderer::isFog() const {
    return fog;
}

bool Renderer::BaseRenderer::isPlane() {
    return false;
}

glm::vec3 Renderer::BaseRenderer::compareSceneMin(const glm::vec3 sceneMin) {
    const auto localMesh = getMesh();
    if (localMesh == nullptr) {
        return sceneMin;
    }

    return glm::min(sceneMin, localMesh->getMin(item->getModelMatrix()));
}

glm::vec3 Renderer::BaseRenderer::compareSceneMax(const glm::vec3 sceneMax) {
    const auto localMesh = getMesh();
    if (localMesh == nullptr) {
        return sceneMax;
    }
    return glm::max(sceneMax, localMesh->getMax(item->getModelMatrix()));
}

shared_ptr<Mesh> Renderer::BaseRenderer::getMesh() {
    return nullptr;
}
