#ifndef SNAKE3_BASERENDERER_H
#define SNAKE3_BASERENDERER_H

#include "../../ItemsDto/BaseItem.h"

using namespace ItemsDto;

namespace Renderer {

    class BaseRenderer {
    public:
        BaseRenderer();
        explicit BaseRenderer(BaseItem *item);
        virtual ~BaseRenderer();
        virtual void render() = 0;
        virtual void beforeRender() = 0;
        virtual void afterRender() = 0;

    protected:
        BaseItem* item{};
        bool ortho;
    };

} // Manager

#endif //SNAKE3_BASERENDERER_H
