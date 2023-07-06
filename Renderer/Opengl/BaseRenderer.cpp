#include "BaseRenderer.h"

namespace Manager {

} // Manager
Renderer::BaseRenderer::BaseRenderer(): item(nullptr), shadow(false), fog(false) {}
Renderer::BaseRenderer::BaseRenderer(BaseItem *item) : item(item), shadow(false), fog(false) {}

Renderer::BaseRenderer::~BaseRenderer() {
    delete item;
}

void Renderer::BaseRenderer::setShadow(bool shadow) {
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
