#include "BarrierRenderer.h"

namespace Renderer {
    BarrierRenderer::BarrierRenderer(Barriers *item, ShaderManager* shader, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : ObjWallRenderer((ObjWall* )item, shader, camera, proj, resManager) {
    }
} // Renderer