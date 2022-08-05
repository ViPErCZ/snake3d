#ifndef SNAKE3_SNAKERENDERER_H
#define SNAKE3_SNAKERENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Snake.h"

namespace Renderer {

    class SnakeRenderer : public BaseRenderer {
    public:
        explicit SnakeRenderer(Snake *item);
        ~SnakeRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Snake* snake;
    };

} // Renderer

#endif //SNAKE3_SNAKERENDERER_H
