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
        virtual void renderShadowMap() = 0;
        virtual void beforeRender() = 0;
        virtual void afterRender() = 0;
        void setShadow(bool shadow);
        [[nodiscard]] bool isShadow() const;
        void setFog(bool fog);
        [[nodiscard]] bool isFog() const;

    protected:
        BaseItem* item{};
        bool shadow;
        bool fog;
    };

} // Manager

#endif //SNAKE3_BASERENDERER_H
