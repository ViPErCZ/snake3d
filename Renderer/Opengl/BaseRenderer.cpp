#include "BaseRenderer.h"

namespace Manager {

} // Manager
Renderer::BaseRenderer::BaseRenderer(): item(nullptr), shadow(false) {}
Renderer::BaseRenderer::BaseRenderer(BaseItem *item) : item(item), shadow(false) {}

Renderer::BaseRenderer::~BaseRenderer() {
    delete item;
}

void Renderer::BaseRenderer::setShadow(bool shadow) {
    BaseRenderer::shadow = shadow;
}
