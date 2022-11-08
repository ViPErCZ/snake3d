#include "BarrierRenderer.h"

namespace Renderer {
    BarrierRenderer::BarrierRenderer(Barriers *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : ObjWallRenderer((ObjWall* )item, camera, proj, resManager) {
    }
} // Renderer