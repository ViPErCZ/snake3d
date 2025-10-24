#include "BarrierRenderer.h"

namespace Renderer {
    BarrierRenderer::BarrierRenderer(const shared_ptr<Snake> &snake, const shared_ptr<Barriers> &item, Camera* camera, const glm::mat4 &proj, ResourceManager* resManager)
        : ObjWallRenderer(snake, std::static_pointer_cast<ObjWall>(item), camera, proj, resManager) {
    }
} // Renderer