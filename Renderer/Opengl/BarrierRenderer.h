#ifndef SNAKE3_BARRIERRENDERER_H
#define SNAKE3_BARRIERRENDERER_H

#include "ObjWallRenderer.h"
#include "../../ItemsDto/Barriers.h"

namespace Renderer {

    class BarrierRenderer : public ObjWallRenderer {
    public:
        explicit BarrierRenderer(Barriers *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager);
    };

} // Renderer

#endif //SNAKE3_BARRIERRENDERER_H
