#include "BarrierRenderer.h"

namespace Renderer {
    BarrierRenderer::BarrierRenderer(Snake* snake, Barriers *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : ObjWallRenderer(snake, reinterpret_cast<ObjWall *>(item), camera, proj, resManager) {
    }
} // Renderer