#ifndef SNAKE3_GAMEFIELDRENDERER_H
#define SNAKE3_GAMEFIELDRENDERER_H

#include "../../ItemsDto/GameField.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>

using namespace ItemsDto;

namespace Renderer {
    class GameFieldRenderer : public BaseRenderer {
    public:
        explicit GameFieldRenderer(GameField *item);
        ~GameFieldRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        GameField* gameField;
    };
}

#endif //SNAKE3_GAMEFIELDRENDERER_H
