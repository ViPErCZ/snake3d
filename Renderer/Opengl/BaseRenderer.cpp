#include "BaseRenderer.h"

namespace Manager {

} // Manager
Renderer::BaseRenderer::BaseRenderer(): item(nullptr), ortho(false) {}
Renderer::BaseRenderer::BaseRenderer(BaseItem *item) : item(item), ortho(false) {}

Renderer::BaseRenderer::~BaseRenderer() {
    delete item;
}

bool Renderer::BaseRenderer::isOrtho() const {
    return ortho;
}
