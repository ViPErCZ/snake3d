#include "BaseRenderer.h"

namespace Manager {

} // Manager
Renderer::BaseRenderer::BaseRenderer(): item(nullptr), ortho(false) {}
Renderer::BaseRenderer::BaseRenderer(BaseItem *item) : item(item), ortho(false) {}

Renderer::BaseRenderer::~BaseRenderer() {
    delete item;
}
