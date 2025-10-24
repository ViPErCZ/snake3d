#ifndef SNAKE3_BARRIERRENDERER_H
#define SNAKE3_BARRIERRENDERER_H

#include "ObjWallRenderer.h"
#include "../../ItemsDto/Barriers.h"

namespace Renderer {
    class BarrierRenderer : public ObjWallRenderer {
    public:
        explicit BarrierRenderer(const shared_ptr<Snake> &snake, const shared_ptr<Barriers> &item, Camera *camera,
                                 const glm::mat4 &proj, ResourceManager *resManager);
    };
} // Renderer

#endif //SNAKE3_BARRIERRENDERER_H
