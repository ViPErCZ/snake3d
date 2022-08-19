#ifndef SNAKE3_EATREMOVEANIMATERENDERER_H
#define SNAKE3_EATREMOVEANIMATERENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Eat.h"

namespace Renderer {

    class EatRemoveAnimateRenderer : public BaseRenderer {
    public:
        explicit EatRemoveAnimateRenderer(Eat *eat);
        ~EatRemoveAnimateRenderer() override;
        [[nodiscard]] bool isCompleted() const;

        void setCompleted(bool completed);

        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        double lastTime{};
        Eat* eat;
        bool completed = false;
    };

} // Renderer

#endif //SNAKE3_EATREMOVEANIMATERENDERER_H
