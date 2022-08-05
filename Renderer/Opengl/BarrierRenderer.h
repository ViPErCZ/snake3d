#ifndef SNAKE3_BARRIERRENDERER_H
#define SNAKE3_BARRIERRENDERER_H

#include "WallRenderer.h"
#include "../../ItemsDto/Barriers.h"

namespace Renderer {

    class BarrierRenderer : public WallRenderer {
    public:
        explicit BarrierRenderer(Barriers *item);
    };

} // Renderer

#endif //SNAKE3_BARRIERRENDERER_H
